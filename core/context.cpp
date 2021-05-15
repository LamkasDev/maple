#pragma once
#include <string>
#include "position.cpp"
using namespace std;

class Context {
    public:
        string display_name;
        Context* parent;
        Position parent_entry_pos;

        void init(string _display_name) {
            display_name = _display_name;
        }

        void set_parent(Context* _parent) {
            parent = _parent;
        }

        void set_parent_entry_pos(Position _parent_entry_pos) {
            parent_entry_pos = _parent_entry_pos;
        }
};
