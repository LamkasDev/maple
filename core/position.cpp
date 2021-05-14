#pragma once
#include <string>
using namespace std;

class Position {
    public:
        int index = 0;
        int line = 0;
        int column = 0;
        string fileName;
        string fileContents;

        void init(int _index, int _line, int _column, string _fileName, string _fileContents) {
            index = _index;
            line = _line;
            column = _column;
            fileName = _fileName;
            fileContents = _fileContents;
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
            p.init(index, line, column, fileName, fileContents);

            return p;
        }
};