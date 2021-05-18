#pragma once
#include <string>
using namespace std;

class Interpreter {
    public:
        void init() {
            
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

        InterpreterResult visit_unary_node(Node* node, Context* context) {
            InterpreterResult res;
            res.init(node->start, node->end);

            if(node->token->type == TT_PLUS) {
                res.set_from(visit_node(node->left, context));
            } else if(node->token->type == TT_MINUS) {
                Token* n_t = new Token();
                n_t->init(TT_MUL);
                IntNumber n_m;
                n_m.init(-1);
                InterpreterResult n_m_i;
                n_m_i.set_from(n_m);

                res = res.processNumber(visit_node(node->left, context), n_t, n_m_i);
            } else if(node->token->matches(TT_KEYWORD, KEYWORD_NOT)) {
                Token* n_t = new Token();
                n_t->init(TT_EQEQ);
                IntNumber n_m;
                n_m.init(0);
                InterpreterResult n_m_i;
                n_m_i.set_from(n_m);

                res = res.processNumber(visit_node(node->left, context), n_t, n_m_i);
            }

            return res.success();
        }

        InterpreterResult visit_binary_node(Node* node, Context* context) {
            InterpreterResult res;
            res.init(node->start, node->end);
            
            InterpreterResult left_res;
            InterpreterResult right_res;

            //printf("%s\n", (left_type + "<>" + right_type).c_str());

            left_res = visit_node(node->left, context);
            res.registerResult(left_res);
            if(res.state == -1) { return res; }

            right_res = visit_node(node->right, context);
            res.registerResult(right_res);
            if(res.state == -1) { return res; }

            res = res.processNumber(left_res, node->token, right_res);
            if(res.state == -1) { return res; }
            
            //printf("returning: %s\n", (res.type == NODE_INT ? to_string(res.res_int.value).c_str() : to_string(res.res_float.value).c_str()));
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
                    IntNumber n;
                    n.init(value.value_int);
                    res.set_from(n);
                } else  if(value.type == SYMBOL_FLOAT) {
                    FloatNumber n;
                    n.init(value.value_float);
                    res.set_from(n);
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

            if(res.type == NODE_INT) {
                SymbolContainer* value = new SymbolContainer();
                value->init(res.res_int.value);
                context->symbol_table->set(node->token->value_string, value);
            } else if(res.type == NODE_FLOAT) {
                SymbolContainer* value = new SymbolContainer();
                value->init(res.res_float.value);
                context->symbol_table->set(node->token->value_string, value);
            }
            
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

                    res = expr_res;
                    break;
                }

                cond = nullptr;
            }
            if(res.type == NODE_UNKNOWN && node->else_result != nullptr) {
                InterpreterResult else_res = res.registerResult(visit_node(node->else_result, context));
                if(res.state == -1) { return res; }

                res = else_res;
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
                SymbolContainer* container;
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

            return res.success();
        }

        void no_visit_method(string type) {
            //printf("No visit method for %s\n", type);
        }
};