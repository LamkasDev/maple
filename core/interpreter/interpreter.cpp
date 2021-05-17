#pragma once
#include <string>
using namespace std;

class Interpreter {
    public:
        void init() {
            
        }

        InterpreterResult visit_int_node(Node* node, Context* context) {
            //printf("Found int node-\n");
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
            //printf("Found float node-\n");
            FloatNumber n;
            n.init(node->value->value_float);
            n.set_pos(node->start, node->end);

            InterpreterResult res;
            res.init(n.start, n.end);
            res.set_from(n);

            return res.success();
        }

        InterpreterResult visit_unary_node(Node* node, Context* context) {
            //printf("Found unary node-\n");

            InterpreterResult res;
            res.init(node->start, node->end);

            if(node->token->type == TT_PLUS) {
                if(node->left->type == NODE_INT) {
                    res.set_from(visit_int_node(node->left, context));
                } else {
                    res.set_from(visit_float_node(node->left, context));
                }
            } else if(node->token->type == TT_MINUS) {
                Token* n_t = new Token();
                n_t->init(TT_MUL);
                IntNumber n_m;
                n_m.init(-1);
                InterpreterResult n_m_i;
                n_m_i.set_from(n_m);
                
                if(node->left->type == NODE_INT) {
                    res = res.processNumber(visit_int_node(node->left, context), n_t, n_m_i);
                } else {
                    res = res.processNumber(visit_float_node(node->left, context), n_t, n_m_i);
                }
            }

            return res.success();
        }

        InterpreterResult visit_binary_node(Node* node, Context* context) {
            //printf("Found binary node-\n");

            InterpreterResult res;
            res.init(node->start, node->end);
            
            InterpreterResult left_res;
            InterpreterResult right_res;

            //printf("%s\n", (left_type + "<>" + right_type).c_str());

            if(node->left->type == NODE_INT) {
                left_res = visit_int_node(node->left, context);
            } else if(node->left->type == NODE_FLOAT) {
                left_res = visit_float_node(node->left, context);
            } else if(node->left->type == NODE_UNARY) {
                left_res = visit_unary_node(node->left, context);
            } else if(node->left->type == NODE_BINARY) {
                left_res = visit_binary_node(node->left, context);
            } else if(node->left->type == NODE_ACCESS) {
                left_res = visit_variable_access(node->left, context);
            }
            res.registerResult(left_res);
            if(res.state == -1) { return res; }

            if(node->right->type == NODE_INT) {
                right_res = visit_int_node(node->right, context);
            } else if(node->right->type == NODE_FLOAT) {
                right_res = visit_float_node(node->right, context);
            } else if(node->right->type == NODE_UNARY) {
                right_res = visit_unary_node(node->right, context);
            } else if(node->right->type == NODE_BINARY) {
                right_res = visit_binary_node(node->right, context);
            } else if(node->right->type == NODE_ACCESS) {
                right_res = visit_variable_access(node->right, context);
            }
            res.registerResult(right_res);
            if(res.state == -1) { return res; }

            res = res.processNumber(left_res, node->token, right_res);
            if(res.state == -1) { return res; }
            
            //printf("returning: %s\n", (res.type == NODE_INT ? to_string(res.res_int.value).c_str() : to_string(res.res_float.value).c_str()));
            return res.success();
        }

        InterpreterResult visit_variable_access(Node* node, Context* context) {
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
            if(node->right->type == NODE_INT) {
                value_res = res.registerResult(visit_int_node(node->right, context));
            } else if(node->right->type == NODE_FLOAT) {
                value_res = res.registerResult(visit_float_node(node->right, context));
            } else if(node->right->type == NODE_UNARY) {
                value_res = res.registerResult(visit_unary_node(node->right, context));
            } else if(node->right->type == NODE_BINARY) {
                value_res = res.registerResult(visit_binary_node(node->right, context));
            } else if(node->right->type == NODE_ACCESS) {
                value_res = res.registerResult(visit_variable_access(node->right, context));
            }
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

        void no_visit_method(string type) {
            //printf("No visit method for %s\n", type);
        }
};