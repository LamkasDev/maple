#pragma once
using namespace std;

const string SYMBOL_LIST_UNKNOWN = "UNKNOWN";
const string SYMBOL_LIST_INT = "INT";
const string SYMBOL_LIST_FLOAT = "FLOAT";
const string SYMBOL_LIST_STRING = "STRING";

class List {
    public:
        Position start;
        Position end;
        shared_ptr<Context> context = nullptr;
        int state = 0;

        vector<int> list_ints;
        vector<float> list_floats;
        vector<string> list_strings;
        string type = SYMBOL_LIST_UNKNOWN;

        void add_value(int _value) {
            list_ints.push_back(_value);
            type = SYMBOL_LIST_INT;
        }

        void add_value(float _value) {
            list_floats.push_back(_value);
            type = SYMBOL_LIST_FLOAT;
        }

        void add_value(string _value) {
            list_strings.push_back(_value);
            type = SYMBOL_LIST_STRING;
        }

        void set_pos(Position _start, Position _end) {
            start = _start;
            end = _end;
        }

        void set_context(shared_ptr<Context> _context) {
            context = _context;
        }

        string repr() {
            return type;
        }
};
