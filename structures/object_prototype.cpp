#pragma once
using namespace std;

class ObjectPrototype {
    public:
        vector<shared_ptr<Token>> arguments;

        void set_arguments(vector<shared_ptr<Token>> _arguments) {
            arguments = _arguments;
        }

};