#pragma once
using namespace std;

class Map {
    public:
        Position start;
        Position end;
        int state = 0;
        int map_id = 0;

        void set_pos(Position _start, Position _end) {
            start = _start;
            end = _end;
        }

        void set_map_id(int _map_id) {
            map_id = _map_id;
        }

        string repr() {
            return "MAP";
        }
};
