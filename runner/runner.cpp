#pragma once
#include <string>
#include <iostream>
#include <list>

#include "runner_result.cpp"

#include "../core/context.cpp"
#include "../core/error.cpp"
#include "../core/node.cpp"
#include "../core/position.cpp"
#include "../core/token.cpp"
#include "../core/utils.cpp"

#include "../core/parser/parser.cpp"
#include "../core/parser/parser_result.cpp"
#include "../core/interpreter/interpreter.cpp"
#include "../core/interpreter/interpreter_result.cpp"
#include "../core/lexer/lexer.cpp"

#include "../structures/number.cpp"

using namespace std;

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

    InterpreterResult interpreterResult;
    string node_type = parserResult.node_type;
    //printf("Root Node: %s\n", node_type.c_str());
    if(node_type == NODE_INT) {
        interpreterResult = interpreter.visit_int_node(parserResult.node_number, context);
    } else if(node_type == NODE_FLOAT) {
        interpreterResult = interpreter.visit_float_node(parserResult.node_number, context);
    } else if(node_type == NODE_BINARY) {
        interpreterResult = interpreter.visit_binary_node(parserResult.node_binary, context);
    } else if(node_type == NODE_UNARY) {
        interpreterResult = interpreter.visit_unary_node(parserResult.node_unary, context);
    } else {
        interpreter.no_visit_method(node_type);
    }

    result.set_interpreter_result(interpreterResult);
    
    return result;
}