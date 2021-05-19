#pragma once
#include <string>
using namespace std;

const string SYMBOL_UNKNOWN = "UNKNOWN";
const string SYMBOL_INT = "INT";
const string SYMBOL_FLOAT = "FLOAT";
const string SYMBOL_FUNC = "FUNC";

class SymbolContainer {
    public:
        int state = 0;

        string type = SYMBOL_UNKNOWN;
        int value_int = 0;
        float value_float = 0;

        void init(int _value) {
            type = SYMBOL_INT;
            value_int = _value;
        }

        void init(float _value) {
            type = SYMBOL_FLOAT;
            value_float = _value;
        }
};
