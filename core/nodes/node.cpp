#pragma once
#include "node_value.cpp"
#include "../token.cpp"
using namespace std;

const string NODE_UNKNOWN = "UNKNOWN";
const string NODE_INT = "INT";
const string NODE_FLOAT = "FLOAT";
const string NODE_STRING = "STRING";

const string NODE_BINARY = "BINARY";
const string NODE_UNARY = "UNARY";
const string NODE_ACCESS = "ACCESS";
const string NODE_ASSIGNMENT = "ASSIGNMENT";

const string NODE_IF = "IF";
const string NODE_FOR = "FOR";
const string NODE_WHILE = "WHILE";
const string NODE_RETURN = "RETURN";
const string NODE_CONTINUE = "CONTINUE";
const string NODE_BREAK = "BREAK";

const string NODE_FUNC_DEF = "FUNC_DEF";
const string NODE_FUNC_CALL = "FUNC_CALL";

const string NODE_OBJECT_NEW = "OBJECT_NEW";

const string NODE_STATEMENTS = "STATEMENTS";

class Node {
    public:
        string type = NODE_UNKNOWN;
        Position start;
        Position end;

        NodeValue* value = nullptr;
        shared_ptr<Token> token = nullptr;

        Node* left = nullptr;
        NodeValue* left_value = nullptr;

        Node* right = nullptr;
        NodeValue* right_value = nullptr;

        list<Node*> if_results;
        Node* else_result = nullptr;

        Node* for_start_result = nullptr;
        Node* for_end_result = nullptr;
        Node* for_step_result = nullptr;
        Node* for_expr_result = nullptr;

        Node* while_condition_result = nullptr;
        Node* while_expr_result = nullptr;

        vector<shared_ptr<Token>> func_def_argument_tokens_result;
        Node* func_def_expression_result = nullptr;

        list<Node*> func_call_argument_nodes_result;
        Node* func_call_expression_result = nullptr;

        list<Node*> statements_nodes_result;

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

        void set_value(string _value) {
            value = new NodeValue();
            value->init(_value);
            type = NODE_STRING;
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

        void set_token(shared_ptr<Token> _token) {
            token = _token;
        }

        void set_start(Position _start) {
            start = _start;
        }

        void set_end(Position _end) {
            end = _end;
        }

        void set_pos(Position _start, Position _end) {
            start = _start;
            end = _end;
        }

        void set_if_results(list<Node*> _if_results) {
            if_results = _if_results;
        }

        void set_else_result(Node* _else_result) {
            else_result = _else_result;
        }

        void set_for_start_result(Node* _for_start_result) {
            for_start_result = _for_start_result;
        }

        void set_for_end_result(Node* _for_end_result) {
            for_end_result = _for_end_result;
        }

        void set_for_step_result(Node* _for_step_result) {
            for_step_result = _for_step_result;
        }

        void set_for_expr_result(Node* _for_expr_result) {
            for_expr_result = _for_expr_result;
        }

        void set_while_condition_result(Node* _while_condition_result) {
            while_condition_result = _while_condition_result;
        }

        void set_while_expr_result(Node* _while_expr_result) {
            while_expr_result = _while_expr_result;
        }

        void set_func_def_argument_tokens_result(vector<shared_ptr<Token>> _func_def_argument_tokens_result) {
            func_def_argument_tokens_result = _func_def_argument_tokens_result;
        }

        void set_func_def_expression_result(Node* _func_def_expression_result) {
            func_def_expression_result = _func_def_expression_result;
        }

        void set_func_call_argument_nodes_result(list<Node*> _func_call_argument_nodes_result) {
            func_call_argument_nodes_result = _func_call_argument_nodes_result;
        }

        void set_func_call_expression_result(Node* _func_call_expression_result) {
            func_call_expression_result = _func_call_expression_result;
        }

        void set_statements_nodes_result(list<Node*> _statements_nodes_result) {
            statements_nodes_result = _statements_nodes_result;
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
            if(type == NODE_INT) {
                return "(" + type + ":" + to_string(value->value_int) + ")";
            } else if(type == NODE_FLOAT) {
                return "(" + type + ":" + to_string(value->value_float) + ")";
            } else if(type == NODE_BINARY) {
                return "(" + type + ":(" + (left != nullptr ? left->repr() : "") + ":" + token->type + ":" + (right != nullptr ? right->repr() : "") + ")";
            } else if(type == NODE_UNARY) {
                return "(" + type + ":(" + token->type + ":" + (left != nullptr ? left->repr() : "") + ")";
            } else if(type == NODE_ACCESS) {
                return "(" + type + ":" + token->value_string + ")";
            } else if(type == NODE_ASSIGNMENT) {
                return "(" + type + ":(" + token->value_string + ":" + (right != nullptr ? right->repr() : "") + ")";
            } else if(type == NODE_IF) {
                return "(" + type + ")";
            } else if(type == NODE_FOR) {
                return "(" + type + ":(" + for_start_result->repr() + ":" + for_end_result->repr() + ":" + (for_step_result != nullptr ? for_step_result->repr() : "(-)") + "->" + for_expr_result->repr() + ")";
            } else if(type == NODE_WHILE) {
                return "(" + type + ":(" + while_condition_result->repr() + "->" + while_expr_result->repr() + ")";
            } else if(type == NODE_FUNC_CALL) {
                return "(" + type + ":" + func_call_expression_result->token->value_string + ")";
            } else if(type == NODE_FUNC_DEF) {
                return "(" + type + ":" + token->value_string + ")";
            } else if(type == NODE_STRING) {
                return "(" + type + ":" + value->value_string + ")";
            } else if(type == NODE_STATEMENTS) {
                return "(" + type + ":" + to_string(statements_nodes_result.size()) + ")";
            } else if(type == NODE_RETURN) {
                return "(" + type + ":" + right->repr() + ")";
            }

            return type;
        }
};