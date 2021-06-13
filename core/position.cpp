#pragma once
using namespace std;

class Position {
    public:
        int index = 0;
        int line = 0;
        int column = 0;

        void set_location(int _index, int _line, int _column) {
            index = _index;
            line = _line;
            column = _column;
        }

        void advance(char current_c) {
            index++;

            if(current_c == '\n') {
                line++;
                column = 0;
            } else {
                column++;
            }
        }

        void advance() {
            index++;
            column++;
        }

        Position copy() {
            Position p;
            p.set_location(index, line, column);
            return p;
        }
};