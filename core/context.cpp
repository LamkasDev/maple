#pragma once
#include "../structures/list.cpp"
#include "../structures/object.cpp"
#include "../structures/function.cpp"
#include "symbols/symbol_container.cpp"
#include "symbols/symbol_table.cpp"
using namespace std;

class Context {
    public:
        string display_name = "";
        shared_ptr<Context> parent = nullptr;
        shared_ptr<SymbolTable> symbol_table;
        map<string, shared_ptr<Function>> functions;

        Context(string _display_name) {
            symbol_table = make_shared<SymbolTable>();
            display_name = _display_name;
        }

        SymbolContainer get_variable(string _name) {
            return symbol_table->get(_name);
        }

        shared_ptr<Function> get_function(string _name) {
            try {
                shared_ptr<Function> value = functions.at(_name);
                return value;
            } catch(out_of_range e) {
                if(parent != nullptr) {
                    return parent->get_function(_name);
                }

                shared_ptr<Function> res_err = make_shared<Function>();
                res_err->state = -1;
                return res_err;
            }
        }

        void set_parent(shared_ptr<Context> _parent) {
            parent = _parent;
        }

        void set_symbol_table(shared_ptr<SymbolTable> _symbol_table) {
            symbol_table = _symbol_table;
        }

        void set_functions(map<string, shared_ptr<Function>> _functions) {
            functions = _functions;
        }
};
