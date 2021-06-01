#pragma once
using namespace std;

class ObjectPrototype {
    public:
        vector<shared_ptr<Token>> arguments;
        shared_ptr<Node> body = nullptr;
        shared_ptr<Node> constructor = nullptr;

        void set_body(shared_ptr<Node> _body) {
            body = _body;
        }

        void set_constructor(shared_ptr<Node> _constructor) {
            constructor = _constructor;
        }

        void set_arguments(vector<shared_ptr<Token>> _arguments) {
            arguments = _arguments;
        }

};