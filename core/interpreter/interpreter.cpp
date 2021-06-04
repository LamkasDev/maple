#pragma once
using namespace std;

class Interpreter {
    public:
        shared_ptr<BuiltInRunner> builtin_runner = nullptr;
        map<string, function<InterpreterResult(Interpreter*, shared_ptr<Node>, shared_ptr<Context>)>> visit_functions;

        shared_ptr<Context> context = nullptr;
        shared_ptr<Object> context_object = nullptr;
        map<string, shared_ptr<Object>> objects;
        map<string, shared_ptr<ObjectPrototype>> object_prototypes;

        Interpreter() {
            SymbolContainer sc_null(0);
            SymbolContainer sc_true(1);
            SymbolContainer sc_false(0);
            
            context = make_shared<Context>("global");
            context->symbol_table->set("NULL", sc_null);
            context->symbol_table->set("TRUE", sc_true);
            context->symbol_table->set("FALSE", sc_false);

            builtin_runner = make_shared<BuiltInRunner>();
            function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> run_func;
            list<string> arguments;
            run_func = &BuiltInRunner::run_print; arguments.clear(); arguments.push_back("value");
            add_builtin_function("print", arguments, run_func);
            run_func = &BuiltInRunner::run_input; arguments.clear();
            add_builtin_function("input", arguments, run_func);
            run_func = &BuiltInRunner::run_is_nan; arguments.clear(); arguments.push_back("value");
            add_builtin_function("is_nan", arguments, run_func);
            run_func = &BuiltInRunner::run_parse_int; arguments.clear(); arguments.push_back("value");
            add_builtin_function("parse_int", arguments, run_func);
            run_func = &BuiltInRunner::run_parse_float; arguments.clear(); arguments.push_back("value");
            add_builtin_function("parse_float", arguments, run_func);
            /*run_func = &BuiltInRunner::run_fetch; arguments.clear(); arguments.push_back("address");
            add_builtin_function("fetch", arguments, run_func);*/

            function<InterpreterResult(Interpreter*, shared_ptr<Node>, shared_ptr<Context>)>visit_func;
            visit_func = &Interpreter::visit_int_node;
            visit_functions.insert_or_assign(NODE_INT, visit_func);
            visit_func = &Interpreter::visit_float_node;
            visit_functions.insert_or_assign(NODE_FLOAT, visit_func);
            visit_func = &Interpreter::visit_unary_node;
            visit_functions.insert_or_assign(NODE_UNARY, visit_func);
            visit_func = &Interpreter::visit_binary_node;
            visit_functions.insert_or_assign(NODE_BINARY, visit_func);
            visit_func = &Interpreter::visit_variable_access_node;
            visit_functions.insert_or_assign(NODE_ACCESS, visit_func);
            visit_func = &Interpreter::visit_variable_assignment_node;
            visit_functions.insert_or_assign(NODE_ASSIGNMENT, visit_func);
            visit_func = &Interpreter::visit_if_node;
            visit_functions.insert_or_assign(NODE_IF, visit_func);
            visit_func = &Interpreter::visit_for_node;
            visit_functions.insert_or_assign(NODE_FOR, visit_func);
            visit_func = &Interpreter::visit_foreach_node;
            visit_functions.insert_or_assign(NODE_FOREACH, visit_func);
            visit_func = &Interpreter::visit_while_node;
            visit_functions.insert_or_assign(NODE_WHILE, visit_func);
            visit_func = &Interpreter::visit_func_call_node;
            visit_functions.insert_or_assign(NODE_FUNC_CALL, visit_func);
            visit_func = &Interpreter::visit_func_def_node;
            visit_functions.insert_or_assign(NODE_FUNC_DEF, visit_func);
            visit_func = &Interpreter::visit_string_node;
            visit_functions.insert_or_assign(NODE_STRING, visit_func);
            visit_func = &Interpreter::visit_list_node;
            visit_functions.insert_or_assign(NODE_LIST, visit_func);
            visit_func = &Interpreter::visit_statements_node;
            visit_functions.insert_or_assign(NODE_STATEMENTS, visit_func);
            visit_func = &Interpreter::visit_return_node;
            visit_functions.insert_or_assign(NODE_RETURN, visit_func);
            visit_func = &Interpreter::visit_continue_node;
            visit_functions.insert_or_assign(NODE_CONTINUE, visit_func);
            visit_func = &Interpreter::visit_break_node;
            visit_functions.insert_or_assign(NODE_BREAK, visit_func);
            visit_func = &Interpreter::visit_object_new_node;
            visit_functions.insert_or_assign(NODE_OBJECT_NEW, visit_func);
            visit_func = &Interpreter::visit_class_def_node;
            visit_functions.insert_or_assign(NODE_CLASS_DEF, visit_func);
            visit_func = &Interpreter::visit_chained_node;
            visit_functions.insert_or_assign(NODE_CHAINED, visit_func);
            visit_func = &Interpreter::visit_chained_assignment_node;
            visit_functions.insert_or_assign(NODE_CHAINED_ASSIGNMENT, visit_func);

            shared_ptr<Node> default_body = make_shared<Node>();
            default_body->type = NODE_STATEMENTS;

            shared_ptr<ObjectPrototype> prototype_object = make_shared<ObjectPrototype>();
            prototype_object->set_body(default_body);
            object_prototypes.insert_or_assign("OBJECT", prototype_object);
        }

        void add_builtin_function(string name, list<string> arguments, function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> function) {
            shared_ptr<Function> f = builtin_runner->create_builtin_function(name, arguments, function);
            builtin_runner->add_builtin_function(name, function);
            context->functions[f->name->value_string] = f;
        }

        InterpreterResult visit_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            if(node->type == NODE_CONSTRUCTOR_DEF) {
                RuntimeError e(node->start, node->end, "Constructor outside of class definition");
                res.failure(e);
            } else {
                try {
                    function<InterpreterResult(Interpreter*, shared_ptr<Node>, shared_ptr<Context>)> visit_func = visit_functions.at(node->type);
                    res = visit_func(this, node, _context);
                } catch(out_of_range e) {
                    no_visit_method(node->type);
                }
            }

            return res;
        }

        InterpreterResult visit_int_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            IntNumber n(node->value.value_int);
            n.set_pos(node->start, node->end);

            InterpreterResult res;
            res.set_pos(n.start, n.end);
            res.set_from(n);

            return res.success();
        }

        InterpreterResult visit_float_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            FloatNumber n(node->value.value_float);
            n.set_pos(node->start, node->end);

            InterpreterResult res;
            res.set_pos(n.start, n.end);
            res.set_from(n);

            return res.success();
        }

        InterpreterResult visit_string_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            String n(node->value.value_string);
            n.set_pos(node->start, node->end);

            InterpreterResult res;
            res.set_pos(n.start, n.end);
            res.set_from(n);

            return res.success();
        }

        InterpreterResult visit_list_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            shared_ptr<List> n = make_shared<List>();
            n->set_pos(node->start, node->end);

            InterpreterResult res;
            res.set_pos(n->start, n->end);
            for(shared_ptr<Node> _node : node->list_nodes_result) {
                InterpreterResult value_res = res.register_result(visit_node(_node, _context));
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

        InterpreterResult visit_object_new_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            shared_ptr<Context> new_context = generate_new_context("obj_context", context);
            shared_ptr<Object> n = make_shared<Object>(new_context);
            n->set_pos(node->start, node->end);
            n->set_prototype(object_prototypes.at(node->token->value_string));
            
            for(shared_ptr<Node> _node : n->prototype->body->statements_nodes_result) {
                if(_node->type != NODE_CONSTRUCTOR_DEF) {
                    res.register_result(visit_node(_node, n->context));
                }
                if(res.should_return()) { return res; }
            }
            if(n->prototype->constructor != nullptr) {
                res = check_args_constructor(node, _context, res, node->object_argument_nodes_result, n);
                if(res.should_return()) { return res; }

                res = populate_args_constructor(node, _context, res,  node->object_argument_nodes_result, n);
                if(res.should_return()) { return res; }

                res.register_result(visit_node(n->prototype->constructor->func_def_expression_result, n->context));
            }
            if(res.should_return()) { return res; }

            res.set_from(n);
            return res.success();
        }

        InterpreterResult visit_class_def_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            shared_ptr<ObjectPrototype> prot = make_shared<ObjectPrototype>();
            prot->set_body(node->class_def_expression_result);
            for(shared_ptr<Node> _node : node->class_def_expression_result->statements_nodes_result) {
                if(_node->type != NODE_ASSIGNMENT && _node->type != NODE_FUNC_DEF && _node->type != NODE_CONSTRUCTOR_DEF) {
                    RuntimeError e(_node->start, _node->end, "Expected variable assignment, function definition or constructor");
                    res.failure(e);
                    break;
                }
                if(_node->type == NODE_CONSTRUCTOR_DEF) {
                    prot->set_constructor(_node);
                }
            }
            if(res.should_return()) { return res; }

            object_prototypes.insert_or_assign(node->token->value_string, prot);

            return res.success();
        }

        InterpreterResult visit_unary_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            InterpreterResult right = res.register_result(visit_node(node->right, _context));
            if(res.should_return()) { return res; }

            if(node->token->type == TT_PLUS) {
                res.set_from(right);
            } else if(node->token->type == TT_MINUS) {
                shared_ptr<Token> n_t = make_shared<Token>(TT_MUL);
                InterpreterResult n_m_i;
                n_m_i.set_from(-1);

                res = res.process_binary(right, n_t, n_m_i);
            } else if(node->token->matches(TT_KEYWORD, KEYWORD_NOT)) {
                shared_ptr<Token> n_t = make_shared<Token>(TT_EQEQ);
                InterpreterResult n_m_i;
                n_m_i.set_from(0);

                res = res.process_binary(right, n_t, n_m_i);
            }

            return res.success();
        }

        InterpreterResult visit_binary_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);
            
            InterpreterResult left_res;
            InterpreterResult right_res;

            left_res = visit_node(node->left, _context);
            res.register_result(left_res);
            if(res.should_return()) { return res; }

            right_res = visit_node(node->right, _context);
            res.register_result(right_res);
            if(res.should_return()) { return res; }

            res = res.process_binary(left_res, node->token, right_res);
            if(res.should_return()) { return res; }

            return res.success();
        }

        InterpreterResult visit_variable_access_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            SymbolContainer value = _context->symbol_table->get(node->token->value_string);
            if(value.state == 0) {
                if(value.type == SYMBOL_INT) {
                    res.set_from(value.value_int);
                } else if(value.type == SYMBOL_FLOAT) {
                    res.set_from(value.value_float);
                } else if(value.type == SYMBOL_STRING) {
                    res.set_from(value.value_string);
                }
            } else {
                shared_ptr<List> value_list = _context->get_list(node->token->value_string);
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

        InterpreterResult process_equals_op(string token, shared_ptr<Node> node, InterpreterResult res, shared_ptr<Context> _context, InterpreterResult value_res) {
            shared_ptr<Node> previous_node = make_shared<Node>();
            previous_node->set_type(NODE_ACCESS);
            previous_node->set_token(node->token);

            InterpreterResult previous_value;

            previous_value = res.register_result(visit_node(previous_node, _context));
            if(res.should_return()) { return res; }
            
            shared_ptr<Token> _token = make_shared<Token>(token);
            InterpreterResult new_value;
            new_value = res.register_result(res.process_binary(previous_value, _token, value_res));

            if(res.should_return()) { return res; }

            res.set_from(res.register_result(new_value));
            return res;    
        }

        InterpreterResult visit_variable_assignment_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);
            
            InterpreterResult value_res;
            value_res = res.register_result(visit_node(node->right, _context));
            if(res.should_return()) { return res; }

            if (node->assignment_token->type == TT_EQ) {
                res.set_from(value_res);
            } else if (node->assignment_token->type == TT_PLUSEQ) {
                res = process_equals_op(TT_PLUS,node,res,_context,value_res);
                if(res.should_return()) { return res; }
            } else if (node->assignment_token->type == TT_MINUSEQ) {
                res = process_equals_op(TT_MINUS,node,res,_context,value_res);
                if(res.should_return()) { return res; }
            }
            
            save_to_context(node->token->value_string, res, _context);
            return res.success();
        }

        InterpreterResult visit_if_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
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

                cond_res = res.register_result(visit_node(cond, _context));
                if(res.should_return()) { return res; }
                if(cond_res.is_true()) {
                    expr_res = res.register_result(visit_node(_node, _context));
                    if(res.should_return()) { break; }

                    res.set_from(expr_res);
                    break;
                }

                cond = nullptr;
            }
            if(res.type == NODE_UNKNOWN && node->else_result != nullptr) {
                InterpreterResult else_res = res.register_result(visit_node(node->else_result, _context));
                if(res.should_return()) { return res; }
                res.set_from(else_res);

                return res.success();
            }
            if(cond_res.should_return() || expr_res.should_return()) { return res; }

            return res.success();
        }

        InterpreterResult visit_for_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            InterpreterResult start_value = res.register_result(visit_node(node->for_start_result, _context));
            if(res.should_return()) { return res; }
            InterpreterResult end_value = res.register_result(visit_node(node->for_end_result, _context));
            if(res.should_return()) { return res; }

            InterpreterResult step_value;
            if(node->for_step_result != nullptr) {
                step_value = res.register_result(visit_node(node->for_step_result, _context));
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
                _context->symbol_table->set(node->token->value_string, container);
                i += step_value.get_value();

                InterpreterResult expr = res.register_result(visit_node(node->for_expr_result, _context));

                if(res.state == -1) { break; }
                if(res.loop_should_continue == true) { res.reset(); continue; }
                if(res.loop_should_break == true) { res.reset(); break; }
                if(res.has_return_value == true) { res.set_from(expr); res.reset(); break; }
            }
            if(res.should_return()) { return res; }

            return res.success();
        }

        InterpreterResult visit_foreach_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            InterpreterResult list = res.register_result(visit_node(node->for_start_result, _context));
            if(res.should_return()) { return res; }
            if(list.type != NODE_LIST) {
                RuntimeError e(node->start, node->end, "Foreach must contain a valid list");
                return res.failure(e);
            }

            if(list.res_list->type == SYMBOL_LIST_INT) {
                for(int e : list.res_list->list_ints) {
                    SymbolContainer container(e);
                    _context->symbol_table->set(node->token->value_string, container);

                    InterpreterResult expr = res.register_result(visit_node(node->for_expr_result, _context));

                    if(res.state == -1) { break; }
                    if(res.loop_should_continue == true) { res.reset(); continue; }
                    if(res.loop_should_break == true) { res.reset(); break; }
                    if(res.has_return_value == true) { res.set_from(expr); res.reset(); break; }
                }
            } else if(list.res_list->type == SYMBOL_LIST_FLOAT) {
                for(float e : list.res_list->list_floats) {
                    SymbolContainer container(e);
                    context->symbol_table->set(node->token->value_string, container);

                    InterpreterResult expr = res.register_result(visit_node(node->for_expr_result, _context));

                    if(res.state == -1) { break; }
                    if(res.loop_should_continue == true) { res.reset(); continue; }
                    if(res.loop_should_break == true) { res.reset(); break; }
                    if(res.has_return_value == true) { res.set_from(expr); res.reset(); break; }
                }
            } else if(list.res_list->type == SYMBOL_LIST_STRING) {
                for(string e : list.res_list->list_strings) {
                    SymbolContainer container(e);
                    context->symbol_table->set(node->token->value_string, container);

                    InterpreterResult expr = res.register_result(visit_node(node->for_expr_result, _context));

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

        InterpreterResult visit_while_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            while(true) {
                InterpreterResult condition = res.register_result(visit_node(node->while_condition_result, _context));
                if(res.should_return() || condition.is_true() == false) { break; }

                InterpreterResult expr = res.register_result(visit_node(node->while_expr_result, _context));

                if(res.state == -1) { break; }
                if(res.loop_should_continue == true) { res.reset(); continue; }
                if(res.loop_should_break == true) { res.reset(); break; }
                if(res.has_return_value == true) { res.set_from(expr); res.reset(); break; }
            }
            if(res.should_return()) { return res; }

            return res.success();
        }

        InterpreterResult visit_func_def_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            shared_ptr<Function> function = make_shared<Function>();
            function->set_pos(node->start, node->end);
            function->set_name(node->token);
            function->set_arguments(node->func_def_argument_tokens_result);
            function->set_expression(node->func_def_expression_result);
            res.set_from(function);

            if(node->token != nullptr) {
                save_to_context(node->token->value_string, res, _context);
            }

            return res.success();
        }

        InterpreterResult visit_func_call_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            InterpreterResult expr = res.register_result(execute(node->func_call_expression_result, node->func_call_argument_nodes_result, _context));
            if(res.should_return()) { return res; }

            res.set_from(expr);
            return res.success();
        }

        InterpreterResult visit_return_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            InterpreterResult expr = res.register_result(visit_node(node->right, _context));
            if(res.should_return()) { return res; }

            res.set_from(expr);
            return res.success_return();
        }

        InterpreterResult visit_continue_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);
            return res.success_continue();
        }

        InterpreterResult visit_break_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);
            return res.success_break();
        }

        InterpreterResult execute(shared_ptr<Node> node, list<shared_ptr<Node>> arguments, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);
            shared_ptr<Context> new_context = generate_new_context(node->token->value_string, _context);

            shared_ptr<Function> function = _context->get_function(node->token->value_string);
            if(function->state == -1) {
                RuntimeError e(node->start, node->end, "Function '" + node->token->value_string + "' does not exist");
                return res.failure(e);
            }

            res = check_args(node, new_context, res, arguments, function);
            if(res.should_return()) { return res; }

            res = populate_args(node, new_context, res, arguments, function);
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

        shared_ptr<Context> generate_new_context(string name, shared_ptr<Context> _context) {
            shared_ptr<Context> new_context = make_shared<Context>(name);
            new_context->set_parent(_context);
            new_context->set_symbol_table(_context->symbol_table);
            new_context->set_lists(_context->lists);
            new_context->set_functions(_context->functions);

            return new_context;
        }
        
        InterpreterResult check_args(shared_ptr<Node> node, shared_ptr<Context> _context, InterpreterResult res, list<shared_ptr<Node>> arguments, shared_ptr<Function> function) {
            if(arguments.size() > function->arguments.size()) {
                RuntimeError e(node->start, node->end, "Too many args passed into '" + node->token->value_string + "' (Got: " + to_string(arguments.size()) + "/" + to_string(function->arguments.size()) + ")");
                res.failure(e);
            } else if(arguments.size() < function->arguments.size()) {
                RuntimeError e(node->start, node->end, "Not enough args passed into '" + node->token->value_string + "' (Got: " + to_string(arguments.size()) + "/" + to_string(function->arguments.size()) + ")");
                res.failure(e);
            }

            return res;
        }
        
        InterpreterResult check_args_constructor(shared_ptr<Node> node, shared_ptr<Context> _context, InterpreterResult res, list<shared_ptr<Node>> arguments, shared_ptr<Object> object) {
            if(arguments.size() > object->prototype->constructor->func_def_argument_tokens_result.size()) {
                RuntimeError e(node->start, node->end, "Too many args passed into '" + node->token->value_string + "' (Got: " + to_string(arguments.size()) + "/" + to_string(object->prototype->constructor->func_def_argument_tokens_result.size()) + ")");
                res.failure(e);
            } else if(arguments.size() < object->prototype->constructor->func_def_argument_tokens_result.size()) {
                RuntimeError e(node->start, node->end, "Not enough args passed into '" + node->token->value_string + "' (Got: " + to_string(arguments.size()) + "/" + to_string(object->prototype->constructor->func_def_argument_tokens_result.size()) + ")");
                res.failure(e);
            }

            return res;
        }

        InterpreterResult populate_args(shared_ptr<Node> node, shared_ptr<Context> _context, InterpreterResult res, list<shared_ptr<Node>> arguments, shared_ptr<Function> function) {
            int i = 0;
            for(shared_ptr<Node> arg : arguments) {
                shared_ptr<Token> token = function->arguments.at(i);
                InterpreterResult arg_res = res.register_result(visit_node(arg, _context));
                if(arg_res.should_return()) { break; }

                save_to_context(token->value_string, arg_res, _context);
                i++;
            }

            return res;
        }

        InterpreterResult populate_args_constructor(shared_ptr<Node> node, shared_ptr<Context> _context, InterpreterResult res, list<shared_ptr<Node>> arguments, shared_ptr<Object> object) {
            int i = 0;
            for(shared_ptr<Node> arg : arguments) {
                shared_ptr<Token> token = object->prototype->constructor->func_def_argument_tokens_result.at(i);
                InterpreterResult arg_res = res.register_result(visit_node(arg, _context));
                if(arg_res.should_return()) { break; }

                save_to_context(token->value_string, arg_res, _context);
                i++;
            }

            return res;
        }

        void save_to_context(string name, InterpreterResult res, shared_ptr<Context> _context) {
            if(res.type == NODE_INT) {
                SymbolContainer value(res.res_int.value);
                _context->symbol_table->set(name, value);
            } else if(res.type == NODE_FLOAT) {
                SymbolContainer value(res.res_float.value);
                _context->symbol_table->set(name, value);
            } else if(res.type == NODE_FUNC_DEF) {
                _context->functions[name] = res.res_func;
            } else if(res.type == NODE_STRING) {
                SymbolContainer value(res.res_string.value);
                _context->symbol_table->set(name, value);
            } else if(res.type == NODE_LIST) {
                _context->lists[name] = res.res_list;
            } else if(res.type == NODE_OBJECT_NEW) {
                if(context_object == nullptr) {
                    objects[name] = res.res_obj;
                } else {
                    context_object->objects[name] = res.res_obj;
                }
            }
        }

        InterpreterResult visit_statements_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            for(shared_ptr<Node> _node : node->statements_nodes_result) {
                InterpreterResult expr = res.register_result(visit_node(_node, _context));
                res.set_from(expr);
                if(res.should_return()) { break; }
            }
            if(res.should_return()) { return res; }

            return res.success();
        }

        InterpreterResult visit_chained_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            InterpreterResult left = res.register_result(visit_node(node->left, _context));
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

        InterpreterResult visit_chained_assignment_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            InterpreterResult left = res.register_result(visit_node(node->left, _context));
            if(res.should_return()) { return res; }
            if(left.type != NODE_OBJECT_NEW) {
                RuntimeError e(node->start, node->end, "Expected object");
                return res.failure(e);
            }

            InterpreterResult expr = res.register_result(visit_node(node->chained_assigment_result, _context));
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
                shared_ptr<Object> res_err = make_shared<Object>(context);
                res_err->state = -1;
                return res_err;
            }
        }

        void no_visit_method(string type) {
            //printf("No visit method for %s\n", type);
        }
};
