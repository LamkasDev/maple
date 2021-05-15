#pragma once
#include <string>
#include "interpreter_result.cpp"
using namespace std;

class Interpreter {
    public:
        void init() {
            
        }

        InterpreterResult visit_int_node(NumberNode node, Context* context) {
            //printf("Found int node-\n");
            IntNumber n;
            n.init(node.token_int);
            n.set_pos(node.start, node.end);
            n.set_context(context);

            InterpreterResult res;
            res.init(n.start, n.end);
            res.set_from(n);
            n.set_context(context);

            return res.success();
        }

        InterpreterResult visit_float_node(NumberNode node, Context* context) {
            //printf("Found float node-\n");
            FloatNumber n;
            n.init(node.token_float);
            n.set_pos(node.start, node.end);

            InterpreterResult res;
            res.init(n.start, n.end);
            res.set_from(n);

            return res.success();
        }

        InterpreterResult visit_unary_node(UnaryOperationNode node, Context* context) {
            //printf("Found unary node-\n");

            InterpreterResult res;
            res.init(node.start, node.end);

            if(node.op.type == TT_PLUS) {
                if(node.node.type == NODE_INT) {
                    res.set_from(visit_int_node(node.node, context));
                } else {
                    res.set_from(visit_float_node(node.node, context));
                }
            } else if(node.op.type == TT_MINUS) {
                Token n_t;
                n_t.init(TT_MUL);
                IntNumber n_m;
                n_m.init(-1);
                InterpreterResult n_m_i;
                n_m_i.set_from(n_m);
                
                if(node.node.type == NODE_INT) {
                    res = res.processNumber(visit_int_node(node.node, context), n_t, n_m_i);
                } else {
                    res = res.processNumber(visit_float_node(node.node, context), n_t, n_m_i);
                }
            }

            return res.success();
        }

        InterpreterResult visit_binary_node(BinaryOperationNode* node, Context* context) {
            //printf("Found binary node-\n");

            InterpreterResult res;
            res.init((*node).start, (*node).end);

            string left_type = (*node).left_type;
            InterpreterResult left_res;

            string right_type = (*node).right_type;
            InterpreterResult right_res;

            //printf("%s\n", (left_type + "<>" + right_type).c_str());

            if(left_type == NODE_INT) {
                left_res = visit_int_node((*node).left, context);
            } else if(left_type == NODE_FLOAT) {
                left_res = visit_float_node((*node).left, context);
            } else if(left_type == NODE_UNARY) {
                left_res = visit_unary_node((*node).left_unary, context);
            } else if(left_type == NODE_BINARY) {
                left_res = visit_binary_node((*node).left_binary, context);
            }
            res.registerResult(left_res);
            if(res.state == -1) { return res; }

            if(right_type == NODE_INT) {
                right_res = visit_int_node((*node).right, context);
            } else if(right_type == NODE_FLOAT) {
                right_res = visit_float_node((*node).right, context);
            } else if(right_type == NODE_UNARY) {
                right_res = visit_unary_node((*node).right_unary, context);
            } else if(right_type == NODE_BINARY) {
                right_res = visit_binary_node((*node).right_binary, context);
            }
            res.registerResult(right_res);
            if(res.state == -1) { return res; }

            res = res.processNumber(left_res, (*node).op, right_res);
            if(res.state == -1) { return res; }
            
            //printf("returning: %s\n", (res.type == NODE_INT ? to_string(res.res_int.value).c_str() : to_string(res.res_float.value).c_str()));
            return res.success();
        }

        void no_visit_method(string type) {
            //printf("No visit method for %s\n", type);
        }
};