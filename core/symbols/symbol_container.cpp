#pragma once
using namespace std;

const string SYMBOL_UNKNOWN = "UNKNOWN";
const string SYMBOL_INT = "INT";
const string SYMBOL_FLOAT = "FLOAT";
const string SYMBOL_FUNC = "FUNC";
const string SYMBOL_STRING = "STRING";
const string SYMBOL_LIST = "LIST";
const string SYMBOL_MAP = "MAP";
const string SYMBOL_OBJECT = "OBJECT";
const string SYMBOL_FUNCTION = "FUNCTION";

class SymbolContainer {
    public:
        int state = 0;

        string type = SYMBOL_UNKNOWN;
        int value_int = 0;
        float value_float = 0;
        string value_string = "";
        shared_ptr<List> value_list = nullptr;
        shared_ptr<Map> value_map = nullptr;
        shared_ptr<Object> value_object = nullptr;
        shared_ptr<Function> value_function = nullptr;

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

        SymbolContainer(shared_ptr<List> _value) {
            type = SYMBOL_LIST;
            value_list = _value;
        }

        SymbolContainer(shared_ptr<Map> _value) {
            type = SYMBOL_MAP;
            value_map = _value;
        }

        SymbolContainer(shared_ptr<Object> _value) {
            type = SYMBOL_OBJECT;
            value_object = _value;
        }

        SymbolContainer(shared_ptr<Function> _value) {
            type = SYMBOL_FUNCTION;
            value_function = _value;
        }
};
