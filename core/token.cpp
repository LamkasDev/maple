#pragma once
#include <vector>
#include <string>
#include <list>
#include "error.cpp"
using namespace std;

const vector<string> KEYWORDS { "VAR" }; 
const string LETTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const string LETTERS_DIGITS = LETTERS + "0123456789";
const string DIGITS = "0123456789";

const string TT_INT = "TT_INT";
const string TT_FLOAT = "TT_FLOAT";

const string TT_IDENFIFIER = "IDENTIFIER";
const string TT_KEYWORD = "KEYWORD";
const string TT_EQ = "EQ";

const string TT_PLUS = "PLUS";
const string TT_MINUS = "MINUS";
const string TT_MUL = "MUL";
const string TT_DIV = "DIV";
const string TT_POW = "POW";
const string TT_MOD = "MOD";

const string TT_LPAREN = "LPAREN";
const string TT_RPAREN = "RPAREN";
const string TT_EOF = "EOF";

const string KEYWORD_VAR = "VAR";

class Token {
    public:
        Position start;
        Position end;
        string type;
        int value_int = 0;
        float value_float = 0;
        string value_string;

        void init(string _type) {
            type = _type;
        }

        void set_start(Position _start) {
            start = _start.copy();
            end = _start.copy();
            end.advance();
        }

        void set_end(Position _end) {
            end = _end;
        }

        bool matches(string _type, string _value) {
            return type == _type && value_string == _value;
        }

        Token* copy() {
            Token* copy = new Token();
            copy->init(type);
            copy->set_start(start);
            copy->set_end(end);
            copy->value_int = value_int;
            copy->value_float = value_float;

            return copy;
        }

        string repr() {
            if(type == TT_INT) {
                return type + ":" + to_string(value_int);
            } else if(type == TT_FLOAT) {
                return type + ":" + to_string(value_float);
            }

            return type;
        }
};

class TokenKeyword : public Token {
    public:
        void init(string _value) {
            type = TT_KEYWORD;
            value_string = _value;
        }
};

class TokenIdentifier : public Token {
    public:
        void init(string _value) {
            type = TT_IDENFIFIER;
            value_string = _value;
        }
};

class TokenInt : public Token {
    public:
        void init(int _value) {
            type = TT_INT;
            value_int = _value;
        }
};

class TokenFloat : public Token {
    public:
        void init(float _value) {
            type = TT_FLOAT;
            value_float = _value;
        }
};

class MakeTokensResult {
    public:
        int state = 0;
        Error e;
        list<Token> tokens;
};