#pragma once
#include <string>
using namespace std;

class ParserResult {
    public:
        int state = 0;
        string node_type = NODE_UNKNOWN;
        NumberNode node_number;
        BinaryOperationNode node_binary;
        UnaryOperationNode node_unary;
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

        void set_to_left(BinaryOperationNode _node) {
            if(node_type == NODE_BINARY) {
                _node.set_left(&node_binary);
            } else if(node_type == NODE_UNARY) {
                _node.set_left(node_unary);
            } else {
                _node.set_left(node_number);
            }
        }

        void set_to_right(BinaryOperationNode _node) {
            if(node_type == NODE_BINARY) {
                _node.set_right(&node_binary);
            } else if(node_type == NODE_UNARY) {
                _node.set_right(node_unary);
            } else {
                _node.set_right(node_number);
            }
        }
};