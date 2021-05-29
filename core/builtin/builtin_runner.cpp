#pragma once
using namespace std;

class BuiltInRunner {
    public:
        map<string, function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)>> builtin_functions;

        shared_ptr<Function> create_builtin_function(string name, list<string> arguments, function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> function) {
            shared_ptr<Function> f = make_shared<Function>();
            f->built_in = true;

            shared_ptr<TokenString> t_name = make_shared<TokenString>();
            t_name->init(name);
            f->set_name(t_name);

            vector<shared_ptr<Token>> arguments_tokens;
            for(string argument : arguments) {
                shared_ptr<TokenString> t_argument = make_shared<TokenString>();
                t_argument->init(argument);
                arguments_tokens.push_back(t_argument);
            }

            f->set_arguments(arguments_tokens);
            builtin_functions.insert_or_assign(name, function);
            return f;
        }

        InterpreterResult run(shared_ptr<Function> _function, shared_ptr<Context> _context) {
            InterpreterResult res;
            try {
                function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> run_function = get_function(_function->name->value_string);
                res = run_function(this, res, _function, _context);
            } catch(out_of_range e_0) {
                RuntimeError e(res.start, res.end, "Function doesn't exist");
                return res.failure(e);
            }
            if(res.state == -1) { return res; }

            return res.success();
        }

        function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> get_function(string _name) {
            function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> value = builtin_functions.at(_name);
            return value;
        }

        InterpreterResult run_print(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            SymbolContainer value = context->symbol_table->get("value");
            if(value.type != SYMBOL_STRING) {
                RuntimeError e(res.start, res.end, "Invalid argument");
                return res.failure(e);
            }
            printf("%s", value.value_string.c_str());
            res.set_from(value.value_string.c_str());

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
                RuntimeError e(res.start, res.end, "Invalid argument");
                return res.failure(e);
            }
            
            return res.success();
        }

        InterpreterResult run_parse_float(InterpreterResult res, shared_ptr<Function> function, shared_ptr<Context> context) {
            SymbolContainer value = context->symbol_table->get("value");
            try {
                res.set_from(stof(value.value_string));
            } catch(invalid_argument e_0) {
                RuntimeError e(res.start, res.end, "Invalid argument");
                return res.failure(e);
            }

            return res.success();
        }
};