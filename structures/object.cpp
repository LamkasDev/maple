#pragma once
#include "object_prototype.cpp"
#include "../core/context.cpp"
using namespace std;

class Object {
    public:
        Position start;
        Position end;
        int state = 0;

        shared_ptr<Context> context = nullptr;
        map<string, shared_ptr<Object>> objects;
        shared_ptr<ObjectPrototype> prototype = nullptr;

        Object(shared_ptr<Context> _context) {
            context = _context;
        }

        void set_prototype(shared_ptr<ObjectPrototype> _prototype) {
            prototype = _prototype;
        }

        void set_pos(Position _start, Position _end) {
            start = _start;
            end = _end;
        }

        string repr() {
            return "OBJECT";
        }

};