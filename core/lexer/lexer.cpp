#pragma once
#include <string>
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

                    case '^': {
                        Token t;
                        t.init(TT_POW);
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