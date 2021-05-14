#pragma once
#include <string>
#include "position.cpp"
using namespace std;

class Error {
    public:
        Position start;
        Position end;
        string name;
        string details;

        void init(Position _start, Position _end, string _name, string _details) {
            start = _start;
            end = _end;
            name = _name;
            details = _details;
        }

        string as_string() {
            return (name + ": " + details + "\nFile " + start.fileName + ", line " + to_string(start.line + 1));
        }
};

class IllegalCharacterError : public Error {
    public:
        void init(Position _start, Position _end, string _details) {
            start = _start;
            end = _end;
            name = "IllegalCharacterError";
            details = _details;
        }
};

class InvalidSyntaxError : public Error {
    public:
        void init(Position _start, Position _end, string _details) {
            start = _start;
            end = _end;
            name = "InvalidSyntaxError";
            details = _details;
        }
};

class RuntimeError : public Error {
    public:
        void init(Position _start, Position _end, string _details) {
            start = _start;
            end = _end;
            name = "RuntimeError";
            details = _details;
        }
};