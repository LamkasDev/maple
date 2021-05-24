#pragma once
#include "../core/nodes/node.cpp"
using namespace std;

class Function {
    public:
        Position start;
        Position end;
        Context* context = nullptr;

        Token* name = nullptr;
        vector<Token*> arguments;
        Node* expression = nullptr;
        bool built_in = false;

        void set_pos(Position _start, Position _end) {
            start = _start;
            end = _end;
        }

        void set_context(Context* _context) {
            context = _context;
        }

        void set_name(Token* _name) {
            name = _name;
        }

        void set_arguments(vector<Token*> _arguments) {
            arguments = _arguments;
        }

        void set_expression(Node* _expression) {
            expression = _expression;
        }
};