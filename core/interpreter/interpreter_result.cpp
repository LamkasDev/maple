#pragma once
#include <string>
#include "../../structures/number.cpp"
using namespace std;

class InterpreterResult {
    public:
        int state = 0;
        Error e;
        Position start;
        Position end;

        string type = NODE_UNKNOWN;
        IntNumber res_int;
        FloatNumber res_float;

        void init(Position _start, Position _end) {
            start = _start;
            end = _end;
        }

        InterpreterResult registerResult(InterpreterResult _result) {
            state = _result.state;
            e = _result.e;

            return _result;
        }

        InterpreterResult success() {
            state = 0;
            return *this;
        }

        InterpreterResult failure(Error _e) {
            state = -1;
            e = _e;
            return *this;
        }

        void set_from(IntNumber _res_int) {
            res_int = _res_int;
            type = NODE_INT;
        }

        void set_from(FloatNumber _res_float) {
            res_float = _res_float;
            type = NODE_FLOAT;
        }

        void set_from(InterpreterResult _res) {
            if(_res.type == NODE_INT) {
                res_int = _res.res_int;
            } else {
                res_float = _res.res_float;
            }
        }

        InterpreterResult processNumber(InterpreterResult _left, Token _op, InterpreterResult _right) {
            if(_op.type == TT_PLUS) {
                return _left.added_to(_right);
            } else if(_op.type == TT_MINUS) {
                return _left.substracted_by(_right);
            } else if(_op.type == TT_MUL) {
                return _left.multiplied_by(_right);
            } else if(_op.type == TT_DIV) {
                return _left.divided_by(_right);
            } else if(_op.type == TT_POW) {
                return _left.power_on(_right);
            } else {
                return _left.modulo_of(_right);
            } 
        }

        InterpreterResult added_to(InterpreterResult _right) {
            InterpreterResult res;
            IntNumber n_int;
            FloatNumber n_float;
            if(type == NODE_INT && _right.type == NODE_INT) {
                n_int.init(res_int.value + _right.res_int.value);
                n_int.set_context(res_int.context);
                res.set_from(n_int);
            } else if(type == NODE_INT && _right.type == NODE_FLOAT) {
                n_float.init(res_int.value + _right.res_float.value);
                n_float.set_context(res_int.context);
                res.set_from(n_float);
            } else if(type == NODE_FLOAT && _right.type == NODE_INT) {
                n_float.init(res_float.value + _right.res_int.value);
                n_float.set_context(res_float.context);
                res.set_from(n_float);
            } else {
                n_float.init(res_float.value + _right.res_float.value);
                n_float.set_context(res_float.context);
                res.set_from(n_float);
            }

            return res;
        }

        InterpreterResult substracted_by(InterpreterResult _right) {
            InterpreterResult res;
            IntNumber n_int;
            FloatNumber n_float;
            if(type == NODE_INT && _right.type == NODE_INT) {
                n_int.init(res_int.value - _right.res_int.value);
                n_int.set_context(res_int.context);
                res.set_from(n_int);
            } else if(type == NODE_INT && _right.type == NODE_FLOAT) {
                n_float.init(res_int.value - _right.res_float.value);
                n_float.set_context(res_int.context);
                res.set_from(n_float);
            } else if(type == NODE_FLOAT && _right.type == NODE_INT) {
                n_float.init(res_float.value - _right.res_int.value);
                n_float.set_context(res_float.context);
                res.set_from(n_float);
            } else {
                n_float.init(res_float.value - _right.res_float.value);
                n_float.set_context(res_float.context);
                res.set_from(n_float);
            }

            return res;
        }

        InterpreterResult multiplied_by(InterpreterResult _right) {
            InterpreterResult res;
            IntNumber n_int;
            FloatNumber n_float;
            if(type == NODE_INT && _right.type == NODE_INT) {
                n_int.init(res_int.value * _right.res_int.value);
                n_int.set_context(res_int.context);
                res.set_from(n_int);
            } else if(type == NODE_INT && _right.type == NODE_FLOAT) {
                n_float.init(res_int.value * _right.res_float.value);
                n_float.set_context(res_int.context);
                res.set_from(n_float);
            } else if(type == NODE_FLOAT && _right.type == NODE_INT) {
                n_float.init(res_float.value * _right.res_int.value);
                n_float.set_context(res_float.context);
                res.set_from(n_float);
            } else {
                n_float.init(res_float.value * _right.res_float.value);
                n_float.set_context(res_float.context);
                res.set_from(n_float);
            }

            return res;
        }

        InterpreterResult divided_by(InterpreterResult _right) {
            InterpreterResult res;
            if(_right.type == NODE_INT && _right.res_int.value == 0) {
                RuntimeError e;
                e.init(_right.start, _right.end, "Division by zero", res_int.context);
                return res.failure(e);
            } else if(_right.type == NODE_FLOAT && _right.res_float.value == 0) {
                RuntimeError e;
                e.init(_right.start, _right.end, "Division by zero", res_float.context);
                return res.failure(e);
            }

            IntNumber n_int;
            FloatNumber n_float;
            if(type == NODE_INT && _right.type == NODE_INT) {
                n_float.init(res_int.value / _right.res_int.value);
                n_float.set_context(res_int.context);
                res.set_from(n_float);
            } else if(type == NODE_INT && _right.type == NODE_FLOAT) {
                n_float.init(res_int.value / _right.res_float.value);
                n_float.set_context(res_int.context);
                res.set_from(n_float);
            } else if(type == NODE_FLOAT && _right.type == NODE_INT) {
                n_float.init(res_float.value / _right.res_int.value);
                n_float.set_context(res_float.context);
                res.set_from(n_float);
            } else {
                n_float.init(res_float.value / _right.res_float.value);
                n_float.set_context(res_float.context);
                res.set_from(n_float);
            }

            return res;
        }

        InterpreterResult power_on(InterpreterResult _right) {
            InterpreterResult res;
            IntNumber n_int;
            FloatNumber n_float;
            if(type == NODE_INT && _right.type == NODE_INT) {
                n_int.init(pow(res_int.value, _right.res_int.value));
                n_int.set_context(res_int.context);
                res.set_from(n_int);
            } else if(type == NODE_INT && _right.type == NODE_FLOAT) {
                n_float.init(pow(res_int.value, _right.res_float.value));
                n_float.set_context(res_int.context);
                res.set_from(n_float);
            } else if(type == NODE_FLOAT && _right.type == NODE_INT) {
                n_float.init(pow(res_float.value, _right.res_int.value));
                n_float.set_context(res_float.context);
                res.set_from(n_float);
            } else {
                n_float.init(pow(res_float.value, _right.res_float.value));
                n_float.set_context(res_float.context);
                res.set_from(n_float);
            }

            return res;
        }

        InterpreterResult modulo_of(InterpreterResult _right) {
            InterpreterResult res;
            IntNumber n_int;
            FloatNumber n_float;
            if(type == NODE_INT && _right.type == NODE_INT) {
                n_int.init(res_int.value % _right.res_int.value);
                n_int.set_context(res_int.context);
                res.set_from(n_int);
            } else if(type == NODE_INT && _right.type == NODE_FLOAT) {
                n_float.init(fmod(res_int.value, _right.res_float.value));
                n_float.set_context(res_int.context);
                res.set_from(n_float);
            } else if(type == NODE_FLOAT && _right.type == NODE_INT) {
                n_float.init(fmod(res_float.value, _right.res_int.value));
                n_float.set_context(res_float.context);
                res.set_from(n_float);
            } else {
                n_float.init(fmod(res_float.value, _right.res_float.value));
                n_float.set_context(res_float.context);
                res.set_from(n_float);
            }

            return res;
        }
};