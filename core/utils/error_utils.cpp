#pragma once
using namespace std;

string generate_traceback(shared_ptr<Context> context) {
    string res = "";
    shared_ptr<Context> _context = context;
    while(_context != nullptr) {
        res = "  File " + _context->file_name + ", line " + to_string(_context->entry_pos.line + 1) + ", in " + _context->display_name + "\n" + res;
        _context = _context->parent;
    }

    return "Traceback (most recent call last):\n" + res;
}