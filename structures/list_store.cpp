#pragma once
using namespace std;

class ListStore {
    public:
        vector<SymbolContainer> list_symbols;

        void add_value(SymbolContainer _value) {
            list_symbols.push_back(_value);
        }
};