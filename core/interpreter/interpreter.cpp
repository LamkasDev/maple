#pragma once
using namespace std;

class Interpreter {
    public:
        shared_ptr<BuiltInRunner> builtin_runner = nullptr;

        shared_ptr<Context> context = nullptr;
        shared_ptr<Object> context_object = nullptr;
        map<string, shared_ptr<Object>> objects;

        Interpreter() {
            SymbolContainer sc_null(0);
            SymbolContainer sc_true(1);
            SymbolContainer sc_false(0);
            
            context = make_shared<Context>("global");
            context->symbol_table->set("NULL", sc_null);
            context->symbol_table->set("TRUE", sc_true);
            context->symbol_table->set("FALSE", sc_false);

            builtin_runner = make_shared<BuiltInRunner>();
            function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> run_print = &BuiltInRunner::run_print;
            add_builtin_function("print", "value", run_print);
            function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> run_input = &BuiltInRunner::run_input;
            add_builtin_function("input", run_input);
            function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> run_is_nan = &BuiltInRunner::run_is_nan;
            add_builtin_function("is_nan", "value", run_is_nan);
            function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> run_parse_int = &BuiltInRunner::run_parse_int;
            add_builtin_function("parse_int", "value", run_parse_int);
            function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> run_parse_float = &BuiltInRunner::run_parse_float;
            add_builtin_function("parse_float", "value", run_parse_float);
        }

        void add_builtin_function(string name, function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> function) {
            list<string> arguments;
            shared_ptr<Function> f = builtin_runner->create_builtin_function(name, arguments, function);
            context->functions[f->name->value_string] = f;
        }

        void add_builtin_function(string name, string arg_1, function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> function) {
            list<string> arguments; arguments.push_back(arg_1);
            shared_ptr<Function> f = builtin_runner->create_builtin_function(name, arguments, function);
            context->functions[f->name->value_string] = f;
        }

        InterpreterResult visit_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            InterpreterResult res;
            if(node->type == NODE_INT) {
                res = visit_int_node(node, context);
            } else if(node->type == NODE_FLOAT) {
                res = visit_float_node(node, context);
            } else if(node->type == NODE_UNARY) {
                res = visit_unary_node(node, context);
            } else if(node->type == NODE_BINARY) {
                res = visit_binary_node(node, context);
            } else if(node->type == NODE_ACCESS) {
                res = visit_variable_access_node(node, context);
            } else if(node->type == NODE_ASSIGNMENT) {
                res = visit_variable_assign(node, context);
            } else if(node->type == NODE_IF) {
                res = visit_if_node(node, context);
            } else if(node->type == NODE_FOR) {
                res = visit_for_node(node, context);
            } else if(node->type == NODE_FOREACH) {
                res = visit_foreach_node(node, context);
            } else if(node->type == NODE_WHILE) {
                res = visit_while_node(node, context);
            } else if(node->type == NODE_FUNC_CALL) {
                res = visit_func_call_node(node, context);
            } else if(node->type == NODE_FUNC_DEF) {
                res = visit_func_def_node(node, context);
            } else if(node->type == NODE_STRING) {
                res = visit_string_node(node, context);
            } else if(node->type == NODE_LIST) {
                res = visit_list_node(node, context);
            } else if(node->type == NODE_STATEMENTS) {
                res = visit_statements_node(node, context);
            } else if(node->type == NODE_RETURN) {
                res = visit_return_node(node, context);
            } else if(node->type == NODE_CONTINUE) {
                res = visit_continue_node(node, context);
            } else if(node->type == NODE_BREAK) {
                res = visit_break_node(node, context);
            } else if(node->type == NODE_OBJECT_NEW) {
                res = visit_object_new_node(node, context);
            } else if(node->type == NODE_CHAINED) {
                res = visit_chained_node(node, context);
            } else if(node->type == NODE_CHAINED_ASSIGNMENT) {
                res = visit_chained_assignment_node(node, context);
            } else {
                no_visit_method(node->type);
            }

            return res;
        }

        InterpreterResult visit_int_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            IntNumber n(node->value.value_int);
            n.set_pos(node->start, node->end);

            InterpreterResult res;
            res.set_pos(n.start, n.end);
            res.set_from(n);

            return res.success();
        }

        InterpreterResult visit_float_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            FloatNumber n(node->value.value_float);
            n.set_pos(node->start, node->end);

            InterpreterResult res;
            res.set_pos(n.start, n.end);
            res.set_from(n);

            return res.success();
        }

        InterpreterResult visit_string_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            String n(node->value.value_string);
            n.set_pos(node->start, node->end);

            InterpreterResult res;
            res.set_pos(n.start, n.end);
            res.set_from(n);

            return res.success();
        }

        InterpreterResult visit_list_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            shared_ptr<List> n = make_shared<List>();
            n->set_pos(node->start, node->end);

            InterpreterResult res;
            res.set_pos(n->start, n->end);
            for(shared_ptr<Node> _node : node->list_nodes_result) {
                InterpreterResult value_res = res.register_result(visit_node(_node, context));
                if(res.should_return()) { break; }

                if(n->type != SYMBOL_LIST_UNKNOWN && n->type != value_res.type) {
                    RuntimeError e(_node->start, _node->end, "Mixed types in an array are not allowed");
                    res.failure(e);
                    break;
                }
                if(value_res.type == NODE_INT) {
                    n->add_value(value_res.res_int.value);
                } else if(value_res.type == NODE_FLOAT) {
                    n->add_value(value_res.res_float.value);
                } else if(value_res.type == NODE_STRING) {
                    n->add_value(value_res.res_string.value);
                } else {
                    RuntimeError e(_node->start, _node->end, "Unsupported type in list");
                    res.failure(e);
                    break;
                }
            }
            if(res.should_return()) { return res; }

            res.set_from(n);
            return res.success();
        }

        InterpreterResult visit_object_new_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            shared_ptr<Object> n = make_shared<Object>();
            n->set_pos(node->start, node->end);

            InterpreterResult res;
            res.set_pos(n->start, n->end);
            res.set_from(n);

            check_args_constructor(node, context, res, node->object_argument_nodes_result, n);

            return res.success();
        }

        InterpreterResult visit_unary_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            InterpreterResult right = res.register_result(visit_node(node->right, context));
            if(res.should_return()) { return res; }

            if(node->token->type == TT_PLUS) {
                res.set_from(right);
            } else if(node->token->type == TT_MINUS) {
                shared_ptr<Token> n_t = make_shared<Token>();
                n_t->init(TT_MUL);
                InterpreterResult n_m_i;
                n_m_i.set_from(-1);

                res = res.process_binary(right, n_t, n_m_i);
            } else if(node->token->matches(TT_KEYWORD, KEYWORD_NOT)) {
                shared_ptr<Token> n_t = make_shared<Token>();
                n_t->init(TT_EQEQ);
                InterpreterResult n_m_i;
                n_m_i.set_from(0);

                res = res.process_binary(right, n_t, n_m_i);
            }

            return res.success();
        }

        InterpreterResult visit_binary_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);
            
            InterpreterResult left_res;
            InterpreterResult right_res;

            left_res = visit_node(node->left, context);
            res.register_result(left_res);
            if(res.should_return()) { return res; }

            right_res = visit_node(node->right, context);
            res.register_result(right_res);
            if(res.should_return()) { return res; }

            res = res.process_binary(left_res, node->token, right_res);
            if(res.should_return()) { return res; }

            return res.success();
        }

        InterpreterResult visit_variable_access_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            SymbolContainer value = context->symbol_table->get(node->token->value_string);
            if(value.state == 0) {
                if(value.type == SYMBOL_INT) {
                    res.set_from(value.value_int);
                } else if(value.type == SYMBOL_FLOAT) {
                    res.set_from(value.value_float);
                } else if(value.type == SYMBOL_STRING) {
                    res.set_from(value.value_string);
                }
            } else {
                shared_ptr<List> value_list = context->get_list(node->token->value_string);
                if(value_list->state == 0) {
                    res.set_from(value_list);
                } else {
                    shared_ptr<Object> value_obj = get_obj(node->token->value_string);
                    if(value_obj->state == 0) {
                        res.set_from(value_obj);
                    } else {
                        RuntimeError e(node->start, node->end, node->token->value_string + " is not defined");
                        return res.failure(e);
                    }
                }
            }

            return res.success();
        }

        InterpreterResult visit_variable_assign(shared_ptr<Node> node, shared_ptr<Context> context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);
            
            InterpreterResult value_res;
            value_res = res.register_result(visit_node(node->right, context));
            if(res.should_return()) { return res; }
            res.set_from(value_res);

            save_to_context(node->token->value_string, res, context);
            return res.success();
        }

        InterpreterResult visit_if_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            InterpreterResult res;
            InterpreterResult cond_res;
            InterpreterResult expr_res;
            res.set_pos(node->start, node->end);
            
            shared_ptr<Node> cond = nullptr;
            for(shared_ptr<Node> _node : node->if_results) {
                if(cond == nullptr) {
                    cond = _node;
                    continue;
                }

                cond_res = res.register_result(visit_node(cond, context));
                if(res.should_return()) { return res; }
                if(cond_res.is_true()) {
                    expr_res = res.register_result(visit_node(_node, context));
                    if(res.should_return()) { break; }

                    res.set_from(expr_res);
                    break;
                }

                cond = nullptr;
            }
            if(res.type == NODE_UNKNOWN && node->else_result != nullptr) {
                InterpreterResult else_res = res.register_result(visit_node(node->else_result, context));
                if(res.should_return()) { return res; }
                res.set_from(else_res);

                return res.success();
            }
            if(cond_res.should_return() || expr_res.should_return()) { return res; }

            return res.success();
        }

        InterpreterResult visit_for_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            InterpreterResult start_value = res.register_result(visit_node(node->for_start_result, context));
            if(res.should_return()) { return res; }
            InterpreterResult end_value = res.register_result(visit_node(node->for_end_result, context));
            if(res.should_return()) { return res; }

            InterpreterResult step_value;
            if(node->for_step_result != nullptr) {
                step_value = res.register_result(visit_node(node->for_step_result, context));
                if(res.should_return()) { return res; }
            } else {
                IntNumber n_s(1);
                InterpreterResult n_s_i;
                n_s_i.set_from(n_s);

                step_value = n_s_i;
            }
            
            bool st_dir = step_value.get_value() >= 0 ? true : false;
            int i = start_value.get_value();
            while((st_dir == true && i < end_value.get_value()) || (st_dir == false && i > end_value.get_value())) {
                SymbolContainer container(i);
                context->symbol_table->set(node->token->value_string, container);
                i += step_value.get_value();

                InterpreterResult expr = res.register_result(visit_node(node->for_expr_result, context));

                if(res.state == -1) { break; }
                if(res.loop_should_continue == true) { res.reset(); continue; }
                if(res.loop_should_break == true) { res.reset(); break; }
                if(res.has_return_value == true) { res.set_from(expr); res.reset(); break; }
            }
            if(res.should_return()) { return res; }

            return res.success();
        }

        InterpreterResult visit_foreach_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            InterpreterResult list = res.register_result(visit_node(node->for_start_result, context));
            if(res.should_return()) { return res; }
            if(list.type != NODE_LIST) {
                RuntimeError e(node->start, node->end, "Foreach must contain a valid list");
                return res.failure(e);
            }

            if(list.res_list->type == SYMBOL_LIST_INT) {
                for(int e : list.res_list->list_ints) {
                    SymbolContainer container(e);
                    context->symbol_table->set(node->token->value_string, container);

                    InterpreterResult expr = res.register_result(visit_node(node->for_expr_result, context));

                    if(res.state == -1) { break; }
                    if(res.loop_should_continue == true) { res.reset(); continue; }
                    if(res.loop_should_break == true) { res.reset(); break; }
                    if(res.has_return_value == true) { res.set_from(expr); res.reset(); break; }
                }
            } else if(list.res_list->type == SYMBOL_LIST_FLOAT) {
                for(float e : list.res_list->list_floats) {
                    SymbolContainer container(e);
                    context->symbol_table->set(node->token->value_string, container);

                    InterpreterResult expr = res.register_result(visit_node(node->for_expr_result, context));

                    if(res.state == -1) { break; }
                    if(res.loop_should_continue == true) { res.reset(); continue; }
                    if(res.loop_should_break == true) { res.reset(); break; }
                    if(res.has_return_value == true) { res.set_from(expr); res.reset(); break; }
                }
            } else if(list.res_list->type == SYMBOL_LIST_STRING) {
                for(string e : list.res_list->list_strings) {
                    SymbolContainer container(e);
                    context->symbol_table->set(node->token->value_string, container);

                    InterpreterResult expr = res.register_result(visit_node(node->for_expr_result, context));

                    if(res.state == -1) { break; }
                    if(res.loop_should_continue == true) { res.reset(); continue; }
                    if(res.loop_should_break == true) { res.reset(); break; }
                    if(res.has_return_value == true) { res.set_from(expr); res.reset(); break; }
                }
            } else {
                RuntimeError e(node->start, node->end, "Invalid type to iterate over");
                return res.failure(e);
            }
            if(res.should_return()) { return res; }

            return res.success();
        }

        InterpreterResult visit_while_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            while(true) {
                InterpreterResult condition = res.register_result(visit_node(node->while_condition_result, context));
                if(res.should_return() || condition.is_true() == false) { break; }

                InterpreterResult expr = res.register_result(visit_node(node->while_expr_result, context));

                if(res.state == -1) { break; }
                if(res.loop_should_continue == true) { res.reset(); continue; }
                if(res.loop_should_break == true) { res.reset(); break; }
                if(res.has_return_value == true) { res.set_from(expr); res.reset(); break; }
            }
            if(res.should_return()) { return res; }

            return res.success();
        }

        InterpreterResult visit_func_def_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            shared_ptr<Function> function = make_shared<Function>();
            function->set_pos(node->start, node->end);
            function->set_name(node->token);
            function->set_arguments(node->func_def_argument_tokens_result);
            function->set_expression(node->func_def_expression_result);
            res.set_from(function);

            if(node->token != nullptr) {
                save_to_context(node->token->value_string, res, context);
            }

            return res.success();
        }

        InterpreterResult visit_func_call_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            InterpreterResult expr = res.register_result(execute(node->func_call_expression_result, node->func_call_argument_nodes_result, context));
            if(res.should_return()) { return res; }

            res.set_from(expr);
            return res.success();
        }

        InterpreterResult visit_return_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            InterpreterResult expr = res.register_result(visit_node(node->right, context));
            if(res.should_return()) { return res; }

            res.set_from(expr);
            return res.success_return();
        }

        InterpreterResult visit_continue_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);
            return res.success_continue();
        }

        InterpreterResult visit_break_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);
            return res.success_break();
        }

        InterpreterResult execute(shared_ptr<Node> node, list<shared_ptr<Node>> arguments, shared_ptr<Context> context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);
            shared_ptr<Context> new_context = generate_new_context(node, context);

            shared_ptr<Function> function = context->get_function(node->token->value_string);
            if(res.state == -1) {
                RuntimeError e(node->start, node->end, "Function '" + node->token->value_string + "' does not exist");
                return res.failure(e);
            }

            check_args(node, new_context, res, arguments, function);
            if(res.should_return()) { return res; }

            populate_args(node, new_context, res, arguments, function);
            if(res.should_return()) { return res; }

            InterpreterResult expr;
            if(function->built_in == false) {
                expr = res.register_result(visit_node(function->expression, new_context));
            } else {
                expr = res.register_result(builtin_runner->run(function, new_context));
            }
            if(res.should_return() && res.has_return_value == false) { return res; }

            res.set_from(expr);
            return res.success();
        }

        shared_ptr<Context> generate_new_context(shared_ptr<Node> node, shared_ptr<Context> context) {
            shared_ptr<Context> new_context = make_shared<Context>(node->token->value_string);
            new_context->set_parent(context);
            new_context->set_parent_entry_pos(node->start);
            new_context->set_symbol_table(new_context->parent->symbol_table);

            return new_context;
        }
        
        void check_args(shared_ptr<Node> node, shared_ptr<Context> context, InterpreterResult res, list<shared_ptr<Node>> arguments, shared_ptr<Function> function) {
            if(arguments.size() > function->arguments.size()) {
                RuntimeError e(node->start, node->end, ((arguments.size() - function->arguments.size()) + " too many args passed into " + node->token->value_string));
                res.failure(e);
            } else if(arguments.size() < function->arguments.size()) {
                RuntimeError e(node->start, node->end, ((function->arguments.size() - arguments.size()) + " too few args passed into " + node->token->value_string));
                res.failure(e);
            }
        }
        
        void check_args_constructor(shared_ptr<Node> node, shared_ptr<Context> context, InterpreterResult res, list<shared_ptr<Node>> arguments, shared_ptr<Object> object) {
            if(arguments.size() > object->arguments.size()) {
                RuntimeError e(node->start, node->end, ((arguments.size() - object->arguments.size()) + " too many args passed into " + node->token->value_string));
                res.failure(e);
            } else if(arguments.size() < object->arguments.size()) {
                RuntimeError e(node->start, node->end, ((object->arguments.size() - arguments.size()) + " too few args passed into " + node->token->value_string));
                res.failure(e);
            }
        }

        void populate_args(shared_ptr<Node> node, shared_ptr<Context> context, InterpreterResult res, list<shared_ptr<Node>> arguments, shared_ptr<Function> function) {
            int i = 0;
            for(shared_ptr<Node> arg : arguments) {
                shared_ptr<Token> token = function->arguments.at(i);
                InterpreterResult arg_res = res.register_result(visit_node(arg, context));
                if(arg_res.should_return()) { break; }

                save_to_context(token->value_string, arg_res, context);
                i++;
            }
        }

        void save_to_context(string name, InterpreterResult res, shared_ptr<Context> context) {
            if(res.type == NODE_INT) {
                SymbolContainer value(res.res_int.value);
                context->symbol_table->set(name, value);
            } else if(res.type == NODE_FLOAT) {
                SymbolContainer value(res.res_float.value);
                context->symbol_table->set(name, value);
            } else if(res.type == NODE_FUNC_DEF) {
                context->functions[name] = res.res_func;
            } else if(res.type == NODE_STRING) {
                SymbolContainer value(res.res_string.value);
                context->symbol_table->set(name, value);
            } else if(res.type == NODE_LIST) {
                context->lists[name] = res.res_list;
            } else if(res.type == NODE_OBJECT_NEW) {
                if(context_object == nullptr) {
                    objects[name] = res.res_obj;
                } else {
                    context_object->objects[name] = res.res_obj;
                }
            }
        }

        InterpreterResult visit_statements_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            for(shared_ptr<Node> _node : node->statements_nodes_result) {
                InterpreterResult expr = res.register_result(visit_node(_node, context));
                res.set_from(expr);
                if(res.should_return()) { break; }
            }
            if(res.should_return()) { return res; }

            return res.success();
        }

        InterpreterResult visit_chained_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            InterpreterResult left = res.register_result(visit_node(node->left, context));
            if(res.should_return()) { return res; }
            if(left.type != NODE_OBJECT_NEW) {
                RuntimeError e(node->start, node->end, "Expected object");
                return res.failure(e);
            }

            context_object = left.res_obj;
            InterpreterResult right = res.register_result(visit_node(node->right, left.res_obj->context));
            context_object = nullptr;
            if(res.should_return()) { return res; }

            res.set_from(right);
            return res.success();
        }

        InterpreterResult visit_chained_assignment_node(shared_ptr<Node> node, shared_ptr<Context> context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            InterpreterResult left = res.register_result(visit_node(node->left, context));
            if(res.should_return()) { return res; }
            if(left.type != NODE_OBJECT_NEW) {
                RuntimeError e(node->start, node->end, "Expected object");
                return res.failure(e);
            }

            InterpreterResult expr = res.register_result(visit_node(node->chained_assigment_result, context));
            if(res.should_return()) { return res; }

            context_object = left.res_obj;
            save_to_context(node->right->token->value_string, expr, left.res_obj->context);
            context_object = nullptr;
            
            return res.success();
        }

        shared_ptr<Object> get_obj(string _name) {
            try {
                if(context_object == nullptr) {
                    shared_ptr<Object> value = objects.at(_name);
                    return value;
                } else {
                    shared_ptr<Object> value = context_object->objects.at(_name);
                    return value;
                }
            } catch(out_of_range e) {
                shared_ptr<Object> res_err = make_shared<Object>();
                res_err->state = -1;
                return res_err;
            }
        }

        void no_visit_method(string type) {
            //printf("No visit method for %s\n", type);
        }
};