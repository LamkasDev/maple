#pragma once
#include "../core/context.cpp"
using namespace std;

class Object {
    public:
        Position start;
        Position end;
        int state = 0;

        shared_ptr<Context> context = nullptr;
        map<string, shared_ptr<Object>> objects;
        vector<shared_ptr<Token>> arguments;

        Object(shared_ptr<Context> _context) {
            context = _context;
        }

        void set_pos(Position _start, Position _end) {
            start = _start;
            end = _end;
        }

        void set_arguments(vector<shared_ptr<Token>> _arguments) {
            arguments = _arguments;
        }

        string repr() {
            return "OBJECT";
        }

};