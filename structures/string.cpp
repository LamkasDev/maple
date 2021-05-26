#pragma once
#include "../core/nodes/node.cpp"
using namespace std;

class String {
    public:
        Position start;
        Position end;
        shared_ptr<Context> context = nullptr;
        string value = "";

        void init(string _value) {
            value = _value;
        }

        void set_pos(Position _start, Position _end) {
            start = _start;
            end = _end;
        }

        void set_context(shared_ptr<Context> _context) {
            context = _context;
        }

        string repr() {
            return value;
        }
};
