#pragma once
using namespace std;

class InterpreterStore {
    public:
        map<int, shared_ptr<ListStore>> lists;
        int list_uuid = 0;
        map<int, shared_ptr<ObjectStore>> objects;
        int object_uuid = 0;

        shared_ptr<ObjectStore> get_object_store(int id) {
            try {
                shared_ptr<ObjectStore> value = objects.at(id);
                return value;
            } catch(out_of_range e) {
                return nullptr;
            }
        }

        int get_object_uuid() {
            int current_uuid = object_uuid;
            object_uuid++;
            return current_uuid;
        }

        shared_ptr<ObjectStore> attach_object_store(shared_ptr<Object> _object, shared_ptr<Context> _context, shared_ptr<ObjectPrototype> _prototype) {
            shared_ptr<ObjectStore> object_store = make_shared<ObjectStore>();
            object_store->set_context(_context);
            object_store->set_prototype(_prototype);
            int id = get_object_uuid();
            objects.insert_or_assign(id, object_store);

            _object->set_object_id(id);
            return object_store;
        }

        shared_ptr<ListStore> get_list_store(int id) {
            try {
                shared_ptr<ListStore> value = lists.at(id);
                return value;
            } catch(out_of_range e) {
                return nullptr;
            }
        }

        int get_list_uuid() {
            int current_uuid = list_uuid;
            list_uuid++;
            return current_uuid;
        }

        shared_ptr<ListStore> attach_list_store(shared_ptr<List> _list) {
            shared_ptr<ListStore> list_store = make_shared<ListStore>();
            int id = get_list_uuid();
            lists.insert_or_assign(id, list_store);

            _list->set_list_id(id);
            return list_store;
        }
};