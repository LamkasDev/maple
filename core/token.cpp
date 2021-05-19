#pragma once
#include <vector>
#include <string>
#include <list>
#include "error.cpp"
using namespace std;

const vector<string> KEYWORDS { "VAR", "AND", "OR", "NOT", "IF", "THEN", "ELSE", "ELIF", "FOR", "TO", "STEP", "WHILE", "FUNC" }; 
const string LETTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const string LETTERS_DIGITS = LETTERS + "0123456789";
const string DIGITS = "0123456789";

const string TT_UNKNOWN = "UNKNOWN";
const string TT_INT = "INT";
const string TT_FLOAT = "FLOAT";
const string TT_STRING = "STRING";

const string TT_IDENFIFIER = "IDENTIFIER";
const string TT_KEYWORD = "KEYWORD";
const string TT_EQ = "EQ";

const string TT_EQEQ = "EQEQ";
const string TT_NEQ = "NEQ";
const string TT_LTHAN = "LTHAN";
const string TT_GTHAN = "GTHAN";
const string TT_LTHANEQ = "LTHANEQ";
const string TT_GTHANEQ = "GTHANEQ";

const string TT_PLUS = "PLUS";
const string TT_MINUS = "MINUS";
const string TT_MUL = "MUL";
const string TT_DIV = "DIV";
const string TT_POW = "POW";
const string TT_MOD = "MOD";

const string TT_LPAREN = "LPAREN";
const string TT_RPAREN = "RPAREN";
const string TT_COMMA = "COMMA";
const string TT_ARROW = "ARROW";
const string TT_EOF = "EOF";

const string KEYWORD_VAR = "VAR";
const string KEYWORD_AND = "AND";
const string KEYWORD_OR = "OR";
const string KEYWORD_NOT = "NOT";
const string KEYWORD_IF = "IF";
const string KEYWORD_THEN = "THEN";
const string KEYWORD_ELSE = "ELSE";
const string KEYWORD_ELIF = "ELIF";
const string KEYWORD_FOR = "FOR";
const string KEYWORD_TO = "TO";
const string KEYWORD_STEP = "STEP";
const string KEYWORD_WHILE = "WHILE";
const string KEYWORD_FUNC = "FUNC";

class Token {
    public:
        Position start;
        Position end;
        string type = TT_UNKNOWN;
        int value_int = 0;
        float value_float = 0;
        string value_string = "";

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
            copy->value_string = value_string;

            return copy;
        }

        string repr() {
            if(type == TT_INT) {
                return type + ":" + to_string(value_int);
            } else if(type == TT_FLOAT) {
                return type + ":" + to_string(value_float);
            } else if(type == TT_STRING) {
                return type + ":" + value_string;
            } else if(type == TT_IDENFIFIER) {
                return type + ":" + value_string;
            } else if(type == TT_KEYWORD) {
                return type + ":" + value_string;
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

class TokenString : public Token {
    public:
        void init(string _value) {
            type = TT_STRING;
            value_string = _value;
        }
};

class MakeTokensResult {
    public:
        int state = 0;
        Error e;
        list<Token*> tokens;
};