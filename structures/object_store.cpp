#pragma once
using namespace std;

class ObjectStore {
    public:
        shared_ptr<Context> context = nullptr;
        shared_ptr<ObjectPrototype> prototype = nullptr;

        void set_context(shared_ptr<Context> _context) {
            context = _context;
        }

        void set_prototype(shared_ptr<ObjectPrototype> _prototype) {
            prototype = _prototype;
        }
};