#pragma once
using namespace std;

const string SYMBOL_UNKNOWN = "UNKNOWN";
const string SYMBOL_INT = "INT";
const string SYMBOL_FLOAT = "FLOAT";
const string SYMBOL_FUNC = "FUNC";
const string SYMBOL_STRING = "STRING";
const string SYMBOL_LIST = "LIST";

class SymbolContainer {
    public:
        int state = 0;

        string type = SYMBOL_UNKNOWN;
        int value_int = 0;
        float value_float = 0;
        string value_string = "";

        SymbolContainer() {
            
        }

        SymbolContainer(int _value) {
            type = SYMBOL_INT;
            value_int = _value;
        }

        SymbolContainer(float _value) {
            type = SYMBOL_FLOAT;
            value_float = _value;
        }

        SymbolContainer(string _value) {
            type = SYMBOL_STRING;
            value_string = _value;
        }
};
