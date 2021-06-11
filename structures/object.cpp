#pragma once
using namespace std;

class Object {
    public:
        Position start;
        Position end;
        int state = 0;
        int object_id = 0;

        void set_pos(Position _start, Position _end) {
            start = _start;
            end = _end;
        }

        void set_object_id(int _object_id) {
            object_id = _object_id;
        }

        string repr() {
            return "OBJECT";
        }

};