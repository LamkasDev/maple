#pragma once
using namespace std;

class BuiltInRunner {
    public:
        map<string, function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)>> builtin_functions;
        map<string, shared_ptr<Function>> non_root_functions;

        shared_ptr<ListStore> non_root_arguments = nullptr;
        shared_ptr<InterpreterStore> interpreter_store = nullptr;

        BuiltInRunner(shared_ptr<InterpreterStore> _interpreter_store) {
            interpreter_store = _interpreter_store;
        }

        shared_ptr<Function> create_builtin_function(string name, list<string> arguments, function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> function) {
            shared_ptr<Function> f = make_shared<Function>();
            f->built_in = true;

            shared_ptr<TokenString> t_name = make_shared<TokenString>(name);
            f->set_name(t_name);

            vector<shared_ptr<Token>> arguments_tokens;
            for(string argument : arguments) {
                shared_ptr<TokenString> t_argument = make_shared<TokenString>(argument);
                arguments_tokens.push_back(t_argument);
            }

            f->set_arguments(arguments_tokens);
            return f;
        }

        void add_builtin_function(string name, function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> function) {
            builtin_functions.insert_or_assign(name, function);
        }

        InterpreterResult run(shared_ptr<Function> _function, shared_ptr<Context> _context) {
            InterpreterResult res;
            function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> run_function;
            try {
                run_function = get_function(_function->name->value_string);
            } catch(out_of_range e_0) {
                RuntimeError e(res.start, res.end, "Function callback doesn't exist", generate_traceback(_context));
                return res.failure(e);
            }
            res = run_function(this, res, _function, _context);
            if(res.should_return()) { return res; }

            return res.success();
        }

        function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> get_function(string _name) {
            function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> value = builtin_functions.at(_name);
            return value;
        }

        InterpreterResult run_print(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string string_value = "";

            SymbolContainer value = context->symbol_table->get("value");
            if(value.type == SYMBOL_STRING) {
                string_value = value.value_string;
            } else if(value.type == SYMBOL_INT) {
                string_value = to_string(value.value_int);
            } else if(value.type == SYMBOL_FLOAT) {
                string_value = to_string(value.value_float).substr(0, to_string(value.value_float).find(".")+4);
            } else {
                RuntimeError e(res.start, res.end, "Invalid argument", generate_traceback(context));
                return res.failure(e);
            }
            printf("%s", string_value.c_str());
            res.set_from(string_value.c_str());

            return res.success();
        }

        InterpreterResult run_input(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            char value[512];
            scanf("%[^\n]%*c", &value);
            res.set_from(string(value));

            return res.success();
        }

        InterpreterResult run_is_nan(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            SymbolContainer value = context->symbol_table->get("value");
            res.set_from(value.type != SYMBOL_INT && value.type != SYMBOL_FLOAT);

            return res.success();
        }

        InterpreterResult run_parse_int(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            SymbolContainer value = context->symbol_table->get("value");
            try {
                res.set_from(stoi(value.value_string));
            } catch(invalid_argument e_0) {
                RuntimeError e(res.start, res.end, "Invalid argument", generate_traceback(context));
                return res.failure(e);
            }
            
            return res.success();
        }

        InterpreterResult run_parse_float(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            SymbolContainer value = context->symbol_table->get("value");
            try {
                res.set_from(stof(value.value_string));
            } catch(invalid_argument e_0) {
                RuntimeError e(res.start, res.end, "Invalid argument", generate_traceback(context));
                return res.failure(e);
            }

            return res.success();
        }

        InterpreterResult run_run_builtin_function(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            SymbolContainer func_name = context->symbol_table->get("func_name");
            shared_ptr<Function> non_root_function;
            try {
                non_root_function = non_root_functions.at(func_name.value_string);
            } catch(out_of_range e_0) {
                RuntimeError e(res.start, res.end, "Function doesn't exist", generate_traceback(context));
                return res.failure(e);
            }

            InterpreterResult result = run(non_root_function, context);
            if(result.should_return()) { return result; }

            return result.success();
        }
        
        InterpreterResult run_http_fetch(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string address = non_root_arguments->list_symbols[0].value_string;
            try {
                httplib::Headers headers = {
                    { "Accept-Encoding", "gzip, deflate" },
                    { "User-Agent", "maple/1.0" }
                };
                httplib::Params params;
                size_t params_result = address.find("?");
                if(params_result != string::npos) {
                    string params_chunk = address.substr(static_cast<int>(params_result) + 1);
                    vector<string> params_strings = split_string(params_chunk, "&");
                    for(string param_whole : params_strings) {
                        vector<string> param_parts = split_string(param_whole, "=");
                        params.emplace(param_parts.at(0), param_parts.at(1));
                    }

                    address = address.substr(0, static_cast<int>(params_result));
                }

                string path = "/";
                int path_start = 0;
                size_t path_start_a = address.find("//");
                if(path_start_a != string::npos) {
                    path_start = static_cast<int>(path_start_a) + 2;
                }
                size_t path_start_b = address.find("/", path_start);
                if(path_start_b != string::npos) {
                    path = address.substr(path_start_b);
                    address = address.substr(0, path_start_b);
                }

                httplib::Client cli(address.c_str());
                cli.set_follow_location(true);
                
                auto _result = cli.Get(path.c_str(), params, headers);
                if(_result) {
                    res.set_from(_result->body);
                } else {
                    auto err = _result.error();
                    res.set_from(static_cast<int>(err));
                }
            } catch(invalid_argument e_0) {
                RuntimeError e(res.start, res.end, "Something went wrong", generate_traceback(context));
                return res.failure(e);
            }

            return res.success();
        }
        
        InterpreterResult run_string_char_at(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            int pos = non_root_arguments->list_symbols[1].value_int;
            try {
                res.set_from(string(1, value.at(pos)));
            } catch(out_of_range e_0) {
                RuntimeError e(res.start, res.end, "The index is out of range", generate_traceback(context));
                return res.failure(e);
            }

            return res.success();
        }
        
        InterpreterResult run_string_char_code_at(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            int pos = non_root_arguments->list_symbols[1].value_int;
            try {
                res.set_from(value.at(pos));
            } catch(out_of_range e_0) {
                RuntimeError e(res.start, res.end, "The index is out of range", generate_traceback(context));
                return res.failure(e);
            }

            return res.success();
        }
        
        InterpreterResult run_string_concat(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value;
            for(SymbolContainer _symbol : non_root_arguments->list_symbols) {
                value += _symbol.value_string;
            }

            res.set_from(value);
            return res.success();
        }
        
        InterpreterResult run_string_ends_with(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            string value_c = non_root_arguments->list_symbols[1].value_string;
            bool result = false; res.set_from(result);
            if (value_c.size() > value.size()) { return res.success(); }
            result = equal(value_c.rbegin(), value_c.rend(), value.rbegin());

            res.set_from(result);
            return res.success();
        }
        
        InterpreterResult run_string_starts_with(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            string value_c = non_root_arguments->list_symbols[1].value_string;
            bool result = false; res.set_from(result);
            if (value_c.size() > value.size()) { return res.success(); }
            result = equal(value_c.begin(), value_c.end(), value.begin());

            res.set_from(result);
            return res.success();
        }
        
        InterpreterResult run_string_includes(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            string value_c = non_root_arguments->list_symbols[1].value_string;
            bool result = value.find(value_c) != string::npos;

            res.set_from(result);
            return res.success();
        }
        
        InterpreterResult run_string_index_of(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            string value_c = non_root_arguments->list_symbols[1].value_string;
            size_t result = value.find(value_c);
            if(result == string::npos) {
                res.set_from(-1);
            } else {
                res.set_from(static_cast<int>(result));
            }

            return res.success();
        }
        
        InterpreterResult run_string_last_index_of(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            string value_c = non_root_arguments->list_symbols[1].value_string;
            size_t result = value.rfind(value_c);
            if(result == string::npos) {
                res.set_from(-1);
            } else {
                res.set_from(static_cast<int>(result));
            }

            return res.success();
        }
        
        InterpreterResult run_string_repeat(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            int n = non_root_arguments->list_symbols[1].value_int;
            res.set_from(repeat_string(value, n));

            return res.success();
        }
        
        InterpreterResult run_string_replace(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            string from_value = non_root_arguments->list_symbols[1].value_string;
            string to_value = non_root_arguments->list_symbols[2].value_string;
            replace_string(value, from_value, to_value);
            res.set_from(value);

            return res.success();
        }
        
        InterpreterResult run_string_replace_all(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            string from_value = non_root_arguments->list_symbols[1].value_string;
            string to_value = non_root_arguments->list_symbols[2].value_string;
            replace_string_all(value, from_value, to_value);
            res.set_from(value);

            return res.success();
        }
        
        /*InterpreterResult run_string_split(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            string delim = non_root_arguments->list_symbols[1].value_string;

            vector<string> array = split_string(value, delim);
            shared_ptr<Node> list_node = make_shared<Node>();
            list_node->set_type(NODE_LIST);

            list<shared_ptr<Node>> chunks;
            for(string chunk : array) {
                shared_ptr<Node> chunk_node = make_shared<Node>();
                chunk_node->set_value(chunk);
                chunks.push_back(chunk_node);
            }

            list_node->set_list_nodes_result(chunks);
            res.set_from(list_node);
            return res.success();
        }*/
        
        InterpreterResult run_string_substring(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            int start = non_root_arguments->list_symbols[1].value_int;
            if(non_root_arguments->list_symbols.size() >= 2) {
                int end = non_root_arguments->list_symbols[2].value_int;
                res.set_from(value.substr(start, end - start));
            } else {
                res.set_from(value.substr(start));
            }

            return res.success();
        }
        
        InterpreterResult run_string_to_lower_case(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            value = to_lower_case_string(value);
            res.set_from(value);

            return res.success();
        }
        
        InterpreterResult run_string_to_upper_case(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            value = to_upper_case_string(value);
            res.set_from(value);

            return res.success();
        }
        
        InterpreterResult run_string_trim(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            trim_string(value);
            res.set_from(value);

            return res.success();
        }
        
        InterpreterResult run_string_trim_start(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            ltrim_string(value);
            res.set_from(value);

            return res.success();
        }
        
        InterpreterResult run_string_trim_end(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            rtrim_string(value);
            res.set_from(value);

            return res.success();
        }
        
        InterpreterResult run_string_length(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            string value = non_root_arguments->list_symbols[0].value_string;
            int l = value.length();
            res.set_from(l);

            return res.success();
        }
        
        InterpreterResult run_list_concat(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            shared_ptr<List> list = make_shared<List>();
            shared_ptr<ListStore> list_store = interpreter_store->attach_list_store(list);
            for(SymbolContainer _list : non_root_arguments->list_symbols) {
                shared_ptr<ListStore> _list_store = interpreter_store->get_list_store(_list.value_list->list_id);
                for(SymbolContainer _symbol : _list_store->list_symbols) {
                    list_store->add_value(_symbol);
                }
            }

            res.set_from(list);
            return res.success();
        }
        
        InterpreterResult run_list_push(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            SymbolContainer list = non_root_arguments->list_symbols[0];
            shared_ptr<ListStore> list_store = interpreter_store->get_list_store(list.value_list->list_id);
            SymbolContainer symbol = non_root_arguments->list_symbols[1];
            list_store->add_value(symbol);

            res.set_from(list.value_list);
            return res.success();
        }
        
        InterpreterResult run_list_pop(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            SymbolContainer list = non_root_arguments->list_symbols[0];
            shared_ptr<ListStore> list_store = interpreter_store->get_list_store(list.value_list->list_id);
            SymbolContainer value = list_store->pop_value();

            res.set_from(value);
            return res.success();
        }
        
        InterpreterResult run_list_shift(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            SymbolContainer list = non_root_arguments->list_symbols[0];
            shared_ptr<ListStore> list_store = interpreter_store->get_list_store(list.value_list->list_id);
            SymbolContainer value = list_store->shift_value();

            res.set_from(value);
            return res.success();
        }
        
        /*InterpreterResult run_list_includes(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            shared_ptr<List> list = non_root_arguments->list_lists[0];
            shared_ptr<ListStore> list_store = interpreter_store->get_list_store(list->list_id);
            SymbolContainer symbol = non_root_arguments->list_symbols[0];
            bool result = false;
            for(SymbolContainer _symbol : list_store->list_symbols) {
                result = symbol == _symbol;
                if(result == true) { break; }
            }

            res.set_from(result);
            return res.success();
        }*/
};
