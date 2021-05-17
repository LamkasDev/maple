#pragma once
#include <string>
#include "../token.cpp"
#include "../position.cpp"
#include "node_value.cpp"
using namespace std;

const string NODE_UNKNOWN = "UNKNOWN";
const string NODE_INT = "INT";
const string NODE_FLOAT = "FLOAT";
const string NODE_BINARY = "BINARY";
const string NODE_UNARY = "UNARY";
const string NODE_ACCESS = "ACCESS";
const string NODE_ASSIGNMENT = "ASSIGNMENT";

class Node {
    public:
        string type = NODE_UNKNOWN;
        Position start;
        Position end;

        NodeValue* value;
        Token* token;

        Node* left;
        NodeValue* left_value;

        Node* right;
        NodeValue* right_value;

        void set_type(string _type) {
            type = _type;
        }

        void set_value(int _value) {
            value = new NodeValue();
            value->init(_value);
            type = NODE_INT;
        }

        void set_value(float _value) {
            value = new NodeValue();
            value->init(_value);
            type = NODE_FLOAT;
        }

        void set_value(NodeValue* _value) {
            value = _value;
        }

        void set_to_left(Node* _node) {
            left = _node;
        }

        void set_to_right(Node* _node) {
            right = _node;
        }

        void set_token(Token* _token) {
            token = _token;
        }

        Node* copy() {
            Node* copy = new Node();

            copy->set_type(type);
            if(value != nullptr) {
                copy->set_value(value->copy());
            }
            if(left != nullptr) {
                copy->set_to_left(left->copy());
            }
            if(right != nullptr) {
                copy->set_to_right(right->copy());
            }
            if(token != nullptr) {
                copy->set_token(token->copy());
            }

            return copy;
        }

        string repr() {
            return type;
        }
};