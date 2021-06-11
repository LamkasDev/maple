#pragma once
using namespace std;

class ParserStore {
    public:
        list<shared_ptr<Token>> tokens;
        shared_ptr<Token> current_t = nullptr;
        vector<string> object_keywords;
        int index = 0;

        ParserStore() {

        }

        ParserStore(vector<string> _object_keywords, list<shared_ptr<Token>> _tokens) {
            object_keywords = _object_keywords;
            tokens = _tokens;
            index = 1;
        }

        int advance() {
            index++;
            update_current_token();
            return 1;
        }

        /* TODO: is this actually intended to not bring the tokens back? */
        int unadvance(int ammount) {
            index -= ammount;
            update_current_token();
            return 1;
        }

        void update_current_token() {
            if(tokens.size() > 0) {
                current_t = tokens.front();
                tokens.pop_front();
            } else {
                current_t = nullptr;
            }
        }
};