#pragma once
#include "../nodes/node.cpp"
using namespace std;

class ParserResult {
    public:
        int state = 0;
        int advance_count = 0;
        int reverse_count = 0;

        string node_type = NODE_UNKNOWN;
        Node* node = nullptr;
        Error e;

        void set_node(Node* _node) {
            if(_node->left != nullptr && _node->right == nullptr && _node->type != NODE_UNARY) {
                node_type = _node->left->type;
                node = _node->left;
            } else {
                node_type = _node->type;
                node = _node;
            }
        }

        void register_advance(int _result) {
            advance_count += _result;
        }

        ParserResult register_result(ParserResult _result) {
            advance_count += _result.advance_count;
            reverse_count = _result.advance_count;
            state = _result.state;
            e = _result.e;

            return _result;
        }

        ParserResult success() {
            state = 0;
            return *this;
        }

        ParserResult failure(Error _e) {
            if(state == 0 || advance_count == 0) {
                state = -1;
                e = _e;
            }
            
            return *this;
        }
};