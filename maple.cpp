#pragma once
#include <string>
#include <iostream>
#include <list>

#include "core/error.cpp"
#include "core/node.cpp"
#include "core/position.cpp"
#include "core/token.cpp"
#include "core/utils.cpp"

#include "core/parser.cpp"
#include "core/parser_result.cpp"
#include "core/interpreter.cpp"
#include "core/interpreter_result.cpp"

#include "structures/number.cpp"

using namespace std;

class Lexer {
    public:
        string fileName;
        string text;
        Position pos;
        char current_c;

        void init(string _fileName, string _text) {
            fileName = _fileName;
            text = _text;

            pos.init(-1, 0, -1, fileName, text);
            advance();
        }

        void advance() {
            pos.advance(current_c);
            if(pos.index < text.length()) {
                current_c = text.at(pos.index);
            }
        }

        MakeTokensResult make_tokens() {
            MakeTokensResult result;

            while(pos.index < text.length()) {
                switch(current_c) {
                    case ' ':
                    case '\t':
                        advance();
                        break;

                    case '+': {
                        Token t;
                        t.init(TT_PLUS);
                        t.set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '-': {
                        Token t;
                        t.init(TT_MINUS);
                        t.set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '*': {
                        Token t;
                        t.init(TT_MUL);
                        t.set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '/': {
                        Token t;
                        t.init(TT_DIV);
                        t.set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '(': {
                        Token t;
                        t.init(TT_LPAREN);
                        t.set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case ')': {
                        Token t;
                        t.init(TT_RPAREN);
                        t.set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    default: {
                        if (DIGITS.find(current_c) != string::npos) {
                            result.tokens.push_back(make_number());
                        } else {
                            char c = current_c;
                            Position start = pos.copy();
                            advance();

                            IllegalCharacterError e;
                            e.init(start, pos, ("'" + string(1, c) + "'").c_str());
                            result.e = e;
                            result.state = -1;
                            return result;
                        }
                    }
                }
            }

            Token t;
            t.init(TT_EOF);
            t.set_start(pos);
            result.tokens.push_back(t);

            result.state = 0;
            return result;
        }

        Token make_number() {
            string str;
            int dot = 0;
            Position start = pos.copy();

            while(pos.index < text.length() && (DIGITS + ".").find(current_c) != string::npos) {
                if(current_c == '.') {
                    if(dot == 1) { break; }
                    dot++;
                }

                str += current_c;
                advance();
            }
            
            if(dot == 0) {
                TokenInt t;
                t.init(TT_INT, stoi(str));
                t.set_start(start);
                t.set_end(pos);

                return t;
            } else {
                TokenFloat t;
                t.init(TT_FLOAT, atof(str.c_str()));
                t.set_start(start);
                t.set_end(pos);

                return t;
            }
        }
};

class RunResult {
    public:
        Lexer lexer;
        MakeTokensResult makeTokensResult;
        ParserResult parserResult;
        InterpreterResult interpreterResult;

        void init() {

        }

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
};

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

    InterpreterResult interpreterResult;
    string node_type = parserResult.node_type;
    //printf("Root Node: %s\n", node_type.c_str());
    if(node_type == NODE_INT) {
        interpreterResult = interpreter.visit_int_node(parserResult.node_number);
    } else if(node_type == NODE_FLOAT) {
        interpreterResult = interpreter.visit_float_node(parserResult.node_number);
    } else if(node_type == NODE_BINARY) {
        interpreterResult = interpreter.visit_binary_node(parserResult.node_binary);
    } else if(node_type == NODE_UNARY) {
        interpreterResult = interpreter.visit_unary_node(parserResult.node_unary);
    } else {
        interpreter.no_visit_method(node_type);
    }

    result.set_interpreter_result(interpreterResult);
    
    return result;
}