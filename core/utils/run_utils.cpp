#pragma once
using namespace std;

RunResult run(string _fileName, string _text) {
    RunResult result;

    Lexer lexer(object_keywords, _fileName, _text);
    result.set_lexer(lexer);
    MakeTokensResult makeTokensResult = lexer.make_tokens();
    result.set_make_tokens_result(makeTokensResult);
    if(makeTokensResult.state == -1) {
        result.state = -1;
        return result;
    }

    object_keywords = lexer.object_keywords;

    Parser parser(object_keywords, makeTokensResult.tokens);
    ParserResult parserResult = parser.parse();
    result.set_parser_result(parserResult);
    if(parserResult.state == -1) {
        result.state = -1;
        return result;
    }

    InterpreterResult interpreterResult;
    interpreterResult = interpreter->visit_node(parserResult.node, interpreter->context);
    result.set_interpreter_result(interpreterResult);
    if(interpreterResult.state == -1) {
        result.state = -1;
        return result;
    }
    
    return result;
}