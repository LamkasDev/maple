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

            pos.set_location(-1, 0, -1, fileName, text);
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
                        result = add_generic_token(result, TT_PLUS);
                        break;
                    }

                    case '-': {
                        result = make_minus(result);
                        if(result.state == -1) { return result; }
                        break;
                    }

                    case '*': {
                        result = add_generic_token(result, TT_MUL);
                        break;
                    }

                    case '/': {
                        result = process_slash(result);
                        if(result.state == -1) { return result; }
                        break;
                    }

                    case '^': {
                        result = add_generic_token(result, TT_POW);
                        break;
                    }

                    case '%': {
                        result = add_generic_token(result, TT_MOD);
                        break;
                    }

                    case '(': {
                        result = add_generic_token(result, TT_LPAREN);
                        break;
                    }

                    case ')': {
                        result = add_generic_token(result, TT_RPAREN);
                        break;
                    }

                    case '{': {
                        result = add_generic_token(result, TT_LCBRACKET);
                        break;
                    }

                    case '}': {
                        result = add_generic_token(result, TT_RCBRACKET);
                        break;
                    }

                    case '[': {
                        result = add_generic_token(result, TT_LSBRACKET);
                        break;
                    }

                    case ']': {
                        result = add_generic_token(result, TT_RSBRACKET);
                        break;
                    }

                    case '=': {
                        result = make_equals(result);
                        if(result.state == -1) { return result; }
                        break;
                    }

                    case '<': {
                        result = make_less_than(result);
                        if(result.state == -1) { return result; }
                        break;
                    }

                    case '>': {
                        result = make_greater_than(result);
                        if(result.state == -1) { return result; }
                        break;
                    }

                    case ',': {
                        result = add_generic_token(result, TT_COMMA);
                        break;
                    }

                    case '.': {
                        result = add_generic_token(result, TT_DOT);
                        break;
                    }

                    case '"': {
                        result = make_string(result);
                        if(result.state == -1) { return result; }
                        break;
                    }

                    case ';': {
                        result = add_generic_token(result, TT_NEWLINE);
                        break;
                    }

                    case '!': {
                        result = make_not_equals(result);
                        if(result.state == -1) { return result; }
                        break;
                    }

                    default: {
                        if (DIGITS.find(current_c) != string::npos) {
                            result = make_number(result);
                            if(result.state == -1) { return result; }
                        } else if (LETTERS.find(current_c) != string::npos) {
                            result = make_identifier(result);
                            if(result.state == -1) { return result; }
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

        MakeTokensResult add_generic_token(MakeTokensResult result, string type) {
            shared_ptr<Token> t = make_shared<Token>();
            t->init(type);
            t->set_start(pos);
            result.tokens.push_back(t); advance();

            return result;
        }

        MakeTokensResult make_number(MakeTokensResult result) {
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

                result.tokens.push_back(t);
            } else {
                shared_ptr<TokenFloat> t = make_shared<TokenFloat>();
                t->init(atof(str.c_str()));
                t->set_start(pos.copy());
                t->set_end(pos.copy());

                result.tokens.push_back(t);
            }

            return result;
        }

        MakeTokensResult make_identifier(MakeTokensResult result) {
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

                result.tokens.push_back(t);
            } else {
                shared_ptr<TokenIdentifier> t = make_shared<TokenIdentifier>();
                t->init(str);
                t->set_start(pos.copy());
                t->set_end(pos.copy());

                result.tokens.push_back(t);
            }

            return result;
        }

        MakeTokensResult make_string(MakeTokensResult result) {
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

            result.tokens.push_back(t);
            return result;
        }

        MakeTokensResult make_not_equals(MakeTokensResult result) {
            shared_ptr<Token> t = make_shared<Token>();
            t->set_start(pos.copy());
            t->set_end(pos.copy());

            advance();
            if(current_c == '=') {
                advance();
                t->init(TT_NEQ);
                result.tokens.push_back(t);
            } else {
                ExpectedCharacterError e(pos.copy(), pos.copy(), "'=' (after '!')");
                result.e = e;
                result.state = -1;
            }
            
            return result;
        }

        MakeTokensResult make_equals(MakeTokensResult result) {
            shared_ptr<Token> t = make_shared<Token>();
            t->init(TT_EQ);
            t->set_start(pos.copy());
            t->set_end(pos.copy());

            advance();
            if(current_c == '=') {
                advance();
                t->init(TT_EQEQ);
            }

            result.tokens.push_back(t);
            return result;
        }

        MakeTokensResult make_less_than(MakeTokensResult result) {
            shared_ptr<Token> t = make_shared<Token>();
            t->init(TT_LTHAN);
            t->set_start(pos.copy());
            t->set_end(pos.copy());

            advance();
            if(current_c == '=') {
                advance();
                t->init(TT_LTHANEQ);
            }

            result.tokens.push_back(t);
            return result;
        }

        MakeTokensResult make_greater_than(MakeTokensResult result) {
            shared_ptr<Token> t = make_shared<Token>();
            t->init(TT_GTHAN);
            t->set_start(pos.copy());
            t->set_end(pos.copy());

            advance();
            if(current_c == '=') {
                advance();
                t->init(TT_GTHANEQ);
            }

            result.tokens.push_back(t);
            return result;
        }

        MakeTokensResult make_minus(MakeTokensResult result) {
            shared_ptr<Token> t = make_shared<Token>();
            t->init(TT_MINUS);
            t->set_start(pos.copy());
            t->set_end(pos.copy());

            advance();
            if(current_c == '>') {
                advance();
                t->init(TT_ARROW);
            }

            result.tokens.push_back(t);
            return result;
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