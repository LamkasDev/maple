#pragma once
#include <string>
#include <vector>
#include "token.cpp"
#include "node.cpp"
#include "parser_result.cpp"
using namespace std;

class Parser {
    public:
        list<Token> tokens;
        int index;
        Token current_t;

        void init(list<Token> _tokens) {
            tokens = _tokens;
            index = 1;
            advance();
        }

        void advance() {
            index++;
            if(tokens.size() > 0) {
                current_t = tokens.front();
                tokens.pop_front();
            }
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

        ParserResult factor() {
            ParserResult result;
            result.init();

            Token t = current_t;
            if(t.type == TT_PLUS || t.type == TT_MINUS) {
                advance();
                ParserResult n_0 = factor();
                if(n_0.state == -1) { return result.failure(n_0.e); }
                UnaryOperationNode n; n.init(t, n_0.node_number);

                result.node_type = n.type;
                result.node_unary = n;
                return result.success();
            } else if(t.type == TT_INT) {
                advance();
                IntNode n; n.init(t);

                result.node_type = n.type;
                result.node_number = n;
                return result.success();
            } else if(t.type == TT_FLOAT) {
                advance();
                FloatNode n; n.init(t);

                result.node_type = n.type;
                result.node_number = n;
                return result.success();
            } else if(t.type == TT_LPAREN) {
                advance();
                ParserResult n_0 = expression();
                if(n_0.state == -1) { return result.failure(n_0.e); }
                if(current_t.type == TT_RPAREN) {
                    advance();
                    
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
            e.init(t.start, t.end, "Expected int or float");
            return result.failure(e);
        }

        ParserResult term() {
            ParserResult result;
            result.init();

            ParserResult left = result.registerResult(factor());
            if(result.state == -1) { return result; }
            string node_type = left.node_type;

            BinaryOperationNode* op = new BinaryOperationNode();
            if(left.node_type == NODE_BINARY) {
                (*op).set_left(&left.node_binary);
            } else if(left.node_type == NODE_UNARY) {
                (*op).set_left(left.node_unary);
            } else {
                (*op).set_left(left.node_number);
            }

            while(current_t.type == TT_MUL || current_t.type == TT_DIV) {
                Token op_token = current_t;
                (*op).init(op_token);
                advance();

                ParserResult right = result.registerResult(factor());
                if(result.state == -1) {
                    break;
                }

                if((*op).right_type != NODE_UNKNOWN) {
                    /*BinaryOperationNode* op_1 = new BinaryOperationNode();
                    (*op_1).set_left(op);
                    *op = (*op_1);*/
                }
                if(right.node_type == NODE_BINARY) {
                    (*op).set_right(&right.node_binary);
                } else if(right.node_type == NODE_UNARY) {
                    (*op).set_right(right.node_unary);
                } else {
                    (*op).set_right(right.node_number);
                }
                node_type = (*op).type;
            }
            if(result.state == -1) { return result; }

            if(node_type == NODE_BINARY) {
                if((*op).right_type == NODE_UNKNOWN) {
                    result.node_type = (*op).left_type;
                    result.node_binary = *((*op).left_binary);
                } else {
                    result.node_type = (*op).type;
                    result.node_binary = (*op);
                }
            } else if(node_type == NODE_UNARY) {
                result.node_type = left.node_unary.type;
                result.node_unary = left.node_unary;
            } else {
                result.node_type = left.node_number.type;
                result.node_number = left.node_number;
            }

            //printf("term: %s\n", result.node_type.c_str());
            return result.success();
        }

        ParserResult expression() {
            ParserResult result;
            result.init();

            ParserResult left = result.registerResult(term());
            if(result.state == -1) { return result; }
            string node_type = left.node_type;

            BinaryOperationNode* op = new BinaryOperationNode();
            if(left.node_type == NODE_BINARY) {
                (*op).set_left(&left.node_binary);
            } else if(left.node_type == NODE_UNARY) {
                (*op).set_left(left.node_unary);
            } else {
                (*op).set_left(left.node_number);
            }
            
            while(current_t.type == TT_PLUS || current_t.type == TT_MINUS) {
                Token op_token = current_t;
                (*op).init(op_token);
                advance();

                ParserResult right = result.registerResult(term());
                if(result.state == -1) {
                    break;
                }

                if((*op).right_type != NODE_UNKNOWN) {
                    /*BinaryOperationNode* op_1 = new BinaryOperationNode();
                    (*op_1).set_left(op);
                    *op = (*op_1);*/
                }
                if(right.node_type == NODE_BINARY) {
                    (*op).set_right(&right.node_binary);
                } else if(left.node_type == NODE_UNARY) {
                    (*op).set_right(right.node_unary);
                } else {
                    (*op).set_right(right.node_number);
                }
                node_type = (*op).type;
            }
            if(result.state == -1) { return result; }

            if(((*op).left_type == NODE_INT || (*op).left_type == NODE_FLOAT) && (*op).right_type == NODE_UNKNOWN) {
                result.node_type = left.node_number.type;
                result.node_number = left.node_number;
            } else if((*op).left_type == NODE_BINARY && (*op).right_type == NODE_UNKNOWN) {
                result.node_type = left.node_binary.type;
                result.node_binary = left.node_binary;
            } else if((*op).left_type == NODE_UNARY && (*op).right_type == NODE_UNKNOWN) {
                result.node_type = left.node_unary.type;
                result.node_unary = left.node_unary;
            } else {
                result.node_type = (*op).type;
                result.node_binary = (*op);
            }

            //printf("expr: %s\n", result.node_type.c_str());
            return result.success();
        }
};