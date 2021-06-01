#pragma once
using namespace std;

InvalidSyntaxError create_syntax_error(shared_ptr<ParserStore> parser_store, string exp, int extra = 0) {
    InvalidSyntaxError e(parser_store->current_t->start, parser_store->current_t->end, "Expected " + exp);
    e.extra = extra;
    return e;
}