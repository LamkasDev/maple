#pragma once
using namespace std;

class ListStore {
    public:
        vector<SymbolContainer> list_symbols;

        void add_value(SymbolContainer _value) {
            list_symbols.push_back(_value);
        }

        SymbolContainer pop_value() {
            SymbolContainer value = list_symbols.back();
            list_symbols.pop_back();
            return value;
        }

        SymbolContainer shift_value() {
            SymbolContainer value = list_symbols.front();
            list_symbols.erase(list_symbols.begin());
            return value;
        }

        void unshift_value(SymbolContainer _value) {
            list_symbols.insert(list_symbols.begin(), _value);
        }

        void reverse_values() {
            reverse(list_symbols.begin(), list_symbols.end());
        }

        SymbolContainer get_value(int index) {
            return list_symbols[index];
        }

        int length_values() {
            return list_symbols.size();
        }
};