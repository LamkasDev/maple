#pragma once
using namespace std;

class ListStore {
    public:
        vector<SymbolContainer> list_symbols;
        vector<shared_ptr<List>> list_lists;
        vector<shared_ptr<Object>> list_objects;
        string type = SYMBOL_LIST_UNKNOWN;

        void add_value(SymbolContainer _value) {
            list_symbols.push_back(_value);
            type = SYMBOL_LIST_SYMBOLS;
        }

        void add_value(shared_ptr<List> _value) {
            list_lists.push_back(_value);
            type = SYMBOL_LIST_LIST;
        }

        void add_value(shared_ptr<Object> _value) {
            list_objects.push_back(_value);
            type = SYMBOL_LIST_OBJECT;
        }
};