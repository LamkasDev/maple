#pragma once
#include <string>
#include <map>
#include "../builtin/builtin_runner.cpp"
using namespace std;

class Interpreter {
    public:
        BuiltInRunner* builtin_runner = nullptr;
        SymbolTable* global_symbol_table = nullptr;
        map<string, Function> functions;

        void init() {
            SymbolContainer* sc_null = new SymbolContainer();
            sc_null->init(0);
            SymbolContainer* sc_true = new SymbolContainer();
            sc_true->init(1);
            SymbolContainer* sc_false = new SymbolContainer();
            sc_false->init(0);

            global_symbol_table = new SymbolTable();
            global_symbol_table->set("NULL", sc_null);
            global_symbol_table->set("TRUE", sc_true);
            global_symbol_table->set("FALSE", sc_false);

            builtin_runner = new BuiltInRunner();
            add_builtin_function("print", "value");
            add_builtin_function("input");
            add_builtin_function("is_nan", "value");
            add_builtin_function("parse_int", "value");
            add_builtin_function("parse_float", "value");
        }

        void add_builtin_function(string name) {
            list<string> arguments;
            Function f = builtin_runner->create_builtin_function(name, arguments);
            functions[f.name->value_string] = f;
        }

        void add_builtin_function(string name, string arg_1) {
            list<string> arguments; arguments.push_back(arg_1);
            Function f = builtin_runner->create_builtin_function(name, arguments);
            functions[f.name->value_string] = f;
        }

        InterpreterResult visit_node(Node* node, Context* context) {
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
            } else if(node->type == NODE_WHILE) {
                res = visit_while_node(node, context);
            } else if(node->type == NODE_FUNC_CALL) {
                res = visit_func_call_node(node, context);
            } else if(node->type == NODE_FUNC_DEF) {
                res = visit_func_def_node(node, context);
            } else if(node->type == NODE_STRING) {
                res = visit_string_node(node, context);
            } else if(node->type == NODE_STATEMENTS) {
                res = visit_statements_node(node, context);
            } else {
                no_visit_method(node->type);
            }

            return res;
        }

        InterpreterResult visit_int_node(Node* node, Context* context) {
            IntNumber n;
            n.init(node->value->value_int);
            n.set_pos(node->start, node->end);
            n.set_context(context);

            InterpreterResult res;
            res.init(n.start, n.end);
            res.set_from(n);
            n.set_context(context);

            return res.success();
        }

        InterpreterResult visit_float_node(Node* node, Context* context) {
            FloatNumber n;
            n.init(node->value->value_float);
            n.set_pos(node->start, node->end);

            InterpreterResult res;
            res.init(n.start, n.end);
            res.set_from(n);

            return res.success();
        }

        InterpreterResult visit_string_node(Node* node, Context* context) {
            String n;
            n.init(node->value->value_string);
            n.set_pos(node->start, node->end);

            InterpreterResult res;
            res.init(n.start, n.end);
            res.set_from(n);

            return res.success();
        }

        InterpreterResult visit_unary_node(Node* node, Context* context) {
            InterpreterResult res;
            res.init(node->start, node->end);

            if(node->token->type == TT_PLUS) {
                res.set_from(visit_node(node->left, context));
            } else if(node->token->type == TT_MINUS) {
                Token* n_t = new Token();
                n_t->init(TT_MUL);
                InterpreterResult n_m_i;
                n_m_i.set_from(-1);

                res = res.process_binary(visit_node(node->left, context), n_t, n_m_i);
            } else if(node->token->matches(TT_KEYWORD, KEYWORD_NOT)) {
                Token* n_t = new Token();
                n_t->init(TT_EQEQ);
                InterpreterResult n_m_i;
                n_m_i.set_from(0);

                res = res.process_binary(visit_node(node->left, context), n_t, n_m_i);
            }

            return res.success();
        }

        InterpreterResult visit_binary_node(Node* node, Context* context) {
            InterpreterResult res;
            res.init(node->start, node->end);
            
            InterpreterResult left_res;
            InterpreterResult right_res;

            left_res = visit_node(node->left, context);
            res.registerResult(left_res);
            if(res.state == -1) { return res; }

            right_res = visit_node(node->right, context);
            res.registerResult(right_res);
            if(res.state == -1) { return res; }

            res = res.process_binary(left_res, node->token, right_res);
            if(res.state == -1) { return res; }

            return res.success();
        }

        InterpreterResult visit_variable_access_node(Node* node, Context* context) {
            InterpreterResult res;
            res.init(node->start, node->end);

            SymbolContainer value = (*context->symbol_table->get(node->token->value_string));
            if(value.state == -1) {
                RuntimeError e;
                e.init(node->start, node->end, node->token->value_string + " is not defined", context);
                return res.failure(e);
            } else {
                if(value.type == SYMBOL_INT) {
                    res.set_from(value.value_int);
                } else if(value.type == SYMBOL_FLOAT) {
                    res.set_from(value.value_float);
                } else if(value.type == SYMBOL_STRING) {
                    res.set_from(value.value_string);
                }
            }

            return res.success();
        }

        InterpreterResult visit_variable_assign(Node* node, Context* context) {
            InterpreterResult res;
            res.init(node->start, node->end);
            
            InterpreterResult value_res;
            value_res = visit_node(node->right, context);
            if(res.state == -1) { return res; }
            res.set_from(value_res);

            save_to_context(node->token->value_string, res, context);
            return res.success();
        }

        InterpreterResult visit_if_node(Node* node, Context* context) {
            InterpreterResult res;
            InterpreterResult cond_res;
            InterpreterResult expr_res;
            res.init(node->start, node->end);
            
            Node* cond = nullptr;
            for(Node* node : node->if_results) {
                if(cond == nullptr) {
                    cond = node;
                    continue;
                }

                cond_res = res.registerResult(visit_node(cond, context));
                if(res.state == -1) { return res; }
                if(cond_res.is_true()) {
                    expr_res = res.registerResult(visit_node(node, context));
                    if(res.state == -1) { break; }

                    res.set_from(expr_res);
                    break;
                }

                cond = nullptr;
            }
            if(res.type == NODE_UNKNOWN && node->else_result != nullptr) {
                InterpreterResult else_res = res.registerResult(visit_node(node->else_result, context));
                if(res.state == -1) { return res; }
                res.set_from(else_res);

                return res.success();
            }
            if(cond_res.state == -1 || expr_res.state == -1) { return res; }

            return res.success();
        }

        InterpreterResult visit_for_node(Node* node, Context* context) {
            InterpreterResult res;
            res.init(node->start, node->end);

            InterpreterResult start_value = res.registerResult(visit_node(node->for_start_result, context));
            if(res.state == -1) { return res; }
            InterpreterResult end_value = res.registerResult(visit_node(node->for_end_result, context));
            if(res.state == -1) { return res; }

            InterpreterResult step_value;
            if(node->for_step_result != nullptr) {
                step_value = res.registerResult(visit_node(node->for_step_result, context));
                if(res.state == -1) { return res; }
            } else {
                IntNumber n_s;
                n_s.init(1);
                InterpreterResult n_s_i;
                n_s_i.set_from(n_s);

                step_value = n_s_i;
            }
            
            bool st_dir = step_value.get_value() >= 0 ? true : false;
            int i = start_value.get_value();
            while((st_dir == true && i < end_value.get_value()) || (st_dir == false && i > end_value.get_value())) {
                SymbolContainer* container = new SymbolContainer();
                container->init(i);

                context->symbol_table->set(node->token->value_string, container);
                i += step_value.get_value();

                res.registerResult(visit_node(node->for_expr_result, context));
                if(res.state == -1) { break; }
            }
            if(res.state == -1) { return res; }

            return res.success();
        }

        InterpreterResult visit_while_node(Node* node, Context* context) {
            InterpreterResult res;
            res.init(node->start, node->end);

            while(true) {
                InterpreterResult condition = res.registerResult(visit_node(node->while_condition_result, context));
                if(res.state == -1 || condition.is_true() == false) { break; }

                InterpreterResult expr = res.registerResult(visit_node(node->while_expr_result, context));
                if(res.state == -1) { break; }
            }
            if(res.state == -1) { return res; }

            return res.success();
        }

        InterpreterResult visit_func_def_node(Node* node, Context* context) {
            InterpreterResult res;
            res.init(node->start, node->end);

            Function function;
            function.set_pos(node->start, node->end);
            function.set_context(context);
            function.set_name(node->token);
            function.set_arguments(node->func_def_argument_tokens_result);
            function.set_expression(node->func_def_expression_result);
            res.set_from(function);

            if(node->token != nullptr) {
                save_to_context(node->token->value_string, res, context);
            }

            return res.success();
        }

        InterpreterResult visit_func_call_node(Node* node, Context* context) {
            InterpreterResult res;
            res.init(node->start, node->end);

            InterpreterResult expr = res.registerResult(execute(node->func_call_expression_result, node->func_call_argument_nodes_result, context));
            if(res.state == -1) { return res; }

            res.set_from(expr);
            return res.success();
        }

        InterpreterResult execute(Node* node, list<Node*> arguments, Context* context) {
            InterpreterResult res;
            res.init(node->start, node->end);

            Interpreter* interpreter = new Interpreter();
            interpreter->init();
            Context* new_context = generate_new_context(node, context);

            string func_name = node->token->value_string;
            try {
                Function function = functions.at(func_name);
                check_args(node, new_context, res, arguments, function);
                if(res.state == -1) { return res; }

                populate_args(node, new_context, res, arguments, function);
                if(res.state == -1) { return res; }

                InterpreterResult expr;
                if(function.built_in == false) {
                    expr = res.registerResult(visit_node(function.expression, new_context));
                } else {
                    expr = res.registerResult(builtin_runner->run(function, new_context));
                }
                if(res.state == -1) { return res; }

                res.set_from(expr);
                return res.success();
            } catch(out_of_range e_0) {
                RuntimeError e;
                e.init(node->start, node->end, "Function '" + func_name + "' does not exist", context);
                return res.failure(e);
            }
        }

        Context* generate_new_context(Node* node, Context* context) {
            Context* new_context = new Context();
            new_context->init(node->token->value_string);
            new_context->set_parent(context);
            new_context->set_parent_entry_pos(node->start);
            new_context->set_symbol_table(new_context->parent->symbol_table);

            return new_context;
        }
        
        void check_args(Node* node, Context* context, InterpreterResult res, list<Node*> arguments, Function function) {
            if(arguments.size() > function.arguments.size()) {
                RuntimeError e;
                e.init(node->start, node->end, ((arguments.size() - function.arguments.size()) + " too many args passed into " + node->token->value_string), context);
                res.failure(e);
            } else if(arguments.size() < function.arguments.size()) {
                RuntimeError e;
                e.init(node->start, node->end, ((function.arguments.size() - arguments.size()) + " too few args passed into " + node->token->value_string), context);
                res.failure(e);
            }
        }

        void populate_args(Node* node, Context* context, InterpreterResult res, list<Node*> arguments, Function function) {
            for(Node* arg : arguments) {
                Token* token = function.arguments.back();
                InterpreterResult arg_res = res.registerResult(visit_node(arg, context));
                if(arg_res.state == -1) { break; }

                save_to_context(token->value_string, arg_res, context);
                function.arguments.pop_back();
            }
        }

        void save_to_context(string name, InterpreterResult res, Context* context) {
            if(res.type == NODE_INT) {
                SymbolContainer* value = new SymbolContainer();
                value->init(res.res_int.value);
                context->symbol_table->set(name, value);
            } else if(res.type == NODE_FLOAT) {
                SymbolContainer* value = new SymbolContainer();
                value->init(res.res_float.value);
                context->symbol_table->set(name, value);
            } else if(res.type == NODE_FUNC_DEF) {
                functions[name] = res.res_func;
            } else if(res.type == NODE_STRING) {
                SymbolContainer* value = new SymbolContainer();
                value->init(res.res_string.value);
                context->symbol_table->set(name, value);
            }
        }

        InterpreterResult visit_statements_node(Node* node, Context* context) {
            InterpreterResult res;
            res.init(node->start, node->end);

            for(Node* _node : node->statements_nodes_result) {
                res.set_from(visit_node(_node, context));
            }

            return res.success();
        }

        void no_visit_method(string type) {
            //printf("No visit method for %s\n", type);
        }
};