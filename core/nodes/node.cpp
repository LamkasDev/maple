#pragma once
#include <string>
using namespace std;

const string NODE_UNKNOWN = "UNKNOWN";
const string NODE_INT = "INT";
const string NODE_FLOAT = "FLOAT";
const string NODE_BINARY = "BINARY";
const string NODE_UNARY = "UNARY";
const string NODE_ACCESS = "ACCESS";
const string NODE_ASSIGNMENT = "ASSIGNMENT";

class Node {
    public:
        Position start;
        Position end;
        string type = NODE_UNKNOWN;

        void init(string _type) {
            type = _type;
        }

        string repr() {
            return type;
        }
};