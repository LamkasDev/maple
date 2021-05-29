#pragma once
#include "position.cpp"
#include "symbols/symbol_table.cpp"
#include "../structures/list.cpp"
#include "../structures/function.cpp"
using namespace std;

class Context {
    public:
        string display_name = "";
        shared_ptr<Context> parent = nullptr;
        Position parent_entry_pos;

        shared_ptr<SymbolTable> symbol_table;
        map<string, shared_ptr<List>> lists;
        map<string, shared_ptr<Function>> functions;

        Context(string _display_name) {
            symbol_table = make_shared<SymbolTable>();
            display_name = _display_name;
        }

        SymbolContainer get_variable(string _name) {
            return symbol_table->get(_name);
        }

        shared_ptr<List> get_list(string _name) {
            try {
                shared_ptr<List> value = lists.at(_name);
                return value;
            } catch(out_of_range e) {
                if(parent != nullptr) {
                    return parent->get_list(_name);
                }

                shared_ptr<List> res_err;
                res_err->state = -1;
                return res_err;
            }
        }

        shared_ptr<Function> get_function(string _name) {
            try {
                shared_ptr<Function> value = functions.at(_name);
                return value;
            } catch(out_of_range e) {
                if(parent != nullptr) {
                    return parent->get_function(_name);
                }

                shared_ptr<Function> res_err;
                res_err->state = -1;
                return res_err;
            }
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
