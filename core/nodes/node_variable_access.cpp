#pragma once
#include <string>
#include "../token.cpp"
#include "node.cpp"
using namespace std;

class VariableAccessNode : public Node {
    public:
        Token name;

        void init(Token _name) {
            type = NODE_ACCESS;
            name = _name;

            start = _name.start;
            end = _name.end;
        }

        VariableAccessNode* copy() {
            VariableAccessNode* node = new VariableAccessNode();
            node->init((*name.copy()));

            node->start = start.copy();
            node->end = end.copy();
            return node;
        }

        string repr() {
            return "(" + type + "-" + name.value_string + ")";
        }
};