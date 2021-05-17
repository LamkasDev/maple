#pragma once
#include <string>
#include "../utils.cpp"
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
                        Token* t = new Token();
                        t->init(TT_PLUS);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '-': {
                        Token* t = new Token();
                        t->init(TT_MINUS);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '*': {
                        Token* t = new Token();
                        t->init(TT_MUL);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '/': {
                        Token* t = new Token();
                        t->init(TT_DIV);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '^': {
                        Token* t = new Token();
                        t->init(TT_POW);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '%': {
                        Token* t = new Token();
                        t->init(TT_MOD);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '(': {
                        Token* t = new Token();
                        t->init(TT_LPAREN);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case ')': {
                        Token* t = new Token();
                        t->init(TT_RPAREN);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '=': {
                        Token* t = new Token();
                        t->init(TT_EQ);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    default: {
                        if (DIGITS.find(current_c) != string::npos) {
                            Token* t = make_number();
                            result.tokens.push_back(t);
                        } else if (LETTERS.find(current_c) != string::npos) {
                            Token* t = make_identifier();
                            result.tokens.push_back(t);
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

            Token* t = new Token();
            t->init(TT_EOF);
            t->set_start(pos);
            result.tokens.push_back(t);

            result.state = 0;
            return result;
        }

        Token* make_number() {
            string str;
            int dot = 0;
            while(pos.index < text.length() && (DIGITS + ".").find(current_c) != string::npos) {
                if(current_c == '.') {
                    if(dot == 1) { break; }
                    dot++;
                }

                str += current_c;
                advance();
            }
            
            if(dot == 0) {
                TokenInt* t = new TokenInt();
                t->init(stoi(str));
                t->set_start(pos.copy());
                t->set_end(pos.copy());

                return t;
            } else {
                TokenFloat* t = new TokenFloat();
                t->init(atof(str.c_str()));
                t->set_start(pos.copy());
                t->set_end(pos.copy());

                return t;
            }
        }

        Token* make_identifier() {
            string str;
            while(pos.index < text.length() && (LETTERS_DIGITS + '_').find(current_c) != string::npos) {
                str += current_c;
                advance();
            }

            if(in_array(str, KEYWORDS)) {
                TokenKeyword* t = new TokenKeyword();
                t->init(str);
                t->set_start(pos.copy());
                t->set_end(pos.copy());

                return t;
            } else {
                TokenIdentifier* t = new TokenIdentifier();
                t->init(str);
                t->set_start(pos.copy());
                t->set_end(pos.copy());

                return t;
            }
        }
};