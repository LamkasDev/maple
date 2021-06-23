#pragma once
using namespace std;

class List {
    public:
        Position start;
        Position end;
        int state = 0;
        int list_id = 0;

        void set_pos(Position _start, Position _end) {
            start = _start;
            end = _end;
        }

        void set_list_id(int _list_id) {
            list_id = _list_id;
        }

        string repr() {
            return "LIST";
        }
};
