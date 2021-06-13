#pragma once
#include "error.cpp"
using namespace std;

class IllegalCharacterError : public Error {
    public:
        IllegalCharacterError() {
            
        }
        
        IllegalCharacterError(string _file_name, Position _start, Position _end, string _details) {
            file_name = _file_name;
            start = _start;
            end = _end;

            type = ERROR_DEFAULT;
            name = "IllegalCharacterError";
            details = _details;
        }
};

class InvalidSyntaxError : public Error {
    public:
        InvalidSyntaxError() {
            
        }

        InvalidSyntaxError(string _file_name, Position _start, Position _end, string _details) {
            file_name = _file_name;
            start = _start;
            end = _end;

            type = ERROR_DEFAULT;
            name = "InvalidSyntaxError";
            details = _details;
        }
};

class RuntimeError : public Error {
    public:
        RuntimeError() {
            
        }
        
        RuntimeError(Position _start, Position _end, string _details, string _traceback) {
            start = _start;
            end = _end;

            type = ERROR_RUNTIME;
            name = "RuntimeError";
            details = _details;
            traceback = _traceback;
        }
};

class ExpectedCharacterError : public Error {
    public:
        ExpectedCharacterError() {
            
        }
        
        ExpectedCharacterError(string _file_name, Position _start, Position _end, string _details) {
            file_name = _file_name;
            start = _start;
            end = _end;

            type = ERROR_RUNTIME;
            name = "ExpectedCharacterError";
            details = _details;
        }
};