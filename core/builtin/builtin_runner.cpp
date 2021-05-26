#pragma once
using namespace std;

class BuiltInRunner {
    public:
        Function* create_builtin_function(string name, list<string> arguments) {
            Function* f = new Function();
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
            return f;
        }

        InterpreterResult run(Function* function, shared_ptr<Context> context) {
            InterpreterResult res;
            if(function->name->value_string == "print") {
                res = run_print(res, function, context);
            } else if(function->name->value_string == "input") {
                res = run_input(res, function, context);
            } else if(function->name->value_string == "is_nan") {
                res = run_is_nan(res, function, context);
            } else if(function->name->value_string == "parse_int") {
                res = run_parse_int(res, function, context);
            } else if(function->name->value_string == "parse_float") {
                res = run_parse_float(res, function, context);
            }
            if(res.state == -1) { return res; }

            return res.success();
        }

        InterpreterResult run_print(InterpreterResult res, Function* function, shared_ptr<Context> context) {
            SymbolContainer value = context->symbol_table->get("value");
            if(value.type != SYMBOL_STRING) {
                RuntimeError e;
                e.init(res.start, res.end, "Invalid argument", context);
                return res.failure(e);
            }
            printf("%s", value.value_string.c_str());
            res.set_from(value.value_string.c_str());

            return res.success();
        }

        InterpreterResult run_input(InterpreterResult res, Function* function, shared_ptr<Context> context) {
            char value[512];
            scanf("%[^\n]%*c", &value);
            res.set_from(string(value));

            return res.success();
        }

        InterpreterResult run_is_nan(InterpreterResult res, Function* function, shared_ptr<Context> context) {
            SymbolContainer value = context->symbol_table->get("value");
            res.set_from(value.type != SYMBOL_INT && value.type != SYMBOL_FLOAT);

            return res.success();
        }

        InterpreterResult run_parse_int(InterpreterResult res, Function* function, shared_ptr<Context> context) {
            SymbolContainer value = context->symbol_table->get("value");
            try {
                res.set_from(stoi(value.value_string));
            } catch(invalid_argument e_0) {
                RuntimeError e;
                e.init(res.start, res.end, "Invalid argument", context);
                return res.failure(e);
            }
            
            return res.success();
        }

        InterpreterResult run_parse_float(InterpreterResult res, Function* function, shared_ptr<Context> context) {
            SymbolContainer value = context->symbol_table->get("value");
            try {
                res.set_from(stof(value.value_string));
            } catch(invalid_argument e_0) {
                RuntimeError e;
                e.init(res.start, res.end, "Invalid argument", context);
                return res.failure(e);
            }

            return res.success();
        }
};