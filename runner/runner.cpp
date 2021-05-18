#pragma once
#include <string>
#include <iostream>
#include <list>

#include "runner_result.cpp"

#include "../core/context.cpp"
#include "../core/error.cpp"
#include "../core/position.cpp"
#include "../core/token.cpp"
#include "../core/utils.cpp"

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

using namespace std;

class Runner {
    public:
        SymbolTable* global_symbol_table;

        void initialize_global_symbol_table() {
            SymbolContainer* sc_null = new SymbolContainer();
            sc_null->init(0);
            SymbolContainer* sc_true = new SymbolContainer();
            sc_true->init(1);
            SymbolContainer* sc_false = new SymbolContainer();
            sc_false->init(0);

            global_symbol_table = new SymbolTable();
            global_symbol_table->set("NULL", sc_null);
            global_symbol_table->set("TRUE", sc_true);
            global_symbol_table->set("FALSE", sc_false);
        }

        RunResult run(string _fileName, string _text) {
            RunResult result;
            result.init();

            Lexer lexer;
            lexer.init(_fileName, _text);
            result.set_lexer(lexer);

            MakeTokensResult makeTokensResult = lexer.make_tokens();
            result.set_make_tokens_result(makeTokensResult);
            if(makeTokensResult.state == -1) {
                return result;
            }

            Parser parser;
            parser.init(makeTokensResult.tokens);

            ParserResult parserResult = parser.parse();
            result.set_parser_result(parserResult);
            if(parserResult.state == -1) {
                return result;
            }
            
            Interpreter interpreter;
            interpreter.init();
            Context* context = new Context();
            context->init(_fileName);
            context->set_symbol_table(global_symbol_table);

            InterpreterResult interpreterResult;
            string node_type = parserResult.node_type;
            //printf("Root Node: %s\n", node_type.c_str());
            interpreterResult = interpreter.visit_node(parserResult.node, context);

            result.set_interpreter_result(interpreterResult);
            
            return result;
        }
};