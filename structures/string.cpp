#pragma once
#include <string>
#include <vector>
#include "../core/nodes/node.cpp"
using namespace std;

class String {
    public:
        Position start;
        Position end;
        Context* context = nullptr;
        string value = "";

        void init(string _value) {
            value = _value;
        }

        void set_pos(Position _start, Position _end) {
            start = _start;
            end = _end;
        }

        void set_context(Context* _context) {
            context = _context;
        }

        string repr() {
            return value;
        }
};
