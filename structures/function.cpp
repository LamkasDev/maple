#pragma once
#include "../core/nodes/node.cpp"
using namespace std;

class Function {
    public:
        Position start;
        Position end;
        shared_ptr<Context> context = nullptr;

        shared_ptr<Token> name = nullptr;
        vector<shared_ptr<Token>> arguments;
        Node* expression = nullptr;
        bool built_in = false;

        void set_pos(Position _start, Position _end) {
            start = _start;
            end = _end;
        }

        void set_context(shared_ptr<Context> _context) {
            context = _context;
        }

        void set_name(shared_ptr<Token> _name) {
            name = _name;
        }

        void set_arguments(vector<shared_ptr<Token>> _arguments) {
            arguments = _arguments;
        }

        void set_expression(Node* _expression) {
            expression = _expression;
        }

        string repr() {
            return name->repr() + "()";
        }
};