#pragma once
#include "error.cpp"
using namespace std;

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
        void init(Position _start, Position _end, string _details, shared_ptr<Context> _context) {
            start = _start;
            end = _end;

            type = ERROR_RUNTIME;
            name = "RuntimeError";
            details = _details;
            context = _context;
        }
};

class ExpectedCharacterError : public Error {
    public:
        void init(Position _start, Position _end, string _details) {
            start = _start;
            end = _end;

            type = ERROR_RUNTIME;
            name = "ExpectedCharacterError";
            details = _details;
        }
};