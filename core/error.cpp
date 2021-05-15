#pragma once
#include <string>
#include "position.cpp"
using namespace std;

const string ERROR_UNKNOWN = "UNKNOWN";
const string ERROR_DEFAULT = "DEFAULT";
const string ERROR_RUNTIME = "RUNTIME";

class Error {
    public:
        Position start;
        Position end;
        Context* context = nullptr;

        string type = ERROR_UNKNOWN;
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

            type = ERROR_DEFAULT;
            name = "IllegalCharacterError";
            details = _details;
        }
};

class InvalidSyntaxError : public Error {
    public:
        void init(Position _start, Position _end, string _details) {
            start = _start;
            end = _end;

            type = ERROR_DEFAULT;
            name = "InvalidSyntaxError";
            details = _details;
        }
};

class RuntimeError : public Error {
    public:
        void init(Position _start, Position _end, string _details, Context* _context) {
            start = _start;
            end = _end;

            type = ERROR_RUNTIME;
            name = "RuntimeError";
            details = _details;
            context = _context;
        }
};