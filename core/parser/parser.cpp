#pragma once
#include <string>
#include "../token.cpp"
using namespace std;

class Parser {
    public:
        list<Token*> tokens;
        int index = 0;
        Token* current_t = nullptr;

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
                e.init(current_t->start, current_t->end, "Expected '+', '-', '*', '/', '^', '==', '!=', '<', '>', '<=', '>=', 'AND' or 'OR'");
                return res.failure(e);
            }

            return res;
        }

        ParserResult atom() {
            ParserResult result;
            Token* t = current_t;
            if(t->type == TT_INT) {
                result.register_advance(advance());
                Node* n = new Node(); n->set_pos(t->start, t->end); n->set_value(t->value_int);

                result.set_node(n);
                return result.success();
            } else if(t->type == TT_FLOAT) {
                result.register_advance(advance());
                Node* n = new Node(); n->set_pos(t->start, t->end); n->set_value(t->value_float);

                result.set_node(n);
                return result.success();
            } else if(t->type == TT_STRING) {
                result.register_advance(advance());
                Node* n = new Node(); n->set_pos(t->start, t->end); n->set_value(t->value_string);

                result.set_node(n);
                return result.success();
            } else if(t->type == TT_IDENFIFIER) {
                result.register_advance(advance());
                Node* n = new Node(); n->set_pos(t->start, t->end); n->set_type(NODE_ACCESS); n->set_token(t);

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
            } else if(t->matches(TT_KEYWORD, KEYWORD_IF)) {
                ParserResult expr = if_expr();
                if(expr.state == -1) { return result.failure(expr.e); }

                result.set_node(expr.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_FOR)) {
                ParserResult expr = for_expr();
                if(expr.state == -1) { return result.failure(expr.e); }

                result.set_node(expr.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_WHILE)) {
                ParserResult expr = while_expr();
                if(expr.state == -1) { return result.failure(expr.e); }

                result.set_node(expr.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_FUNC)) {
                ParserResult def = func_def();
                if(def.state == -1) { return result.failure(def.e); }

                result.set_node(def.node);
                return result.success();
            }

            InvalidSyntaxError e;
            e.init(t->start, t->end, "Expected int/float, identifier, '+', '-', '(', 'IF', 'FOR', 'WHILE' or 'FUNC'");
            return result.failure(e);
        }

        ParserResult call() {
            ParserResult result;
            
            ParserResult left = result.register_result(atom());
            if(result.state == -1) { return result; }

            if(current_t->type == TT_LPAREN) {
                Node* call_node = new Node();
                call_node->set_start(current_t->start);
                call_node->set_type(NODE_FUNC_CALL);
                list<Node*> arguments;

                result.register_advance(advance());
                
                if(current_t->type == TT_RPAREN) {
                    result.register_advance(advance());
                } else {
                    ParserResult arg = result.register_result(expression());
                    if(result.state == -1) {
                        InvalidSyntaxError e;
                        e.init(current_t->start, current_t->end, "Expected '),' 'VAR', 'IF', 'FOR', 'WHILE', 'FUNC', int/float, identifier, '+', '-' or '(' or 'NOT'");
                        return result.failure(e);
                    }
                    arguments.push_back(arg.node);

                    while(current_t->type == TT_COMMA) {
                        result.register_advance(advance());

                        ParserResult arg_1 = result.register_result(expression());
                        if(result.state == -1) { break; }
                        arguments.push_back(arg_1.node);
                    }
                    if(result.state == -1) { return result; }

                    if(current_t->type != TT_RPAREN) {
                        InvalidSyntaxError e;
                        e.init(current_t->start, current_t->end, "Expected ',' or ')'");
                        return result.failure(e);
                    }

                    result.register_advance(advance());
                }

                call_node->set_func_call_argument_nodes_result(arguments);
                call_node->set_func_call_expression_result(left.node);
                call_node->set_end(left.node->end);
                result.set_node(call_node);

                return result.success();
            }

            Node* op = new Node();
            op->set_pos(left.node->start, left.node->end);
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
                op->set_end(right.node->end);
                op->set_to_right(right.node);
            }
            if(result.state == -1) { return result; }
            result.set_node(op);

            return result.success();
        }

        ParserResult power() {
            ParserResult result;
            
            ParserResult left = result.register_result(call());
            if(result.state == -1) { return result; }

            Node* op = new Node();
            op->set_pos(left.node->start, left.node->end);
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
                op->set_end(right.node->end);
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
                Node* n = new Node(); n->set_pos(t->start, n_0.node->end); n->set_type(NODE_UNARY); n->set_token(t); n->set_to_left(n_0.node);

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
            op->set_pos(left.node->start, left.node->end);
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
                op->set_end(right.node->end);
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
            op->set_pos(left.node->start, left.node->end);
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
                op->set_end(right.node->end);
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
            op->set_pos(left.node->start, left.node->end);
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
                op->set_end(right.node->end);
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
                Node* n = new Node(); n->set_pos(op_token->start, expr.node->end); n->set_type(NODE_UNARY); n->set_token(op_token); n->set_to_left(expr.node);

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

                Node* n = new Node(); n->set_pos(identifier->start, expr.node->end); n->set_type(NODE_ASSIGNMENT); n->set_token(identifier); n->set_to_right(expr.node);
                result.set_node(n);
                
                return result.success();
            }

            ParserResult left = result.register_result(comp_expr());
            if(result.state == -1) {
                if(result.e.extra == 1) {
                    return result;
                } else {
                    InvalidSyntaxError e;
                    e.init(current_t->start, current_t->end, "Expected 'VAR', 'IF', 'FOR', 'WHILE', 'FUNC', int/float, identifier, '+', '-' or '(' or 'NOT'");
                    return result.failure(e);
                }
            }

            Node* op = new Node();
            op->set_pos(left.node->start, left.node->end);
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
                op->set_end(right.node->end);
                op->set_to_right(right.node);
            }
            if(result.state == -1) { return result; }
            result.set_node(op);

            return result.success();
        }

        ParserResult if_expr() {
            ParserResult result;
            list<Node*> cases;

            Node* node = new Node();
            node->set_start(current_t->start);
            node->set_type(NODE_IF);

            if(current_t->matches(TT_KEYWORD, KEYWORD_IF) == false) {
                InvalidSyntaxError e;
                e.init(current_t->start, current_t->end, "Expected 'IF'");
                e.extra = 1;
                return result.failure(e);
            }

            result.register_advance(advance());

            ParserResult condition = result.register_result(expression());
            if(result.state == -1) { return result; }

            if(current_t->matches(TT_KEYWORD, KEYWORD_THEN) == false) {
                InvalidSyntaxError e;
                e.init(current_t->start, current_t->end, "Expected 'THEN'");
                e.extra = 1;
                return result.failure(e);
            }

            result.register_advance(advance());

            ParserResult expr = result.register_result(expression());
            if(result.state == -1) { return result; }

            cases.push_back(condition.node);
            cases.push_back(expr.node);

            while(current_t->matches(TT_KEYWORD, KEYWORD_ELIF)) {
                result.register_advance(advance());

                ParserResult condition_1 = result.register_result(expression());
                if(result.state == -1) { return result; }

                if(current_t->matches(TT_KEYWORD, KEYWORD_THEN) == false) {
                    InvalidSyntaxError e;
                    e.init(current_t->start, current_t->end, "Expected 'THEN'");
                    e.extra = 1;
                    return result.failure(e);
                }

                result.register_advance(advance());

                ParserResult expr_1 = result.register_result(expression());
                if(result.state == -1) { return result; }
                
                node->set_end(expr_1.node->end);
                cases.push_back(condition_1.node);
                cases.push_back(expr_1.node);
            }

            if(current_t->matches(TT_KEYWORD, KEYWORD_ELSE)) {
                result.register_advance(advance());

                ParserResult else_case = result.register_result(expression());
                if(result.state == -1) { return result; }
                node->set_end(else_case.node->end);
                node->set_else_result(else_case.node);
            }

            node->set_if_results(cases);
            result.set_node(node);

            return result.success();
        }

        ParserResult for_expr() {
            ParserResult result;
            Node* node = new Node();
            node->set_start(current_t->start);
            node->set_type(NODE_FOR);

            if(current_t->matches(TT_KEYWORD, KEYWORD_FOR) == false) {
                InvalidSyntaxError e;
                e.init(current_t->start, current_t->end, "Expected 'FOR'");
                e.extra = 1;
                return result.failure(e);
            }

            result.register_advance(advance());

            if(current_t->type != TT_IDENFIFIER) {
                InvalidSyntaxError e;
                e.init(current_t->start, current_t->end, "Expected identifier");
                e.extra = 1;
                return result.failure(e);
            }

            Token* var_name = current_t;
            result.register_advance(advance());

            if(current_t->type != TT_EQ) {
                InvalidSyntaxError e;
                e.init(current_t->start, current_t->end, "Expected '='");
                e.extra = 1;
                return result.failure(e);
            }

            result.register_advance(advance());

            ParserResult start_value = result.register_result(expression());
            if(result.state == -1) { return result; }

            if(current_t->matches(TT_KEYWORD, KEYWORD_TO) == false) {
                InvalidSyntaxError e;
                e.init(current_t->start, current_t->end, "Expected 'TO'");
                e.extra = 1;
                return result.failure(e);
            }

            result.register_advance(advance());

            ParserResult end_value = result.register_result(expression());
            if(result.state == -1) { return result; }

            if(current_t->matches(TT_KEYWORD, KEYWORD_STEP)) {
                result.register_advance(advance());

                ParserResult step_value = result.register_result(expression());
                if(result.state == -1) { return result; }
                node->set_for_step_result(step_value.node);
            }

            if(current_t->matches(TT_KEYWORD, KEYWORD_THEN) == false) {
                InvalidSyntaxError e;
                e.init(current_t->start, current_t->end, "Expected 'THEN'");
                e.extra = 1;
                return result.failure(e);
            }

            result.register_advance(advance());

            ParserResult expr = result.register_result(expression());
            if(result.state == -1) { return result; }

            node->set_end(expr.node->end);
            node->set_token(var_name);
            node->set_for_start_result(start_value.node);
            node->set_for_end_result(end_value.node);
            node->set_for_expr_result(expr.node);
            result.set_node(node);

            return result.success();
        }

        ParserResult while_expr() {
            ParserResult result;
            Node* node = new Node();
            node->set_start(current_t->start);
            node->set_type(NODE_WHILE);

            if(current_t->matches(TT_KEYWORD, KEYWORD_WHILE) == false) {
                InvalidSyntaxError e;
                e.init(current_t->start, current_t->end, "Expected 'WHILE'");
                e.extra = 1;
                return result.failure(e);
            }

            result.register_advance(advance());

            ParserResult condition = result.register_result(expression());
            if(result.state == -1) { return result; }

            if(current_t->matches(TT_KEYWORD, KEYWORD_THEN) == false) {
                InvalidSyntaxError e;
                e.init(current_t->start, current_t->end, "Expected 'THEN'");
                e.extra = 1;
                return result.failure(e);
            }

            result.register_advance(advance());

            ParserResult expr = result.register_result(expression());
            if(result.state == -1) { return result; }

            node->set_end(expr.node->end);
            node->set_while_condition_result(condition.node);
            node->set_while_expr_result(expr.node);
            result.set_node(node);

            return result.success();
        }

        ParserResult func_def() {
            ParserResult result;
            list<Token*> arguments;

            Node* node = new Node();
            node->set_start(current_t->start);
            node->set_type(NODE_FUNC_DEF);

            if(current_t->matches(TT_KEYWORD, KEYWORD_FUNC) == false) {
                InvalidSyntaxError e;
                e.init(current_t->start, current_t->end, "Expected 'FUNC'");
                e.extra = 1;
                return result.failure(e);
            }

            result.register_advance(advance());

            if(current_t->type == TT_IDENFIFIER) {
                Token* var_name = current_t;
                result.register_advance(advance());

                if(current_t->type != TT_LPAREN) {
                    InvalidSyntaxError e;
                    e.init(current_t->start, current_t->end, "Expected '('");
                    e.extra = 1;
                    return result.failure(e);
                }

                node->set_token(var_name);
            } else {
                if(current_t->type != TT_LPAREN) {
                    InvalidSyntaxError e;
                    e.init(current_t->start, current_t->end, "Expected identifier or '('");
                    e.extra = 1;
                    return result.failure(e);
                }
            }

            result.register_advance(advance());

            bool identifier_errored_out = false;
            InvalidSyntaxError id_e;
            if(current_t->type == TT_IDENFIFIER) {
                arguments.push_back(current_t);
                result.register_advance(advance());

                while(current_t->type == TT_COMMA) {
                    result.register_advance(advance());
                    
                    if(current_t->type != TT_IDENFIFIER) {
                        id_e.init(current_t->start, current_t->end, "Expected identifier");
                        id_e.extra = 1;
                        identifier_errored_out = true;
                        break;
                    }

                    arguments.push_back(current_t);
                    result.register_advance(advance());
                }

                if(identifier_errored_out == false && current_t->type != TT_RPAREN) {
                    id_e.init(current_t->start, current_t->end, "Expected ',' or ')'");
                    id_e.extra = 1;
                    identifier_errored_out = true;
                }
            } else {
                if(current_t->type != TT_RPAREN) {
                    id_e.init(current_t->start, current_t->end, "Expected identifier or ')'");
                    id_e.extra = 1;
                    identifier_errored_out = true;
                }
            }
            if(identifier_errored_out == true) {
                return result.failure(id_e);
            }

            result.register_advance(advance());

            if(current_t->type != TT_ARROW) {
                InvalidSyntaxError e;
                e.init(current_t->start, current_t->end, "Expected '->'");
                e.extra = 1;
                return result.failure(e);
            }

            result.register_advance(advance());

            ParserResult expr = result.register_result(expression());
            if(result.state == -1) { return result; }

            node->set_end(expr.node->end);
            node->set_func_def_argument_tokens_result(arguments);
            node->set_func_def_expression_result(expr.node);
            result.set_node(node);

            return result.success();
        }
};