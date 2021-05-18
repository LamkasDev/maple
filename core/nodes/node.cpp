#pragma once
#include <string>
#include "../token.cpp"
#include "../position.cpp"
#include "../parser/parser_result.cpp"
#include "node_value.cpp"
using namespace std;

const string NODE_UNKNOWN = "UNKNOWN";
const string NODE_INT = "INT";
const string NODE_FLOAT = "FLOAT";
const string NODE_BINARY = "BINARY";
const string NODE_UNARY = "UNARY";
const string NODE_ACCESS = "ACCESS";
const string NODE_ASSIGNMENT = "ASSIGNMENT";
const string NODE_IF = "IF";

class Node {
    public:
        string type = NODE_UNKNOWN;
        Position start;
        Position end;

        NodeValue* value = nullptr;
        Token* token = nullptr;

        Node* left = nullptr;
        NodeValue* left_value = nullptr;

        Node* right = nullptr;
        NodeValue* right_value = nullptr;

        list<Node*> if_results;
        Node* else_result = nullptr;

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
            start = _node->start;
            end = _node->end;
        }

        void set_to_right(Node* _node) {
            right = _node;
            end = _node->end;
        }

        void set_token(Token* _token) {
            token = _token;
        }

        void set_if_results(list<Node*> _if_results) {
            if_results = _if_results;
            start = _if_results.front()->start;
            end = _if_results.back()->end;
        }

        void set_else_result(Node* _else_result) {
            else_result = _else_result;
            end = _else_result->end;
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