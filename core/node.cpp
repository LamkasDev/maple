#pragma once
#include <string>
#include "token.cpp"
using namespace std;

const string NODE_UNKNOWN = "UNKNOWN";
const string NODE_INT = "INT";
const string NODE_FLOAT = "FLOAT";
const string NODE_BINARY = "BINARY";
const string NODE_UNARY = "UNARY";

class Node {
    public:
        Position start;
        Position end;
        string type = NODE_UNKNOWN;

        void init(string _type) {
            type = _type;
        }

        string repr() {
            return type;
        }
};

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

class BinaryOperationNode : public Node {
    public:
        NumberNode left;
        BinaryOperationNode* left_binary;
        UnaryOperationNode left_unary;
        string left_type = NODE_UNKNOWN;

        Token op;

        NumberNode right;
        BinaryOperationNode* right_binary;
        UnaryOperationNode right_unary;
        string right_type = NODE_UNKNOWN;

        void init(Token _op) {
            type = NODE_BINARY;
            op = _op;
        }

        void set_left(NumberNode _left) {
            left = _left;
            left_type = _left.type;
            start = _left.start.copy();
        }

        void set_left(BinaryOperationNode* _left) {
            left_binary = _left;
            left_type = (*_left).type;
            start = (*_left).start.copy();
        }

        void set_left(UnaryOperationNode _left) {
            left_unary = _left;
            left_type = _left.type;
            start = _left.start.copy();
        }

        void set_right(NumberNode _right) {
            right = _right;
            right_type = _right.type;
            end = _right.end.copy();
        }

        void set_right(BinaryOperationNode* _right) {
            right_binary = _right;
            right_type = (*_right).type;
            end = (*_right).end.copy();
        }

        void set_right(UnaryOperationNode _right) {
            right_unary = _right;
            right_type = _right.type;
            end = _right.end.copy();
        }

        BinaryOperationNode* copy() {
            BinaryOperationNode* copy = new BinaryOperationNode();
            copy->init(op.copy());
            if(left_type == NODE_BINARY) {
                copy->set_left(left_binary->copy());
            } else if(left_type == NODE_UNARY) {
                copy->set_left(left_unary.copy());
            } else {
                copy->set_left(left.copy());
            }
            if(right_type == NODE_BINARY) {
                copy->set_right(right_binary->copy());
            } else if(right_type == NODE_UNARY) {
                copy->set_right(right_unary.copy());
            } else {
                copy->set_right(right.copy());
            }

            return copy;
        }

        string repr() {
            string s = "(";
            s += (left_type == NODE_BINARY ? (*left_binary).repr() : (left_type == NODE_UNARY ? left_unary.repr() : left.repr()));
            s += "-" + op.repr() + "-";
            s += (right_type == NODE_BINARY ? (*right_binary).repr() : (right_type == NODE_UNARY ? right_unary.repr() : right.repr()));
            s += ")";

            return s.c_str();
        }
};