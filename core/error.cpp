#pragma once
#include <string>
#include "context.cpp"
#include "utils.cpp"
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
		        s += "\n\n" + print_error(start.fileContents, start, end);
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

        string print_error(string text, Position pos_start, Position pos_end) {
            string result = "";

            int idx_start = text.rfind("\n", 0, pos_start.index); if(idx_start < 0) { idx_start = 0; }
            int idx_end = text.find('\n', idx_start + 1); if(idx_end < 0) { idx_end = text.length(); }

            int line_count = pos_end.line - pos_start.line + 1;
            for(int i = 0; i < line_count; i++) {
                string line = text.substr(idx_start, idx_end - idx_start);
                int col_start = pos_start.column; if(i != 0) { col_start = 0; }
                int col_end = pos_end.column; if(i != line_count - 1) { col_end = line.length() - 1; }

                result += line + "\n";
                result += string(col_start, ' ') + string(col_end - col_start, '^');

                int idx_start = idx_end;
                int idx_end = text.find("\n", idx_start + 1);
                if(idx_end < 0) { idx_end = text.length(); }
            }

            return result;
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