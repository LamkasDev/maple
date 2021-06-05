
#pragma once
using namespace std;

class ListStore {
    public:
        vector<SymbolContainer> list_ints;
        vector<SymbolContainer> list_floats;
        vector<SymbolContainer> list_strings;
        vector<shared_ptr<List>> list_lists;
        vector<shared_ptr<Object>> list_objects;
        string type = SYMBOL_LIST_UNKNOWN;

        void add_value(SymbolContainer _value) {
            if(_value.type == SYMBOL_INT) {
                list_ints.push_back(_value);
                type = SYMBOL_LIST_INT;
            } else if(_value.type == SYMBOL_FLOAT) {
                list_floats.push_back(_value);
                type = SYMBOL_LIST_FLOAT;
            } else if(_value.type == SYMBOL_STRING) {
                list_strings.push_back(_value);
                type = SYMBOL_LIST_STRING;
            }
        }

        void add_value(shared_ptr<List> _value) {
            list_lists.push_back(_value);
            type = SYMBOL_LIST_LIST;
        }

        void add_value(shared_ptr<Object> _value) {
            list_objects.push_back(_value);
            type = SYMBOL_LIST_OBJECT;
        }

        vector<SymbolContainer> get_iterable_containers() {
            if(type == SYMBOL_LIST_INT) {
                return list_ints;
            } else if(type == SYMBOL_LIST_FLOAT) {
                return list_floats;
            } else if(type == SYMBOL_LIST_STRING) {
                return list_strings;
            } else {
                vector<SymbolContainer> empty;
                return empty;
            }
        }
};