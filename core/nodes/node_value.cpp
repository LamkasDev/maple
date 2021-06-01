#pragma once
using namespace std;

const string NODE_VALUE_UNKNOWN = "UNKNOWN";
const string NODE_VALUE_INT = "INT";
const string NODE_VALUE_FLOAT = "FLOAT";
const string NODE_VALUE_STRING = "STRING";

class NodeValue {
    public:
        int value_int = 0;
        float value_float = 0;
        string value_string = "";
        string type = NODE_VALUE_UNKNOWN;

        void set_value(int _value) {
            value_int = _value;
            type = NODE_VALUE_INT;
        }

        void set_value(float _value) {
            value_float = _value;
            type = NODE_VALUE_FLOAT;
        }

        void set_value(string _value) {
            value_string = _value;
            type = NODE_VALUE_STRING;
        }

        NodeValue copy() {
            NodeValue copy;
            if(type == NODE_VALUE_INT) {
                copy.set_value(value_int);
            } else if(type == NODE_VALUE_FLOAT) {
                copy.set_value(value_float);
            } else if(type == NODE_VALUE_STRING) {
                copy.set_value(value_string);
            }

            return copy;
        }
};