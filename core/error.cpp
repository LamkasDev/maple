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
            if(type != ERROR_RUNTIME) {
                string s = name + ": " + details + "\n";
                s += "File " + start.fileName + ", line " + to_string(start.line + 1);
                return s;
            } else {
                string s = generate_traceback();
                s += name + ": " + details + "\n";
                return s;
            }
        }

        string generate_traceback() {
            string res = "";
            Position _start = start;
            Context* _context = context;
            while(_context != nullptr) {
                res = "  File " + start.fileName + ", line " + to_string(start.line + 1) + ", in " + _context->display_name + "\n" + res;
                _start = _context->parent_entry_pos;
                _context = _context->parent;
            }

            return "Traceback (most recent call last):\n" + res;
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