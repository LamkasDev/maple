#pragma once
#include <string>
using namespace std;

class Parser {
    public:
        list<Token> tokens;
        int index = 0;
        Token current_t;

        void init(list<Token> _tokens) {
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
            if(res.state != -1 && current_t.type != TT_EOF) {
                InvalidSyntaxError e;
                e.init(current_t.start, current_t.end, "Expected '+', '-', '*' or '/'");
                return res.failure(e);
            }

            return res;
        }

        ParserResult atom() {
            ParserResult result;
            result.init();

            Token t = current_t;
            if(t.type == TT_INT) {
                result.register_advance(advance());
                IntNode n; n.init(t);

                result.node_type = n.type;
                result.node_number = n;
                return result.success();
            } else if(t.type == TT_FLOAT) {
                result.register_advance(advance());
                FloatNode n; n.init(t);

                result.node_type = n.type;
                result.node_number = n;
                return result.success();
            } else if(t.type == TT_IDENFIFIER) {
                result.register_advance(advance());
                VariableAccessNode n; n.init(t);

                result.node_type = n.type;
                result.node_access = n;
                return result.success();
            } else if(t.type == TT_LPAREN) {
                result.register_advance(advance());
                ParserResult n_0 = expression();
                if(n_0.state == -1) { return result.failure(n_0.e); }
                if(current_t.type == TT_RPAREN) {
                    result.register_advance(advance());
                    
                    result.node_type = n_0.node_type;
                    result.node_binary = n_0.node_binary;
                    return result.success();
                } else {
                    InvalidSyntaxError e;
                    e.init(current_t.start, current_t.end, "Expected ')'");
                    return result.failure(e);
                }
            }

            InvalidSyntaxError e;
            e.init(t.start, t.end, "Expected int or float, '+', '-' or '('");
            return result.failure(e);
        }

        ParserResult power() {
            ParserResult result;
            result.init();

            ParserResult left = result.register_result(atom());
            if(result.state == -1) { return result; }

            BinaryOperationNode* op = new BinaryOperationNode();
            left.set_to_left(op);

            while(current_t.type == TT_POW) {
                Token op_token = current_t;
                op->init(op_token);
                result.register_advance(advance());

                ParserResult right = result.register_result(factor());
                if(result.state == -1) {
                    break;
                }

                if(op->right_type != NODE_UNKNOWN) {
                    BinaryOperationNode* copy = op->copy();
                    op->set_left(copy);
                }
                right.set_to_right(op);
            }
            if(result.state == -1) { return result; }
            result.set_from(op);

            return result.success();
        }

        ParserResult factor() {
            ParserResult result;
            result.init();

            Token t = current_t;
            if(t.type == TT_PLUS || t.type == TT_MINUS) {
                result.register_advance(advance());
                ParserResult n_0 = factor();
                if(n_0.state == -1) { return result.failure(n_0.e); }
                UnaryOperationNode n; n.init(t, n_0.node_number);

                result.node_type = n.type;
                result.node_unary = n;
                return result.success();
            }

            return power();
        }

        ParserResult term() {
            ParserResult result;
            result.init();

            ParserResult left = result.register_result(factor());
            if(result.state == -1) { return result; }

            BinaryOperationNode* op = new BinaryOperationNode();
            left.set_to_left(op);

            while(current_t.type == TT_MUL || current_t.type == TT_DIV || current_t.type == TT_MOD) {
                Token op_token = current_t;
                op->init(op_token);
                result.register_advance(advance());

                ParserResult right = result.register_result(factor());
                if(result.state == -1) {
                    break;
                }

                if(op->right_type != NODE_UNKNOWN) {
                    BinaryOperationNode* copy = op->copy();
                    op->set_left(copy);
                }
                right.set_to_right(op);
            }
            if(result.state == -1) { return result; }
            result.set_from(op);

            return result.success();
        }

        ParserResult expression() {
            ParserResult result;
            result.init();

            if(current_t.matches(TT_KEYWORD, KEYWORD_VAR)) {
                result.register_advance(advance());
                if(current_t.type != TT_IDENFIFIER) {
                    InvalidSyntaxError e;
                    e.init(current_t.start, current_t.end, "Expected identifier");
                    return result.failure(e);
                }

                Token identifier = current_t;
                result.register_advance(advance());

                if(current_t.type != TT_EQ) {
                    InvalidSyntaxError e;
                    e.init(current_t.start, current_t.end, "Expected '='");
                    return result.failure(e);
                }

                result.register_advance(advance());
                ParserResult expr = result.register_result(expression());
                if(result.state == -1) { return result; }

                VariableAssignmentNode n; n.init(identifier); n = expr.set_to_assignment(n);
                result.node_type = n.type;
                result.node_assignment = n;
                
                return result.success();
            }

            ParserResult left = result.register_result(term());
            if(result.state == -1) { return result; }

            BinaryOperationNode* op = new BinaryOperationNode();
            left.set_to_left(op);
            
            while(current_t.type == TT_PLUS || current_t.type == TT_MINUS) {
                Token op_token = current_t;
                op->init(op_token);
                result.register_advance(advance());

                ParserResult right = result.register_result(term());
                if(result.state == -1) {
                    break;
                }

                if(op->right_type != NODE_UNKNOWN) {
                    BinaryOperationNode* copy = op->copy();
                    op->set_left(copy);
                }
                right.set_to_right(op);
            }
            if(result.state == -1) { return result; }
            result.set_from(op);

            return result.success();
        }
};