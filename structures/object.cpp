#pragma once
#include "../core/nodes/node.cpp"
using namespace std;

class Object {
    public:
        Position start;
        Position end;
        Context* context = nullptr;
        int state = 0;
        
        SymbolTable* symbol_table = nullptr;
        map<string, Function> functions;

        void set_pos(Position _start, Position _end) {
            start = _start;
            end = _end;
        }

        string repr() {
            return "OBJECT";
        }

};