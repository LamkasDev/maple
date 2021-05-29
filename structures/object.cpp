#pragma once
#include "../core/nodes/node.cpp"
using namespace std;

class Object {
    public:
        Position start;
        Position end;
        int state = 0;

        vector<shared_ptr<Token>> arguments;

        shared_ptr<Context> context = nullptr;
        map<string, Function> functions;

        Object() {
            context = make_shared<Context>("obj_context");
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