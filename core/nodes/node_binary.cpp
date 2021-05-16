#pragma once
#include <string>
#include "node_number.cpp"
#include "node_variable_access.cpp"
using namespace std;

class BinaryOperationNode : public Node {
    public:
        NumberNode left;
        BinaryOperationNode* left_binary;
        UnaryOperationNode left_unary;
        VariableAccessNode left_access;
        string left_type = NODE_UNKNOWN;

        Token op;

        NumberNode right;
        BinaryOperationNode* right_binary;
        UnaryOperationNode right_unary;
        VariableAccessNode right_access;
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
            left_type = _left->type;
            start = _left->start.copy();
        }

        void set_left(UnaryOperationNode _left) {
            left_unary = _left;
            left_type = _left.type;
            start = _left.start.copy();
        }

        void set_left(VariableAccessNode _left) {
            left_access = _left;
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
            right_type = _right->type;
            end = _right->end.copy();
        }

        void set_right(UnaryOperationNode _right) {
            right_unary = _right;
            right_type = _right.type;
            end = _right.end.copy();
        }

        void set_right(VariableAccessNode _right) {
            right_access = _right;
            right_type = _right.type;
            start = _right.start.copy();
        }

        BinaryOperationNode* copy() {
            BinaryOperationNode* copy = new BinaryOperationNode();
            copy->init((*op.copy()));
            if(left_type == NODE_BINARY) {
                copy->set_left(left_binary->copy());
            } else if(left_type == NODE_UNARY) {
                copy->set_left((*left_unary.copy()));
            } else if(left_type == NODE_ACCESS) {
                copy->set_left((*left_access.copy()));
            } else {
                copy->set_left((*left.copy()));
            }
            if(right_type == NODE_BINARY) {
                copy->set_right(right_binary->copy());
            } else if(right_type == NODE_UNARY) {
                copy->set_right((*right_unary.copy()));
            } else if(right_type == NODE_ACCESS) {
                copy->set_right((*right_access.copy()));
            } else {
                copy->set_right((*right.copy()));
            }

            return copy;
        }

        string repr() {
            string s = "(";
            if(left_type == NODE_BINARY) {
                s += left_binary->repr();
            } else if(left_type == NODE_UNARY) {
                s += left_unary.repr();
            } else if(left_type == NODE_ACCESS) {
                s += left_access.repr();
            } else {
                s += left.repr();
            }
            s += "-" + op.repr() + "-";
            if(right_type == NODE_BINARY) {
                s += right_binary->repr();
            } else if(right_type == NODE_UNARY) {
                s += right_unary.repr();
            } else if(right_type == NODE_ACCESS) {
                s += right_access.repr();
            } else {
                s += right.repr();
            }
            s += ")";

            return s.c_str();
        }
};