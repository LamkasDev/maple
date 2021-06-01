#pragma once
using namespace std;

class Function {
    public:
        Position start;
        Position end;
        int state = 0;

        shared_ptr<Token> name = nullptr;
        vector<shared_ptr<Token>> arguments;
        shared_ptr<Node> expression = nullptr;
        bool built_in = false;

        void set_pos(Position _start, Position _end) {
            start = _start;
            end = _end;
        }

        void set_name(shared_ptr<Token> _name) {
            name = _name;
        }

        void set_arguments(vector<shared_ptr<Token>> _arguments) {
            arguments = _arguments;
        }

        void set_expression(shared_ptr<Node> _expression) {
            expression = _expression;
        }

        string repr() {
            return name->repr() + "()";
        }
};