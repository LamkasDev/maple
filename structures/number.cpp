#pragma once
using namespace std;

class Number {
    public:
        Position start;
        Position end;
        shared_ptr<Context> context = nullptr;

        void set_pos(Position _start, Position _end) {
            start = _start;
            end = _end;
        }

        void set_context(shared_ptr<Context> _context) {
            context = _context;
        }
};


class FloatNumber : public Number {
    public:
        float value = 0;

        void init(float _value) {
            value = _value;
        }

        string repr() {
            return to_string(value);
        }
};

class IntNumber : public Number {
    public:
        int value = 0;

        void init(int _value) {
            value = _value;
        }

        string repr() {
            return to_string(value);
        }
};