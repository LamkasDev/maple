#pragma once
#include <string>
#include "node_binary.cpp"
#include "node_variable_access.cpp"
using namespace std;

class VariableAssignmentNode : public Node {
    public:
        Token name;

        string value_type = NODE_UNKNOWN;
        NumberNode value_number;
        BinaryOperationNode* value_binary;
        UnaryOperationNode value_unary;
        VariableAccessNode value_access;

        void init(Token _name) {
            type = NODE_ASSIGNMENT;
            name = _name;

            start = _name.start;
        }

        void set_value(NumberNode _value) {
            value_number = _value;
            value_type = _value.type;
            end = _value.end;
        }

        void set_value(BinaryOperationNode* _value) {
            value_binary = _value;
            value_type = (*_value).type;
            end = (*_value).end;
        }

        void set_value(UnaryOperationNode _value) {
            value_unary = _value;
            value_type = _value.type;
            end = _value.end;
        }

        void set_value(VariableAccessNode _value) {
            value_access = _value;
            value_type = _value.type;
            end = _value.end;
        }

        string repr() {
            return "(" + type + "-" + name.value_string + "=" + value_type + ")";
        }
};