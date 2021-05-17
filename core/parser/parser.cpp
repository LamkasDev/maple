#pragma once
#include <string>
using namespace std;

class Parser {
    public:
        list<Token*> tokens;
        int index = 0;
        Token* current_t;

        void init(list<Token*> _tokens) {
            tokens = _tokens;
            index = 1;
            advance();
        }

        int advance() {
            index++;
            if(tokens.size() > 0) {
                current_t = tokens.front();
                tokens.pop_front();
            }

            return 1;
        }

        ParserResult parse() {
            ParserResult res = expression();
            if(res.state != -1 && current_t->type != TT_EOF) {
                InvalidSyntaxError e;
                e.init(current_t->start, current_t->end, "Expected '+', '-', '*' or '/'");
                return res.failure(e);
            }

            return res;
        }

        ParserResult atom() {
            ParserResult result;
            Token* t = current_t;
            if(t->type == TT_INT) {
                result.register_advance(advance());
                Node* n = new Node(); n->set_value(t->value_int);

                result.set_node(n);
                return result.success();
            } else if(t->type == TT_FLOAT) {
                result.register_advance(advance());
                Node* n = new Node(); n->set_value(t->value_float);

                result.set_node(n);
                return result.success();
            } else if(t->type == TT_IDENFIFIER) {
                result.register_advance(advance());
                Node* n = new Node(); n->set_type(NODE_ACCESS); n->set_token(t);

                result.set_node(n);
                return result.success();
            } else if(t->type == TT_LPAREN) {
                result.register_advance(advance());
                ParserResult n_0 = expression();
                if(n_0.state == -1) { return result.failure(n_0.e); }
                if(current_t->type == TT_RPAREN) {
                    result.register_advance(advance());

                    result.set_node(n_0.node);
                    return result.success();
                } else {
                    InvalidSyntaxError e;
                    e.init(current_t->start, current_t->end, "Expected ')'");
                    return result.failure(e);
                }
            }

            InvalidSyntaxError e;
            e.init(t->start, t->end, "Expected int/float, identifier, '+', '-' or '('");
            return result.failure(e);
        }

        ParserResult power() {
            ParserResult result;
            
            ParserResult left = result.register_result(atom());
            if(result.state == -1) { return result; }

            Node* op = new Node();
            op->set_type(NODE_BINARY);
            op->set_to_left(left.node);

            while(current_t->type == TT_POW) {
                Token* op_token = current_t;
                op->set_token(op_token);
                result.register_advance(advance());

                ParserResult right = result.register_result(factor());
                if(result.state == -1) {
                    break;
                }

                if(op->right != nullptr) {
                    Node* copy = op->copy();
                    op->set_to_left(copy);
                }
                op->set_to_right(right.node);
            }
            if(result.state == -1) { return result; }
            result.set_node(op);

            return result.success();
        }

        ParserResult factor() {
            ParserResult result;
            Token* t = current_t;
            if(t->type == TT_PLUS || t->type == TT_MINUS) {
                result.register_advance(advance());
                ParserResult n_0 = factor();
                if(n_0.state == -1) { return result.failure(n_0.e); }
                Node* n = new Node(); n->set_type(NODE_UNARY); n->set_token(t); n->set_to_left(n_0.node);

                result.set_node(n);
                return result.success();
            }

            return power();
        }

        ParserResult term() {
            ParserResult result;

            ParserResult left = result.register_result(factor());
            if(result.state == -1) { return result; }

            Node* op = new Node();
            op->set_type(NODE_BINARY);
            op->set_to_left(left.node);

            while(current_t->type == TT_MUL || current_t->type == TT_DIV || current_t->type == TT_MOD) {
                Token* op_token = current_t;
                op->set_token(op_token);
                result.register_advance(advance());

                ParserResult right = result.register_result(factor());
                if(result.state == -1) {
                    break;
                }

                if(op->right != nullptr) {
                    Node* copy = op->copy();
                    op->set_to_left(copy);
                }
                op->set_to_right(right.node);
            }
            if(result.state == -1) { return result; }
            result.set_node(op);

            return result.success();
        }

        ParserResult arith_expr() {
            ParserResult result;

            ParserResult left = result.register_result(term());
            if(result.state == -1) { return result; }

            Node* op = new Node();
            op->set_type(NODE_BINARY);
            op->set_to_left(left.node);

            while(current_t->type == TT_PLUS || current_t->type == TT_MINUS) {
                Token* op_token = current_t;
                op->set_token(op_token);
                result.register_advance(advance());

                ParserResult right = result.register_result(term());
                if(result.state == -1) {
                    break;
                }

                if(op->right != nullptr) {
                    Node* copy = op->copy();
                    op->set_to_left(copy);
                }
                op->set_to_right(right.node);
            }
            if(result.state == -1) { return result; }
            result.set_node(op);

            return result.success();
        }

        ParserResult arith_expr_pre() {
            ParserResult result;

            ParserResult left = result.register_result(arith_expr());
            if(result.state == -1) { return result; }

            Node* op = new Node();
            op->set_type(NODE_BINARY);
            op->set_to_left(left.node);

            while(current_t->type == TT_EQEQ || current_t->type == TT_NEQ || current_t->type == TT_LTHAN || current_t->type == TT_GTHAN || current_t->type == TT_LTHANEQ || current_t->type == TT_GTHANEQ) {
                Token* op_token = current_t;
                op->set_token(op_token);
                result.register_advance(advance());

                ParserResult right = result.register_result(term());
                if(result.state == -1) {
                    break;
                }

                if(op->right != nullptr) {
                    Node* copy = op->copy();
                    op->set_to_left(copy);
                }
                op->set_to_right(right.node);
            }
            if(result.state == -1) { return result; }
            result.set_node(op);

            return result.success();
        }

        ParserResult comp_expr() {
            ParserResult result;

            if(current_t->matches(TT_KEYWORD, KEYWORD_NOT)) {
                Token* op_token = current_t;
                result.register_advance(advance());

                ParserResult expr = result.register_result(comp_expr());
                if(result.state == -1) { return result; }
                Node* n = new Node(); n->set_type(NODE_UNARY); n->set_token(op_token); n->set_to_left(expr.node);

                result.set_node(n);
                return result.success();
            }

            ParserResult arith = result.register_result(arith_expr_pre());
            if(result.state == -1) {
                InvalidSyntaxError e;
                e.init(current_t->start, current_t->end, "Expected int/float, identifier, '+', '-', '(' or 'NOT'");
                return result.failure(e);
            }
            result.set_node(arith.node);

            return result.success();
        }
        
        ParserResult expression() {
            ParserResult result;

            if(current_t->matches(TT_KEYWORD, KEYWORD_VAR)) {
                result.register_advance(advance());
                if(current_t->type != TT_IDENFIFIER) {
                    InvalidSyntaxError e;
                    e.init(current_t->start, current_t->end, "Expected identifier");
                    return result.failure(e);
                }

                Token* identifier = current_t;
                result.register_advance(advance());

                if(current_t->type != TT_EQ) {
                    InvalidSyntaxError e;
                    e.init(current_t->start, current_t->end, "Expected '='");
                    return result.failure(e);
                }

                result.register_advance(advance());
                ParserResult expr = result.register_result(expression());
                if(result.state == -1) { return result; }
                if(expr.node_type == NODE_UNKNOWN) {
                    InvalidSyntaxError e;
                    e.init(current_t->start, current_t->end, "Expected int/float, identifier, '+', '-' or '('");
                    return result.failure(e);
                }

                Node* n = new Node(); n->set_type(NODE_ASSIGNMENT); n->set_token(identifier); n->set_to_right(expr.node);
                result.set_node(n);
                
                return result.success();
            }

            ParserResult left = result.register_result(comp_expr());
            if(result.state == -1) {
                InvalidSyntaxError e;
                e.init(current_t->start, current_t->end, "Expected 'VAR', int/float, identifier, '+', '-' or '(' or 'NOT'");
                return result.failure(e);
            }

            Node* op = new Node();
            op->set_type(NODE_BINARY);
            op->set_to_left(left.node);
            
            while(current_t->matches(TT_KEYWORD, KEYWORD_AND) || current_t->matches(TT_KEYWORD, KEYWORD_OR)) {
                Token* op_token = current_t;
                op->set_token(op_token);
                result.register_advance(advance());

                ParserResult right = result.register_result(comp_expr());
                if(result.state == -1) {
                    break;
                }

                if(op->right != nullptr) {
                    Node* copy = op->copy();
                    op->set_to_left(copy);
                }
                op->set_to_right(right.node);
            }
            if(result.state == -1) { return result; }
            result.set_node(op);

            return result.success();
        }
};