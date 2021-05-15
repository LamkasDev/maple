#pragma once
#include <string>
#include <vector>
#include "../core/position.cpp"
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

        void init(float _value) {
            value = _value;
        }

        FloatNumber added_to(FloatNumber _number) {
            FloatNumber n; n.init(value + _number.value); return n;
        }

        FloatNumber substracted_by(FloatNumber _number) {
            FloatNumber n; n.init(value - _number.value); return n;
        }

        FloatNumber multiplied_by(FloatNumber _number) {
            FloatNumber n; n.init(value * _number.value); return n;
        }

        FloatNumber divided_by(FloatNumber _number) {
            FloatNumber n; n.init(value / _number.value); return n;
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

        IntNumber added_to(IntNumber _number) {
            IntNumber n; n.init(value + _number.value); return n;
        }

        FloatNumber added_to(FloatNumber _number) {
            FloatNumber n; n.init(value + _number.value); return n;
        }

        IntNumber substracted_by(IntNumber _number) {
            IntNumber n; n.init(value - _number.value); return n;
        }

        FloatNumber substracted_by(FloatNumber _number) {
            FloatNumber n; n.init(value - _number.value); return n;
        }

        IntNumber multiplied_by(IntNumber _number) {
            IntNumber n; n.init(value * _number.value); return n;
        }

        FloatNumber multiplied_by(FloatNumber _number) {
            FloatNumber n; n.init(value * _number.value); return n;
        }

        IntNumber divided_by(IntNumber _number) {
            IntNumber n; n.init(value / _number.value); return n;
        }

        FloatNumber divided_by(FloatNumber _number) {
            FloatNumber n; n.init(value / _number.value); return n;
        }

        string repr() {
            return to_string(value);
        }
};