#pragma once
#include "../utils.cpp"
#include "../errors/error_types.cpp"
using namespace std;

class Lexer {
    public:
        string fileName = "";
        string text = "";
        Position pos;
        char current_c;

        Lexer() {
            
        }

        Lexer(string _fileName, string _text) {
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
                    case '\n':
                        advance();
                        break;

                    case '+': {
                        shared_ptr<Token> t = make_shared<Token>();
                        t->init(TT_PLUS);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '-': {
                        shared_ptr<Token> t = make_minus();
                        result.tokens.push_back(t);
                        break;
                    }

                    case '*': {
                        shared_ptr<Token> t = make_shared<Token>();
                        t->init(TT_MUL);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '/': {
                        result = process_slash(result);
                        if(result.state == -1) { return result; }
                        break;
                    }

                    case '^': {
                        shared_ptr<Token> t = make_shared<Token>();
                        t->init(TT_POW);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '%': {
                        shared_ptr<Token> t = make_shared<Token>();
                        t->init(TT_MOD);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '(': {
                        shared_ptr<Token> t = make_shared<Token>();
                        t->init(TT_LPAREN);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case ')': {
                        shared_ptr<Token> t = make_shared<Token>();
                        t->init(TT_RPAREN);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '{': {
                        shared_ptr<Token> t = make_shared<Token>();
                        t->init(TT_LCBRACKET);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '}': {
                        shared_ptr<Token> t = make_shared<Token>();
                        t->init(TT_RCBRACKET);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '=': {
                        shared_ptr<Token> t = make_equals();
                        result.tokens.push_back(t);
                        break;
                    }

                    case '<': {
                        shared_ptr<Token> t = make_less_than();
                        result.tokens.push_back(t);
                        break;
                    }

                    case '>': {
                        shared_ptr<Token> t = make_greater_than();
                        result.tokens.push_back(t);
                        break;
                    }

                    case ',': {
                        shared_ptr<Token> t = make_shared<Token>();
                        t->init(TT_COMMA);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '"': {
                        shared_ptr<Token> t = make_string();
                        result.tokens.push_back(t);
                        break;
                    }

                    case ';': {
                        shared_ptr<Token> t = make_shared<Token>();
                        t->init(TT_NEWLINE);
                        t->set_start(pos);
                        result.tokens.push_back(t); advance();
                        break;
                    }

                    case '!': {
                        shared_ptr<Token> t = make_not_equals(result);
                        if(result.state == -1) { return result; }
                        result.tokens.push_back(t);
                        break;
                    }

                    default: {
                        if (DIGITS.find(current_c) != string::npos) {
                            shared_ptr<Token> t = make_number();
                            result.tokens.push_back(t);
                        } else if (LETTERS.find(current_c) != string::npos) {
                            shared_ptr<Token> t = make_identifier();
                            result.tokens.push_back(t);
                        } else {
                            char c = current_c;
                            Position start = pos.copy();
                            advance();

                            IllegalCharacterError e(start, pos, ("'" + string(1, c) + "'").c_str());
                            result.e = e;
                            result.state = -1;
                            return result;
                        }
                    }
                }
            }

            shared_ptr<Token> t = make_shared<Token>();
            t->init(TT_EOF);
            t->set_start(pos);
            result.tokens.push_back(t);

            result.state = 0;
            return result;
        }

        shared_ptr<Token> make_number() {
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
                shared_ptr<TokenInt> t = make_shared<TokenInt>();
                t->init(stoi(str));
                t->set_start(pos.copy());
                t->set_end(pos.copy());

                return t;
            } else {
                shared_ptr<TokenFloat> t = make_shared<TokenFloat>();
                t->init(atof(str.c_str()));
                t->set_start(pos.copy());
                t->set_end(pos.copy());

                return t;
            }
        }

        shared_ptr<Token> make_identifier() {
            string str;
            while(pos.index < text.length() && (LETTERS_DIGITS + '_').find(current_c) != string::npos) {
                str += current_c;
                advance();
            }

            if(in_array(str, KEYWORDS)) {
                shared_ptr<TokenKeyword> t = make_shared<TokenKeyword>();
                t->init(str);
                t->set_start(pos.copy());
                t->set_end(pos.copy());

                return t;
            } else {
                shared_ptr<TokenIdentifier> t = make_shared<TokenIdentifier>();
                t->init(str);
                t->set_start(pos.copy());
                t->set_end(pos.copy());

                return t;
            }
        }

        shared_ptr<Token> make_string() {
            string str;
            bool escaped = false;
            advance();

            while(pos.index < text.length() && (current_c != '"' || escaped == true)) {
                if(escaped == true) {
                    if(current_c == 'n') {
                        str += '\n';
                    } else if(current_c == 't') {
                        str += '\t';
                    } else {
                        str += current_c;
                    }
                    
                    escaped = false;
                } else {
                    if(current_c == '\\') {
                        escaped = true;
                    } else {
                        str += current_c;
                    }
                }

                advance();
            }

            advance();

            shared_ptr<TokenString> t = make_shared<TokenString>();
            t->init(str);
            t->set_start(pos.copy());
            t->set_end(pos.copy());

            return t;
        }

        shared_ptr<Token> make_not_equals(MakeTokensResult result) {
            string str;
            shared_ptr<Token> t = make_shared<Token>();
            t->set_start(pos.copy());
            t->set_end(pos.copy());

            advance();
            if(current_c == '=') {
                advance();
                t->init(TT_NEQ);
            } else {
                ExpectedCharacterError e(pos.copy(), pos.copy(), "'=' (after '!')");
                result.e = e;
                result.state = -1;
            }
            
            return t;
        }

        shared_ptr<Token> make_equals() {
            string str;
            shared_ptr<Token> t = make_shared<Token>();
            t->init(TT_EQ);
            t->set_start(pos.copy());
            t->set_end(pos.copy());

            advance();
            if(current_c == '=') {
                advance();
                t->init(TT_EQEQ);
            }

            return t;
        }

        shared_ptr<Token> make_less_than() {
            string str;
            shared_ptr<Token> t = make_shared<Token>();
            t->init(TT_LTHAN);
            t->set_start(pos.copy());
            t->set_end(pos.copy());

            advance();
            if(current_c == '=') {
                advance();
                t->init(TT_LTHANEQ);
            }

            return t;
        }

        shared_ptr<Token> make_greater_than() {
            string str;
            shared_ptr<Token> t = make_shared<Token>();
            t->init(TT_GTHAN);
            t->set_start(pos.copy());
            t->set_end(pos.copy());

            advance();
            if(current_c == '=') {
                advance();
                t->init(TT_GTHANEQ);
            }

            return t;
        }

        shared_ptr<Token> make_minus() {
            string str;
            shared_ptr<Token> t = make_shared<Token>();
            t->init(TT_MINUS);
            t->set_start(pos.copy());
            t->set_end(pos.copy());

            advance();
            if(current_c == '>') {
                advance();
                t->init(TT_ARROW);
            }

            return t;
        }

        MakeTokensResult process_slash(MakeTokensResult result) {
            shared_ptr<Token> t = make_shared<Token>();
            t->init(TT_DIV);
            t->set_start(pos.copy());
            t->set_end(pos.copy());

            advance();
            if(current_c == '/') {
                advance();
                skip_comment();
                return result;
            } else if(current_c == '*') {
                advance();
                result = skip_long_comment(result);
                return result;
            }

            result.tokens.push_back(t);
            return result;
        }

        void skip_comment() {
            while(current_c != '\n') {
                advance();
            }
            advance();
        }

        MakeTokensResult skip_long_comment(MakeTokensResult result) {
    	    while(current_c != '*') {
                advance();
            }
            advance();

            if(current_c == '/') {
                advance();
            } else {
                ExpectedCharacterError e(pos.copy(), pos.copy(), "'/'");
                result.e = e;
                result.state = -1;
            }

            return result;
        }
};