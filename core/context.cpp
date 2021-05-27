#pragma once
#include "position.cpp"
#include "symbols/symbol_table.cpp"
using namespace std;

class Context {
    public:
        string display_name = "";
        shared_ptr<Context> parent = nullptr;
        Position parent_entry_pos;
        shared_ptr<SymbolTable> symbol_table;

        Context(string _display_name) {
            display_name = _display_name;
        }

        void set_parent(shared_ptr<Context> _parent) {
            parent = _parent;
        }

        void set_parent_entry_pos(Position _parent_entry_pos) {
            parent_entry_pos = _parent_entry_pos;
        }

        void set_symbol_table(shared_ptr<SymbolTable> _symbol_table) {
            symbol_table = _symbol_table;
        }
};
