#pragma once

#include "runner_result.cpp"

#include "../core/context.cpp"
#include "../core/position.cpp"
#include "../core/token.cpp"
#include "../core/utils.cpp"

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

#include "../structures/number.cpp"
#include "../structures/function.cpp"
#include "../structures/string.cpp"
#include "../structures/object.cpp"

using namespace std;

class Runner {
    public:
        shared_ptr<Interpreter> interpreter;

        Runner() {
            interpreter = make_shared<Interpreter>();
        }

        RunResult run(string _fileName, string _text) {
            RunResult result;

            Lexer lexer(_fileName, _text);
            result.set_lexer(lexer);

            MakeTokensResult makeTokensResult = lexer.make_tokens();
            result.set_make_tokens_result(makeTokensResult);
            if(makeTokensResult.state == -1) {
                return result;
            }

            Parser parser(makeTokensResult.tokens);
            ParserResult parserResult = parser.parse();
            result.set_parser_result(parserResult);
            if(parserResult.state == -1) {
                return result;
            }

            shared_ptr<Context> context = make_shared<Context>(_fileName);
            context->set_symbol_table(interpreter->global_symbol_table);

            InterpreterResult interpreterResult;
            string node_type = parserResult.node_type;
            //printf("Root Node: %s\n", node_type.c_str());
            interpreterResult = interpreter->visit_node(parserResult.node, context);

            result.set_interpreter_result(interpreterResult);
            
            return result;
        }
};