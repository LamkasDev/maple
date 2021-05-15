#pragma once
#include <string>
#include "../token.cpp"
#include "node.cpp"
using namespace std;

class NumberNode : public Node {
    public:
        int token_int = 0;
        float token_float = 0;

        void init(string _type) {
            type = _type;
        }

        NumberNode copy() {
            NumberNode* node = new NumberNode();
            node->init(type);
            if(type == NODE_INT) {
                node->token_int = token_int;
            } else {
                node->token_float = token_float;
            }

            node->start = start.copy();
            node->end = end.copy();
            return (*node);
        }

        string repr() {
            return type;
        }
};

class IntNode : public NumberNode {
    public:
        void init(Token _token) {
            type = NODE_INT;
            token_int = _token.value_int;

            start = _token.start;
            end = _token.end;
        }

        string repr() {
            return to_string(token_int);
        }
};

class FloatNode : public NumberNode {
    public:
        void init(Token _token) {
            type = NODE_FLOAT;
            token_float = _token.value_float;

            start = _token.start;
            end = _token.end;
        }

        string repr() {
            return to_string(token_float);
        }
};

class UnaryOperationNode : public Node {
    public:
        Token op;
        NumberNode node;

        void init(Token _op, NumberNode  _node) {
            type = NODE_UNARY;
            op = _op;
            node = _node;

            start = _op.start;
            end = _node.end;
        }

        UnaryOperationNode copy() {
            UnaryOperationNode* copy = new UnaryOperationNode();
            copy->init(op.copy(), node.copy());

            return (*copy);
        }

        string repr() {
            return "(" + op.repr() + "-" + node.repr() + ")";
        }
};