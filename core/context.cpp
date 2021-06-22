#pragma once
#include "../structures/list.cpp"
#include "../structures/map.cpp"
#include "../structures/object.cpp"
#include "../structures/function.cpp"
#include "symbols/symbol_container.cpp"
#include "symbols/symbol_table.cpp"
using namespace std;

class Context {
    public:
        string display_name = "";
        string file_name = "";
        Position entry_pos;

        shared_ptr<Context> parent = nullptr;
        shared_ptr<SymbolTable> symbol_table;

        Context(string _file_name, string _display_name) {
            symbol_table = make_shared<SymbolTable>();
            file_name = _file_name;
            display_name = _display_name;
        }

        SymbolContainer get_variable(string _name) {
            return symbol_table->get(_name);
        }

        void set_parent(shared_ptr<Context> _parent) {
            parent = _parent;
        }

        void set_symbol_table(shared_ptr<SymbolTable> _symbol_table) {
            symbol_table = _symbol_table;
        }
        
        void set_entry_pos(Position _entry_pos) {
            entry_pos = _entry_pos;
        }
};
