#pragma once
#include <string>
#include "../../structures/number.cpp"
#include "../../structures/function.cpp"
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
        Function res_func;

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

        void set_from(Function _res_func) {
            res_func = _res_func;
            type = NODE_FUNC_DEF;
        }

        void set_from(InterpreterResult _res) {
            if(_res.type == NODE_INT) {
                res_int = _res.res_int;
                type = NODE_INT;
            } else if(_res.type == NODE_FLOAT) {
                res_float = _res.res_float;
                type = NODE_FLOAT;
            } else {
                res_func = _res.res_func;
                type = NODE_FUNC_DEF;
            }
        }

        InterpreterResult processNumber(InterpreterResult _left, Token* _op, InterpreterResult _right) {
            if(_op->type == TT_PLUS) {
                return _left.added_to(_right);
            } else if(_op->type == TT_MINUS) {
                return _left.substracted_by(_right);
            } else if(_op->type == TT_MUL) {
                return _left.multiplied_by(_right);
            } else if(_op->type == TT_DIV) {
                return _left.divided_by(_right);
            } else if(_op->type == TT_POW) {
                return _left.power_on(_right);
            } else if(_op->type == TT_EQEQ || _op->type == TT_NEQ || _op->type == TT_LTHAN || _op->type == TT_GTHAN || _op->type == TT_LTHANEQ || _op->type == TT_GTHANEQ) {
                return _left.get_comparison(_op, _right);
            } else if(_op->matches(TT_KEYWORD, KEYWORD_AND)) {
                return _left.and_by(_right);
            } else if(_op->matches(TT_KEYWORD, KEYWORD_OR))  {
                return _left.or_by(_right);
            } else if(_op->type == TT_MOD) {
                return _left.modulo_of(_right);
            }

            InterpreterResult res;
            return res;
        }

        InterpreterResult added_to(InterpreterResult _right) {
            if((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT)) { return illegal_operation(_right); }

            InterpreterResult res;
            IntNumber n_int;
            FloatNumber n_float;
            if(type == NODE_INT && _right.type == NODE_INT) {
                n_int.init(res_int.value + _right.res_int.value);
                n_int.set_context(res_int.context);
            } else {
                n_float.init(get_value() + _right.get_value());
                n_float.set_context(get_context());
            }
            if(type == NODE_INT && _right.type == NODE_INT) { res.set_from(n_int); } else { res.set_from(n_float); }

            return res;
        }

        InterpreterResult substracted_by(InterpreterResult _right) {
            if((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT)) { return illegal_operation(_right); }

            InterpreterResult res;
            IntNumber n_int;
            FloatNumber n_float;
            if(type == NODE_INT && _right.type == NODE_INT) {
                n_int.init(res_int.value - _right.res_int.value);
                n_int.set_context(res_int.context);
            } else {
                n_float.init(get_value() - _right.get_value());
                n_float.set_context(get_context());
            }
            if(type == NODE_INT && _right.type == NODE_INT) { res.set_from(n_int); } else { res.set_from(n_float); }

            return res;
        }

        InterpreterResult multiplied_by(InterpreterResult _right) {
            if((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT)) { return illegal_operation(_right); }

            InterpreterResult res;
            IntNumber n_int;
            FloatNumber n_float;
            if(type == NODE_INT && _right.type == NODE_INT) {
                n_int.init(res_int.value * _right.res_int.value);
                n_int.set_context(res_int.context);
            } else {
                n_float.init(get_value() * _right.get_value());
                n_float.set_context(get_context());
            }
            if(type == NODE_INT && _right.type == NODE_INT) { res.set_from(n_int); } else { res.set_from(n_float); }

            return res;
        }

        InterpreterResult divided_by(InterpreterResult _right) {
            if((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT)) { return illegal_operation(_right); }

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

            FloatNumber n_float;
            n_float.init(get_value() / _right.get_value());
            n_float.set_context(get_context());
            res.set_from(n_float);

            return res;
        }

        InterpreterResult power_on(InterpreterResult _right) {
            if((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT)) { return illegal_operation(_right); }

            InterpreterResult res;
            IntNumber n_int;
            FloatNumber n_float;
            if(type == NODE_INT && _right.type == NODE_INT) {
                n_int.init(pow(res_int.value, _right.res_int.value));
                n_int.set_context(res_int.context);
            } else {
                n_float.init(pow(get_value(), _right.get_value()));
                n_float.set_context(get_context());
            }
            if(type == NODE_INT && _right.type == NODE_INT) { res.set_from(n_int); } else { res.set_from(n_float); }

            return res;
        }

        InterpreterResult modulo_of(InterpreterResult _right) {
            if((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT)) { return illegal_operation(_right); }

            InterpreterResult res;
            IntNumber n_int;
            FloatNumber n_float;
            if(type == NODE_INT && _right.type == NODE_INT) {
                n_int.init(res_int.value % _right.res_int.value);
                n_int.set_context(res_int.context);
            } else {
                n_float.init(fmod(get_value(), _right.get_value()));
                n_float.set_context(get_context());
            }
            if(type == NODE_INT && _right.type == NODE_INT) { res.set_from(n_int); } else { res.set_from(n_float); }

            return res;
        }

        InterpreterResult get_comparison(Token* _token, InterpreterResult _right) {
            if((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT)) { return illegal_operation(_right); }

            InterpreterResult res;
            IntNumber n_int;
            if(_token->type == TT_EQEQ) {
                n_int.init(get_value() == _right.get_value());
            } else if(_token->type == TT_EQEQ) {
                n_int.init(get_value() == _right.get_value());
            } else if(_token->type == TT_NEQ) {
                n_int.init(get_value() != _right.get_value());
            } else if(_token->type == TT_LTHAN) {
                n_int.init(get_value() < _right.get_value());
            } else if(_token->type == TT_GTHAN) {
                n_int.init(get_value() > _right.get_value());
            } else if(_token->type == TT_LTHANEQ) {
                n_int.init(get_value() <= _right.get_value());
            } else if(_token->type == TT_GTHANEQ) {
                n_int.init(get_value() >= _right.get_value());
            }
            n_int.set_context(get_context());
            res.set_from(n_int);

            return res;
        }

        InterpreterResult and_by(InterpreterResult _right) {
            if((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT)) { return illegal_operation(_right); }

            InterpreterResult res;
            IntNumber n_int;
            n_int.init(get_value() && _right.get_value());
            n_int.set_context(get_context());
            res.set_from(n_int);

            return res;
        }

        InterpreterResult or_by(InterpreterResult _right) {
            if((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT)) { return illegal_operation(_right); }

            InterpreterResult res;
            IntNumber n_int;
            n_int.init(get_value() || _right.get_value());
            n_int.set_context(get_context());
            res.set_from(n_int);

            return res;
        }

        bool is_true() {
            return get_value() != 0;
        }

        InterpreterResult illegal_operation(InterpreterResult _right) {
            InterpreterResult res;

            RuntimeError e;
            e.init(start, _right.end, "Illegal operation", get_context());
            return res.failure(e);
        }

        float get_value() {
            if(type == NODE_INT) {
                return res_int.value;
            } else {
                return res_float.value;
            }
        }

        Context* get_context() {
            if(type == NODE_INT) {
                return res_int.context;
            } else if(type == NODE_FLOAT) {
                return res_float.context;
            } else {
                return res_func.context;
            }
        }
};