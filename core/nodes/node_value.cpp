#pragma once
#include <string>
using namespace std;

const string NODE_VALUE_UNKNOWN = "UNKNOWN";
const string NODE_VALUE_INT = "INT";
const string NODE_VALUE_FLOAT = "FLOAT";

class NodeValue {
    public:
        int value_int = 0;
        float value_float = 0;
        string type = NODE_VALUE_UNKNOWN;

        void init(int _value) {
            value_int = _value;
            type = NODE_VALUE_INT;
        }

        void init(float _value) {
            value_float = _value;
            type = NODE_VALUE_FLOAT;
        }

        NodeValue* copy() {
            NodeValue* copy = new NodeValue();
            if(type == NODE_VALUE_INT) {
                copy->init(value_int);
            } else if(type == NODE_VALUE_FLOAT) {
                copy->init(value_float);
            }

            return copy;
        }
};