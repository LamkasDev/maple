#pragma once
#include "../context.cpp"
#include "../utils/error_utils.cpp"
#include "../../structures/number.cpp"
#include "../../structures/string.cpp"
#include "../../structures/object_prototype.cpp"
#include "../../structures/list_store.cpp"
#include "../../structures/map_store.cpp"
#include "../../structures/object_store.cpp"
#include "interpreter_store.cpp"
#include "interpreter_result.cpp"
#include "../builtin/builtin_runner.cpp"
using namespace std;

class Interpreter {
    public:
        shared_ptr<BuiltInRunner> builtin_runner = nullptr;
        map<string, function<InterpreterResult(Interpreter*, shared_ptr<Node>, shared_ptr<Context>)>> visit_functions;

        shared_ptr<Context> context = nullptr;
        map<string, shared_ptr<ObjectPrototype>> object_prototypes;
        shared_ptr<InterpreterStore> interpreter_store = nullptr;

        Interpreter() {
            interpreter_store = make_shared<InterpreterStore>();

            SymbolContainer sc_null(0);
            SymbolContainer sc_true(1);
            SymbolContainer sc_false(0);
            
            context = make_shared<Context>("unknown", "global");
            save_to_context("NULL", sc_null, context);
            save_to_context("TRUE", sc_true, context);
            save_to_context("FALSE", sc_false, context);

            builtin_runner = make_shared<BuiltInRunner>(interpreter_store);

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
            run_func = &BuiltInRunner::run_run_builtin_function; arguments.clear(); arguments.push_back("func_name"); arguments.push_back("arguments");
            add_builtin_function("run_builtin_function", arguments, run_func);

            run_func = &BuiltInRunner::run_string_char_at; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_char_at", arguments, run_func);
            run_func = &BuiltInRunner::run_string_char_code_at; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_char_code_at", arguments, run_func);
            run_func = &BuiltInRunner::run_string_concat; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_concat", arguments, run_func);
            run_func = &BuiltInRunner::run_string_ends_with; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_ends_with", arguments, run_func);
            run_func = &BuiltInRunner::run_string_starts_with; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_starts_with", arguments, run_func);
            run_func = &BuiltInRunner::run_string_includes; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_includes", arguments, run_func);
            run_func = &BuiltInRunner::run_string_index_of; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_index_of", arguments, run_func);
            run_func = &BuiltInRunner::run_string_last_index_of; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_last_index_of", arguments, run_func);
            run_func = &BuiltInRunner::run_string_replace; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_replace", arguments, run_func);
            run_func = &BuiltInRunner::run_string_replace_all; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_replace_all", arguments, run_func);
            /*run_func = &BuiltInRunner::run_string_split; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_split", arguments, run_func);*/
            run_func = &BuiltInRunner::run_string_substring; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_substring", arguments, run_func);
            run_func = &BuiltInRunner::run_string_to_lower_case; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_to_lower_case", arguments, run_func);
            run_func = &BuiltInRunner::run_string_to_upper_case; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_to_upper_case", arguments, run_func);
            run_func = &BuiltInRunner::run_string_trim; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_trim", arguments, run_func);
            run_func = &BuiltInRunner::run_string_trim_start; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_trim_start", arguments, run_func);
            run_func = &BuiltInRunner::run_string_trim_end; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_trim_end", arguments, run_func);
            run_func = &BuiltInRunner::run_string_length; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("STRING_length", arguments, run_func);

            run_func = &BuiltInRunner::run_list_concat; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("LIST_concat", arguments, run_func);
            run_func = &BuiltInRunner::run_list_push; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("LIST_push", arguments, run_func);
            run_func = &BuiltInRunner::run_list_pop; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("LIST_pop", arguments, run_func);
            run_func = &BuiltInRunner::run_list_shift; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("LIST_shift", arguments, run_func);
            /*run_func = &BuiltInRunner::run_list_includes; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("LIST_includes", arguments, run_func);*/

            run_func = &BuiltInRunner::run_http_fetch; arguments.clear(); arguments.push_back("arguments");
            add_non_root_builtin_function("http_fetch", arguments, run_func);

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
            visit_func = &Interpreter::visit_map_node;
            visit_functions.insert_or_assign(NODE_MAP, visit_func);
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

            SymbolContainer f_s(f);
            context->symbol_table->set(f->name->value_string, f_s);
        }

        void add_non_root_builtin_function(string name, list<string> arguments, function<InterpreterResult(BuiltInRunner*, InterpreterResult, shared_ptr<Function>, shared_ptr<Context>)> function) {
            shared_ptr<Function> f = builtin_runner->create_builtin_function(name, arguments, function);
            builtin_runner->add_builtin_function(name, function);
            builtin_runner->non_root_functions.insert_or_assign(name, f);
        }

        InterpreterResult visit_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            if(node->type == NODE_CONSTRUCTOR_DEF) {
                RuntimeError e(node->start, node->end, "Constructor outside of class definition", generate_traceback(_context));
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
            shared_ptr<ListStore> n_1 = interpreter_store->attach_list_store(n);

            InterpreterResult res;
            res.set_pos(n->start, n->end);
            for(shared_ptr<Node> _node : node->list_nodes_result) {
                InterpreterResult value_res = res.register_result(visit_node(_node, _context));
                if(res.should_return()) { break; }

                if(value_res.type == NODE_INT) {
                    SymbolContainer container(value_res.res_int.value);
                    n_1->add_value(container);
                } else if(value_res.type == NODE_FLOAT) {
                    SymbolContainer container(value_res.res_float.value);
                    n_1->add_value(container);
                } else if(value_res.type == NODE_STRING) {
                    SymbolContainer container(value_res.res_string.value);
                    n_1->add_value(container);
                } else if(value_res.type == NODE_LIST) {
                    SymbolContainer container(value_res.res_list);
                    n_1->add_value(container);
                } else if(value_res.type == NODE_OBJECT_NEW) {
                    SymbolContainer container(value_res.res_obj);
                    n_1->add_value(container);
                } else {
                    RuntimeError e(_node->start, _node->end, "Unsupported type in list", generate_traceback(_context));
                    res.failure(e);
                    break;
                }
            }
            if(res.should_return()) { return res; }

            res.set_from(n);
            return res.success();
        }

        InterpreterResult visit_map_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            shared_ptr<Map> n = make_shared<Map>();
            n->set_pos(node->start, node->end);
            shared_ptr<MapStore> n_1 = interpreter_store->attach_map_store(n);

            InterpreterResult res;
            res.set_pos(n->start, n->end);
            if(res.should_return()) { return res; }

            res.set_from(n);
            return res.success();
        }

        InterpreterResult visit_object_new_node(shared_ptr<Node> node, shared_ptr<Context> _context) {
            InterpreterResult res;
            res.set_pos(node->start, node->end);

            shared_ptr<Context> new_context = generate_new_context("obj_context", context);
            shared_ptr<Object> n = make_shared<Object>();
            n->set_pos(node->start, node->end);
            shared_ptr<ObjectStore> n_1 = interpreter_store->attach_object_store(n, new_context, object_prototypes.at(node->token->value_string));
            
            for(shared_ptr<Node> _node : n_1->prototype->body->statements_nodes_result) {
                if(_node->type != NODE_CONSTRUCTOR_DEF) {
                    res.register_result(visit_node(_node, n_1->context));
                }
                if(res.should_return()) { return res; }
            }
            if(n_1->prototype->constructor != nullptr) {
                res = check_args_constructor(node, _context, res, node->object_argument_nodes_result, n_1);
                if(res.should_return()) { return res; }

                res = populate_args_constructor(node, _context, res,  node->object_argument_nodes_result, n_1);
                if(res.should_return()) { return res; }

                res.register_result(visit_node(n_1->prototype->constructor->func_def_expression_result, n_1->context));
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
                    RuntimeError e(_node->start, _node->end, "Expected variable assignment, function definition or constructor", generate_traceback(_context));
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

                res = res.process_binary(right, n_t, n_m_i, _context);
            } else if(node->token->matches(TT_KEYWORD, KEYWORD_NOT)) {
                shared_ptr<Token> n_t = make_shared<Token>(TT_EQEQ);
                InterpreterResult n_m_i;
                n_m_i.set_from(0);

                res = res.process_binary(right, n_t, n_m_i, _context);
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

            res = res.process_binary(left_res, node->token, right_res, _context);
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
                } else if(value.type == SYMBOL_LIST) {
                    res.set_from(value.value_list);
                } else if(value.type == SYMBOL_OBJECT) {
                    res.set_from(value.value_object);
                }
            } else {
                RuntimeError e(node->start, node->end, node->token->value_string + " is not defined", generate_traceback(_context));
                return res.failure(e);
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
            new_value = res.register_result(res.process_binary(previous_value, _token, value_res, _context));

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
                save_to_context(node->token->value_string, container, _context);
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
                RuntimeError e(node->start, node->end, "Foreach must contain a valid list", generate_traceback(_context));
                return res.failure(e);
            }

            shared_ptr<ListStore> list_store = interpreter_store->get_list_store(list.res_list->list_id);
            for(SymbolContainer e : list_store->list_symbols) {
                save_to_context(node->token->value_string, e, _context);

                InterpreterResult expr = res.register_result(visit_node(node->for_expr_result, _context));

                if(res.state == -1) { break; }
                if(res.loop_should_continue == true) { res.reset(); continue; }
                if(res.loop_should_break == true) { res.reset(); break; }
                if(res.has_return_value == true) { res.set_from(expr); res.reset(); break; }
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

            SymbolContainer function = _context->get_variable(node->token->value_string);
            if(function.state == -1) {
                RuntimeError e(node->start, node->end, "Function '" + node->token->value_string + "' does not exist", generate_traceback(_context));
                return res.failure(e);
            }

            res = check_args(node, new_context, res, arguments, function.value_function);
            if(res.should_return()) { return res; }

            res = populate_args(node, new_context, res, arguments, function.value_function);
            if(res.should_return()) { return res; }

            InterpreterResult expr;
            if(function.value_function->built_in == false) {
                expr = res.register_result(visit_node(function.value_function->expression, new_context));
            } else if(function.value_function->name->value_string == "run_builtin_function") {
                shared_ptr<ListStore> arguments = interpreter_store->get_list_store(new_context->get_variable("arguments").value_list->list_id);
                builtin_runner->non_root_arguments = arguments;

                expr = res.register_result(builtin_runner->run(function.value_function, new_context));
            } else {
                expr = res.register_result(builtin_runner->run(function.value_function, new_context));
            }
            if(res.should_return() && res.has_return_value == false) { return res; }

            res.set_from(expr);
            return res.success();
        }

        shared_ptr<Context> generate_new_context(string name, shared_ptr<Context> _context) {
            shared_ptr<Context> new_context = make_shared<Context>(_context->file_name, name);
            new_context->set_parent(_context);
            new_context->set_symbol_table(_context->symbol_table);

            return new_context;
        }
        
        InterpreterResult check_args(shared_ptr<Node> node, shared_ptr<Context> _context, InterpreterResult res, list<shared_ptr<Node>> arguments, shared_ptr<Function> function) {
            if(arguments.size() > function->arguments.size()) {
                RuntimeError e(node->start, node->end, "Too many args passed into '" + node->token->value_string + "' (Got: " + to_string(arguments.size()) + "/" + to_string(function->arguments.size()) + ")", generate_traceback(_context));
                res.failure(e);
            } else if(arguments.size() < function->arguments.size()) {
                RuntimeError e(node->start, node->end, "Not enough args passed into '" + node->token->value_string + "' (Got: " + to_string(arguments.size()) + "/" + to_string(function->arguments.size()) + ")", generate_traceback(_context));
                res.failure(e);
            }

            return res;
        }
        
        InterpreterResult check_args_constructor(shared_ptr<Node> node, shared_ptr<Context> _context, InterpreterResult res, list<shared_ptr<Node>> arguments, shared_ptr<ObjectStore> object) {
            if(arguments.size() > object->prototype->constructor->func_def_argument_tokens_result.size()) {
                RuntimeError e(node->start, node->end, "Too many args passed into '" + node->token->value_string + "' (Got: " + to_string(arguments.size()) + "/" + to_string(object->prototype->constructor->func_def_argument_tokens_result.size()) + ")", generate_traceback(_context));
                res.failure(e);
            } else if(arguments.size() < object->prototype->constructor->func_def_argument_tokens_result.size()) {
                RuntimeError e(node->start, node->end, "Not enough args passed into '" + node->token->value_string + "' (Got: " + to_string(arguments.size()) + "/" + to_string(object->prototype->constructor->func_def_argument_tokens_result.size()) + ")", generate_traceback(_context));
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

        InterpreterResult populate_args_constructor(shared_ptr<Node> node, shared_ptr<Context> _context, InterpreterResult res, list<shared_ptr<Node>> arguments, shared_ptr<ObjectStore> object) {
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
                SymbolContainer value(res.res_func);
                _context->symbol_table->set(name, value);
            } else if(res.type == NODE_STRING) {
                SymbolContainer value(res.res_string.value);
                _context->symbol_table->set(name, value);
            } else if(res.type == NODE_LIST) {
                SymbolContainer value(res.res_list);
                _context->symbol_table->set(name, value);
            } else if(res.type == NODE_OBJECT_NEW) {
                SymbolContainer value(res.res_obj);
                _context->symbol_table->set(name, value);
            }
        }

        void save_to_context(string name, SymbolContainer value, shared_ptr<Context> _context) {
            _context->symbol_table->set(name, value);
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
            if(left.type != NODE_OBJECT_NEW && node->right->type == NODE_FUNC_CALL) {
                string func_name = node->right->func_call_expression_result->token->value_string;
                node->right->func_call_expression_result->token->value_string = "run_builtin_function";
                
                list<shared_ptr<Node>> arguments;
                list<shared_ptr<Node>> list_elements;

                shared_ptr<Node> func_name_node = make_shared<Node>();
                func_name_node->set_value(left.type + "_" + func_name);

                shared_ptr<Node> list_node = make_shared<Node>();
                list_node->set_type(NODE_LIST);
                list_elements.push_back(node->left);
                for(shared_ptr<Node> _node : node->right->func_call_argument_nodes_result) {
                    list_elements.push_back(_node);
                }
                list_node->set_list_nodes_result(list_elements);

                arguments.push_back(func_name_node);
                arguments.push_back(list_node);
                node->right->set_func_call_argument_nodes_result(arguments);

                InterpreterResult right = res.register_result(visit_node(node->right, _context));
                if(res.should_return()) { return res; }

                res.set_from(right);
                return res.success();
            }
            if(left.type != NODE_OBJECT_NEW) {
                RuntimeError e(node->start, node->end, "Expected object", generate_traceback(_context));
                return res.failure(e);
            }

            shared_ptr<ObjectStore> object_store = interpreter_store->get_object_store(left.res_obj->object_id);
            InterpreterResult right = res.register_result(visit_node(node->right, object_store->context));
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
                RuntimeError e(node->start, node->end, "Expected object", generate_traceback(_context));
                return res.failure(e);
            }

            InterpreterResult expr = res.register_result(visit_node(node->chained_assigment_result, _context));
            if(res.should_return()) { return res; }

            shared_ptr<ObjectStore> object_store = interpreter_store->get_object_store(left.res_obj->object_id);
            save_to_context(node->right->token->value_string, expr, object_store->context);
            
            return res.success();
        }

        void no_visit_method(string type) {
            //printf("No visit method for %s\n", type);
        }
};
