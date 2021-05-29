#pragma once
using namespace std;

class Number {
    public:
        Position start;
        Position end;

        void set_pos(Position _start, Position _end) {
            start = _start;
            end = _end;
        }
};


class FloatNumber : public Number {
    public:
        float value = 0;

        FloatNumber() {

        }

        FloatNumber(float _value) {
            value = _value;
        }

        string repr() {
            return to_string(value);
        }
};

class IntNumber : public Number {
    public:
        int value = 0;

        IntNumber() {
            
        }

        IntNumber(int _value) {
            value = _value;
        }

        string repr() {
            return to_string(value);
        }
};