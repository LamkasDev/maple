#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include "parser/parser_result.cpp"
using namespace std;

bool in_array(const string &value, const vector<string> &array){
    return find(array.begin(), array.end(), value) != array.end();
}

string print_tree(list<Token*> tokens) {
    string s = "(";
    for (Token* t : tokens) {
        s += t->repr() + ", ";
    }

    s = s.length() > 1 ? s.substr(0, s.length() - 2) + ")" : s;
    return s;
}

string print_node(ParserResult root) {
    return root.node->repr();
}