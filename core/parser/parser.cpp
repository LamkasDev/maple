#pragma once
#include "../nodes/node.cpp"
#include "parser_store.cpp"
#include "parser_utils.cpp"
#include "parser_result.cpp"
#include "modules/parse_all.cpp"
using namespace std;

class Parser {
    public:
        shared_ptr<ParserStore> parser_store = nullptr;
        shared_ptr<ParseAll> parse_module = nullptr;

        Parser(vector<string> _object_keywords, list<shared_ptr<Token>> _tokens) {
            parser_store = make_shared<ParserStore>(_object_keywords, _tokens);
            parser_store->advance();

            parse_module = make_shared<ParseAll>();
        }

        ParserResult parse() {
            ParserResult res = parse_module->statements(parser_store);
            if(res.state != -1 && parser_store->current_t->type != TT_EOF) {
                if(res.e.extra == 1) {
                    return res;
                } else {
                    return res.failure(create_syntax_error(parser_store, "'+', '-', '*', '/', '^', '==', '!=', '<', '>', '<=', '>=', 'AND' or 'OR'"));
                }
            }

            return res;
        }
};