#pragma once
#include <string>
#include "../nodes/node_variable.cpp"
using namespace std;

class ParserResult {
    public:
        int state = 0;
        string node_type = NODE_UNKNOWN;
        NumberNode node_number;
        BinaryOperationNode* node_binary;
        UnaryOperationNode node_unary;
        VariableAssignmentNode node_assignment;
        VariableAccessNode node_access;
        Error e;

        void init() {

        }

        ParserResult registerResult(ParserResult _result) {
            state = _result.state;
            e = _result.e;

            return _result;
        }

        ParserResult success() {
            state = 0;
            return *this;
        }

        ParserResult failure(Error _e) {
            state = -1;
            e = _e;
            return *this;
        }

        void set_to_left(BinaryOperationNode* _node) {
            if(node_type == NODE_BINARY) {
                _node->set_left(node_binary);
            } else if(node_type == NODE_UNARY) {
                _node->set_left(node_unary);
            } else if(node_type == NODE_ACCESS) {
                _node->set_left(node_access);
            } else {
                _node->set_left(node_number);
            }
        }

        void set_to_right(BinaryOperationNode* _node) {
            if(node_type == NODE_BINARY) {
                _node->set_right(node_binary);
            } else if(node_type == NODE_UNARY) {
                _node->set_right(node_unary);
            } else if(node_type == NODE_ACCESS) {
                _node->set_right(node_access);
            } else {
                _node->set_right(node_number);
            }
        }

        void set_from(BinaryOperationNode* _node) {
            if(((*_node).left_type == NODE_INT || (*_node).left_type == NODE_FLOAT) && (*_node).right_type == NODE_UNKNOWN) {
                node_type = (*_node).left.type;
                node_number = (*_node).left;
            } else if((*_node).left_type == NODE_BINARY && (*_node).right_type == NODE_UNKNOWN) {
                node_type = (*(*_node).left_binary).type;
                node_binary = (*_node).left_binary;
            } else if((*_node).left_type == NODE_UNARY && (*_node).right_type == NODE_UNKNOWN) {
                node_type = (*_node).left_unary.type;
                node_unary = (*_node).left_unary;
            } else if((*_node).left_type == NODE_ACCESS && (*_node).right_type == NODE_UNKNOWN) {
                node_type = (*_node).left_access.type;
                node_access = (*_node).left_access;
            } else {
                node_type = (*_node).type;
                node_binary = _node;
            }
        }

        VariableAssignmentNode set_to_assignment(VariableAssignmentNode _node) {
            if(node_type == NODE_BINARY) {
                _node.set_value(node_binary);
            } else if(node_type == NODE_UNARY) {
                _node.set_value(node_unary);
            } else if(node_type == NODE_ACCESS) {
                _node.set_value(node_access);
            } else {
                _node.set_value(node_number);
            }

            return _node;
        }
};