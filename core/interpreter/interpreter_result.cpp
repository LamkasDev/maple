#pragma once
#include "../../structures/number.cpp"
#include "../../structures/string.cpp"
#include "../../structures/object.cpp"
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
        String res_string;
        shared_ptr<List> res_list;
        shared_ptr<Function> res_func;
        shared_ptr<Object> res_obj;

        bool has_return_value = false;
        bool loop_should_continue = false;
        bool loop_should_break = false;

        InterpreterResult() {
            
        }

        void set_pos(Position _start, Position _end) {
            start = _start;
            end = _end;
        }

        void reset() {
            state = 0;
            has_return_value = false;
            loop_should_continue = false;
            loop_should_break = false;
        }

        InterpreterResult register_result(InterpreterResult _result) {
            state = _result.state;
            has_return_value = _result.has_return_value;
            loop_should_continue = _result.loop_should_continue;
            loop_should_break = _result.loop_should_break;
            e = _result.e;

            return _result;
        }

        InterpreterResult success() {
            reset();
            return *this;
        }

        InterpreterResult success_return() {
            reset();
            has_return_value = true;
            return *this;
        }

        InterpreterResult success_continue() {
            reset();
            loop_should_continue = true;
            return *this;
        }

        InterpreterResult success_break() {
            reset();
            loop_should_break = true;
            return *this;
        }

        InterpreterResult failure(Error _e) {
            reset();
            state = -1;
            e = _e;
            return *this;
        }

        bool should_return() {
            return state == -1 || has_return_value || loop_should_continue || loop_should_break;
        }

        void set_from(int _res_int) {
            IntNumber _res(_res_int);
            res_int = _res;
            type = NODE_INT;
        }

        void set_from(IntNumber _res_int) {
            res_int = _res_int;
            type = NODE_INT;
        }

        void set_from(float _res_float) {
            FloatNumber _res(_res_float);
            res_float = _res;
            type = NODE_FLOAT;
        }

        void set_from(FloatNumber _res_float) {
            res_float = _res_float;
            type = NODE_FLOAT;
        }

        void set_from(shared_ptr<Function> _res_func) {
            res_func = _res_func;
            type = NODE_FUNC_DEF;
        }

        void set_from(shared_ptr<Object> _res_obj) {
            res_obj = _res_obj;
            type = NODE_OBJECT_NEW;
        }

        void set_from(string _res_string) {
            String _res(_res_string);
            res_string = _res;
            type = NODE_STRING;
        }

        void set_from(String _res_string) {
            res_string = _res_string;
            type = NODE_STRING;
        }

        void set_from(shared_ptr<List> _res_list) {
            res_list = _res_list;
            type = NODE_LIST;
        }

        void set_from(InterpreterResult _res) {
            if(_res.type == NODE_INT) {
                res_int = _res.res_int;
                type = NODE_INT;
            } else if(_res.type == NODE_FLOAT) {
                res_float = _res.res_float;
                type = NODE_FLOAT;
            } else if(_res.type == NODE_FUNC_DEF) {
                res_func = _res.res_func;
                type = NODE_FUNC_DEF;
            } else if(_res.type == NODE_STRING) {
                res_string = _res.res_string;
                type = NODE_STRING;
            } else if(_res.type == NODE_LIST) {
                res_list = _res.res_list;
                type = NODE_LIST;
            } else if(_res.type == NODE_OBJECT_NEW) {
                res_obj = _res.res_obj;
                type = NODE_OBJECT_NEW;
            }
        }

        InterpreterResult process_binary(InterpreterResult _left, shared_ptr<Token> _op, InterpreterResult _right) {
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
            if((type == NODE_STRING && _right.type == NODE_INT) || (type == NODE_INT && _right.type == NODE_STRING) || (type == NODE_STRING && _right.type == NODE_FLOAT) || (type == NODE_FLOAT && _right.type == NODE_STRING) || (type == NODE_STRING && _right.type == NODE_STRING)) {
                InterpreterResult res;
                String n_str(get_string_value() + _right.get_string_value());
                res.set_from(n_str);

                return res;
            }

            if(((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT))) { return illegal_operation(_right); }

            InterpreterResult res;
            if(type == NODE_INT && _right.type == NODE_INT) {
                IntNumber n_int(res_int.value + _right.res_int.value);
                res.set_from(n_int);
            } else {
                FloatNumber n_float(get_value() + _right.get_value());
                res.set_from(n_float);
            }

            return res;
        }

        InterpreterResult substracted_by(InterpreterResult _right) {
            if((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT)) { return illegal_operation(_right); }

            InterpreterResult res;
            if(type == NODE_INT && _right.type == NODE_INT) {
                IntNumber n_int(res_int.value - _right.res_int.value);
                res.set_from(n_int);
            } else {
                FloatNumber n_float(get_value() - _right.get_value());
                res.set_from(n_float);
            }

            return res;
        }

        InterpreterResult multiplied_by(InterpreterResult _right) {
            if((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT)) { return illegal_operation(_right); }

            InterpreterResult res;
            if(type == NODE_INT && _right.type == NODE_INT) {
                IntNumber n_int(res_int.value * _right.res_int.value);
                res.set_from(n_int);
            } else {
                FloatNumber n_float(get_value() * _right.get_value());
                res.set_from(n_float);
            }

            return res;
        }

        InterpreterResult divided_by(InterpreterResult _right) {
            if((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT)) { return illegal_operation(_right); }

            InterpreterResult res;
            if(_right.type == NODE_INT && _right.res_int.value == 0) {
                RuntimeError e_0(_right.start, _right.end, "Division by zero");
                return res.failure(e_0);
            } else if(_right.type == NODE_FLOAT && _right.res_float.value == 0) {
                RuntimeError e_0(_right.start, _right.end, "Division by zero");
                return res.failure(e_0);
            }

            FloatNumber n_float(get_value() / _right.get_value());
            res.set_from(n_float);

            return res;
        }

        InterpreterResult power_on(InterpreterResult _right) {
            if((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT)) { return illegal_operation(_right); }

            InterpreterResult res;
            if(type == NODE_INT && _right.type == NODE_INT) {
                IntNumber n_int(pow(res_int.value, _right.res_int.value));
                res.set_from(n_int);
            } else {
                FloatNumber n_float(pow(get_value(), _right.get_value()));
                res.set_from(n_float);
            }

            return res;
        }

        InterpreterResult modulo_of(InterpreterResult _right) {
            if((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT)) { return illegal_operation(_right); }

            InterpreterResult res;
            if(type == NODE_INT && _right.type == NODE_INT) {
                IntNumber n_int(res_int.value % _right.res_int.value);
                res.set_from(n_int);
            } else {
                FloatNumber n_float(fmod(get_value(), _right.get_value()));
                res.set_from(n_float);
            }

            return res;
        }

        InterpreterResult get_comparison(shared_ptr<Token> _token, InterpreterResult _right) {
            if((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT)) { return illegal_operation(_right); }

            InterpreterResult res;
            IntNumber n_int(0);
            if(_token->type == TT_EQEQ) {
                n_int.value = get_value() == _right.get_value();
            } else if(_token->type == TT_NEQ) {
                n_int.value = get_value() != _right.get_value();
            } else if(_token->type == TT_LTHAN) {
                n_int.value = get_value() < _right.get_value();
            } else if(_token->type == TT_GTHAN) {
                n_int.value = get_value() > _right.get_value();
            } else if(_token->type == TT_LTHANEQ) {
                n_int.value = get_value() <= _right.get_value();
            } else if(_token->type == TT_GTHANEQ) {
                n_int.value = get_value() >= _right.get_value();
            }
            res.set_from(n_int);

            return res;
        }

        InterpreterResult and_by(InterpreterResult _right) {
            if((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT)) { return illegal_operation(_right); }

            InterpreterResult res;
            IntNumber n_int(get_value() && _right.get_value());
            res.set_from(n_int);

            return res;
        }

        InterpreterResult or_by(InterpreterResult _right) {
            if((type != NODE_INT && type != NODE_FLOAT) || (_right.type != NODE_INT && _right.type != NODE_FLOAT)) { return illegal_operation(_right); }

            InterpreterResult res;
            IntNumber n_int(get_value() || _right.get_value());
            res.set_from(n_int);

            return res;
        }

        bool is_true() {
            if(type == NODE_INT || type == NODE_FLOAT) {
                return get_value() != 0;
            } else if(type == NODE_STRING) {
                return get_string_value().length() > 0;
            } else {
                return false;
            }
        }

        InterpreterResult illegal_operation(InterpreterResult _right) {
            InterpreterResult res;
            RuntimeError e_0(start, _right.end, "Illegal operation");
            return res.failure(e_0);
        }

        float get_value() {
            if(type == NODE_INT) {
                return res_int.value;
            } else if(type == NODE_FLOAT) {
                return res_float.value;
            } else {
                return 0;
            }
        }

        string get_string_value() {
            if(type == NODE_INT) {
                return to_string(res_int.value);
            } else if(type == NODE_FLOAT) {
                return to_string(res_float.value).substr(0, to_string(res_float.value).find(".")+4);;
            } else if(type == NODE_STRING) {
                return res_string.value;
            } else {
                return "";
            }
        }

        string repr() {
            if(type == NODE_INT) {
                return res_int.repr();
            } else if(type == NODE_FLOAT) {
                return res_float.repr();
            } else if(type == NODE_FUNC_DEF) {
                return res_func->repr();
            } else if(type == NODE_STRING) {
                return res_string.repr();
            } else if(type == NODE_LIST) {
                return res_list->repr();
            } else if(type == NODE_OBJECT_NEW) {
                return res_obj->repr();
            } else {
                return "";
            }
        }
};