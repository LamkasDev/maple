#pragma once

#include "runner_result.cpp"

#include "../core/context.cpp"
#include "../core/position.cpp"
#include "../core/token.cpp"

#include "../core/builtin/builtin_runner.cpp"

#include "../core/errors/error_types.cpp"
#include "../core/errors/error.cpp"

#include "../core/interpreter/interpreter.cpp"
#include "../core/interpreter/interpreter_result.cpp"
#include "../core/lexer/lexer.cpp"

#include "../core/nodes/node.cpp"
#include "../core/nodes/node_value.cpp"

#include "../core/parser/parser.cpp"
#include "../core/parser/parser_result.cpp"

#include "../core/symbols/symbol_table.cpp"
#include "../core/symbols/symbol_container.cpp"

#include "../core/utils/basic_utils.cpp"
#include "../core/utils/other_utils.cpp"

#include "../structures/number.cpp"
#include "../structures/function.cpp"
#include "../structures/string.cpp"
#include "../structures/object.cpp"
#include "../structures/object_prototype.cpp"
#include "../structures/list.cpp"

using namespace std;

class Runner {
    public:
        shared_ptr<Interpreter> interpreter = nullptr;
        vector<string> object_keywords;

        Runner() {
            interpreter = make_shared<Interpreter>();
            object_keywords.push_back("OBJECT");
        }

        RunResult run(string _fileName, string _text) {
            RunResult result;

            Lexer lexer(object_keywords, _fileName, _text);
            result.set_lexer(lexer);
            MakeTokensResult makeTokensResult = lexer.make_tokens();
            result.set_make_tokens_result(makeTokensResult);
            if(makeTokensResult.state == -1) {
                return result;
            }

            object_keywords = lexer.object_keywords;

            Parser parser(object_keywords, makeTokensResult.tokens);
            ParserResult parserResult = parser.parse();
            result.set_parser_result(parserResult);
            if(parserResult.state == -1) {
                return result;
            }

            InterpreterResult interpreterResult;
            interpreterResult = interpreter->visit_node(parserResult.node, interpreter->context);
            result.set_interpreter_result(interpreterResult);
            if(interpreterResult.state == -1) {
                return result;
            }
            
            return result;
        }
};