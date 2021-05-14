#pragma once
#include <string>
#include <stdexcept>
#include "node.cpp"
#include "../structures/number.cpp"
using namespace std;

class InterpreterResult {
    public:
        string type = NODE_UNKNOWN;
        IntNumber res_int;
        FloatNumber res_float;

        void init(IntNumber _res_int) {
            res_int = _res_int;
            type = NODE_INT;
        }

        void init(FloatNumber _res_float) {
            res_float = _res_float;
            type = NODE_FLOAT;
        }
};

class Interpreter {
    public:
        void init() {
            
        }

        IntNumber visit_int_node(NumberNode node) {
            //printf("Found int node-\n");
            IntNumber n;
            n.init(node.token_int);
            n.set_pos(node.start, node.end);

            return n;
        }

        FloatNumber visit_float_node(NumberNode node) {
            //printf("Found float node-\n");
            FloatNumber n;
            n.init(node.token_float);
            n.set_pos(node.start, node.end);

            return n;
        }

        InterpreterResult visit_binary_node(BinaryOperationNode node) {
            //printf("Found binary node-\n");
            string left_type = node.left_type;
            IntNumber left_int;
            FloatNumber left_float;

            string right_type = node.right_type;
            IntNumber right_int;
            FloatNumber right_float;

            //printf("%s\n", (left_type + "<>" + right_type).c_str());

            if(left_type == NODE_INT) {
                left_int = visit_int_node(node.left);
            } else if(left_type == NODE_FLOAT) {
                left_float = visit_float_node(node.left);
            } else if(left_type == NODE_BINARY) {
                InterpreterResult res = visit_binary_node(*node.left_binary);
                if(res.type == NODE_INT) { left_int = res.res_int; left_type = NODE_INT; } else { left_float = res.res_float; left_type = NODE_FLOAT; }
            }

            if(right_type == NODE_INT) {
                right_int = visit_int_node(node.right);
            } else if(right_type == NODE_FLOAT) {
                right_float = visit_float_node(node.right);
            } else if(right_type == NODE_BINARY) {
                InterpreterResult res = visit_binary_node(*node.right_binary);
                if(res.type == NODE_INT) { right_int = res.res_int; right_type = NODE_INT; } else { right_float = res.res_float; right_type = NODE_FLOAT; }
            }

            InterpreterResult res;
            if(node.op.type == TT_PLUS) {
                if(left_type == NODE_INT && right_type == NODE_INT) {
                    IntNumber n = left_int.added_to(right_int); /*n.set_pos(node.start, node.end);*/ res.init(n);
                } else if(right_type == NODE_INT && left_type == NODE_FLOAT) {
                    FloatNumber n_0; n_0.init(right_int.value); FloatNumber n = left_float.added_to(n_0); /*n.set_pos(node.start, node.end);*/ res.init(n);
                } else if(right_type == NODE_FLOAT && left_type == NODE_INT) {
                    FloatNumber n = left_int.added_to(right_float); /*n.set_pos(node.start, node.end);*/ res.init(n);
                } else {
                    FloatNumber n = left_float.added_to(right_float); /*n.set_pos(node.start, node.end);*/ res.init(n);
                }
            } else if(node.op.type == TT_MINUS) {
                if(left_type == NODE_INT && right_type == NODE_INT) {
                    IntNumber n = left_int.substracted_by(right_int); /*n.set_pos(node.start, node.end);*/ res.init(n);
                } else if(right_type == NODE_INT && left_type == NODE_FLOAT) {
                    FloatNumber n_0; n_0.init(right_int.value); FloatNumber n = left_float.substracted_by(n_0); /*n.set_pos(node.start, node.end);*/ res.init(n);
                } else if(right_type == NODE_FLOAT && left_type == NODE_INT) {
                    FloatNumber n = left_int.substracted_by(right_float); /*n.set_pos(node.start, node.end);*/ res.init(n);
                } else {
                    FloatNumber n = left_float.substracted_by(right_float); /*n.set_pos(node.start, node.end);*/ res.init(n);
                }
            } else if(node.op.type == TT_MUL) {
                if(left_type == NODE_INT && right_type == NODE_INT) {
                    IntNumber n = left_int.multiplied_by(right_int); /*n.set_pos(node.start, node.end);*/ res.init(n);
                } else if(right_type == NODE_INT && left_type == NODE_FLOAT) {
                    FloatNumber n_0; n_0.init(right_int.value); FloatNumber n = left_float.multiplied_by(n_0); /*n.set_pos(node.start, node.end);*/ res.init(n);
                } else if(right_type == NODE_FLOAT && left_type == NODE_INT) {
                    FloatNumber n = left_int.multiplied_by(right_float); /*n.set_pos(node.start, node.end);*/ res.init(n);
                } else {
                    FloatNumber n = left_float.multiplied_by(right_float); /*n.set_pos(node.start, node.end);*/ res.init(n);
                }
            } else if(node.op.type == TT_DIV) {
                if(left_type == NODE_INT && right_type == NODE_INT) {
                    IntNumber n = left_int.divided_by(right_int); /*n.set_pos(node.start, node.end);*/ res.init(n);
                } else if(right_type == NODE_INT && left_type == NODE_FLOAT) {
                    FloatNumber n_0; n_0.init(right_int.value); FloatNumber n = left_float.divided_by(n_0); /*n.set_pos(node.start, node.end);*/ res.init(n);
                } else if(right_type == NODE_FLOAT && left_type == NODE_INT) {
                    FloatNumber n = left_int.divided_by(right_float); /*n.set_pos(node.start, node.end);*/ res.init(n);
                } else {
                    FloatNumber n = left_float.divided_by(right_float); /*n.set_pos(node.start, node.end);*/ res.init(n);
                }
            }
            
            //printf("returning: %s\n", (res.type == NODE_INT ? to_string(res.res_int.value).c_str() : to_string(res.res_float.value).c_str()));
            return res;
        }

        InterpreterResult visit_unary_node(UnaryOperationNode node) {
            //printf("Found unary node-\n");

            InterpreterResult res;
            if(node.op.type == TT_PLUS) {
                if(node.node.type == NODE_INT) {
                    IntNumber n = visit_int_node(node.node); /*n.set_pos(node.start, node.end);*/ res.init(n);
                } else {
                    FloatNumber n = visit_float_node(node.node); /*n.set_pos(node.start, node.end);*/ res.init(n);
                }
            } else if(node.op.type == TT_MINUS) {
                IntNumber n_m;
                n_m.init(-1);
            
                if(node.node.type == NODE_INT) {
                    IntNumber n = n_m.multiplied_by(visit_int_node(node.node)); /*n.set_pos(node.start, node.end);*/ res.init(n);
                } else {
                    FloatNumber n = n_m.multiplied_by(visit_float_node(node.node)); /*n.set_pos(node.start, node.end);*/ res.init(n);
                }
            }

            return res;
        }

        void no_visit_method(string type) {
            //printf("No visit method for %s\n", type);
        }
};