#pragma once
#include <string>
#include <map>
#include <stdexcept>
#include "symbol_container.cpp"
using namespace std;

class SymbolTable {
    public:
        SymbolTable* parent = nullptr;
        map<string, SymbolContainer*> symbols;

        void init() {

        }

        SymbolContainer* get(string _name) {
            try {
                SymbolContainer* value = symbols.at(_name);
                return value;
            } catch(out_of_range e) {
                if(parent != nullptr) {
                    return parent->get(_name);
                }

                SymbolContainer* res_err = new SymbolContainer();
                res_err->state = -1;
                return res_err;
            }
        }

        void set(string _name, SymbolContainer* _value) {
            symbols[_name] = _value;
        } 

        void remove(string _name) {
            symbols.erase(_name);
        }
};
