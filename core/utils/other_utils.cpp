#pragma once
using namespace std;

string print_tree(list<shared_ptr<Token>> tokens) {
    string s = "(";
    for (shared_ptr<Token> t : tokens) {
        s += t->repr() + ", ";
    }

    s = s.length() > 1 ? s.substr(0, s.length() - 2) + ")" : s;
    return s;
}