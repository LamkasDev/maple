#pragma once
using namespace std;

class RunResult {
    public:
        Lexer lexer;
        MakeTokensResult makeTokensResult;
        ParserResult parserResult;
        InterpreterResult interpreterResult;
        string file_contents = "";
        int state = 0;

        void set_make_tokens_result(MakeTokensResult _makeTokensResult) {
            makeTokensResult = _makeTokensResult;
        }

        void set_parser_result(ParserResult _parserResult) {
            parserResult = _parserResult;
        }

        void set_interpreter_result(InterpreterResult _interpreterResult) {
            interpreterResult = _interpreterResult;
        }

        void set_lexer(Lexer _lexer) {
            lexer = _lexer;
        }

        void set_file_contents(string _file_contents) {
            file_contents = _file_contents;
        }
};