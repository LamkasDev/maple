#pragma once
using namespace std;

class String {
    public:
        Position start;
        Position end;
        string value = "";

        String() {
            
        }

        String(string _value) {
            value = _value;
        }

        void set_pos(Position _start, Position _end) {
            start = _start;
            end = _end;
        }

        string repr() {
            return value;
        }
};
