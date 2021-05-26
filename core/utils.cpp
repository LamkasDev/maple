#pragma once
#include "parser/parser_result.cpp"

using namespace std;

bool in_array(const string &value, const vector<string> &array){
    return find(array.begin(), array.end(), value) != array.end();
}

string print_tree(list<shared_ptr<Token>> tokens) {
    string s = "(";
    for (shared_ptr<Token> t : tokens) {
        s += t->repr() + ", ";
    }

    s = s.length() > 1 ? s.substr(0, s.length() - 2) + ")" : s;
    return s;
}

string print_node(ParserResult root) {
    return root.node->repr();
}