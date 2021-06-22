#pragma once
using namespace std;

class ParseAll {
    public:
        ParserResult power(shared_ptr<ParserStore> parser_store) {
            ParserResult result;
            
            ParserResult left = result.register_result(call(parser_store));
            if(result.state == -1) { return result; }

            shared_ptr<Node> op = make_shared<Node>();
            op->set_pos(left.node->start, left.node->end);
            op->set_type(NODE_BINARY);
            op->set_to_left(left.node);

            while(parser_store->current_t->type == TT_POW) {
                shared_ptr<Token> op_token = parser_store->current_t;
                result.register_advance(parser_store->advance());

                ParserResult right = result.register_result(factor(parser_store));
                if(result.state == -1) {
                    break;
                }

                if(op->right != nullptr) {
                    shared_ptr<Node> copy = op->copy();
                    op->set_to_left(copy);
                }
                op->set_token(op_token);
                op->set_to_right(right.node);
            }
            if(result.state == -1) { return result; }

            op->set_end(parser_store->current_t->start);
            
            result.set_node(op);
            return result.success();
        }

        ParserResult factor(shared_ptr<ParserStore> parser_store) {
            ParserResult result;
            shared_ptr<Token> t = parser_store->current_t;
            if(t->type == TT_PLUS || t->type == TT_MINUS) {
                result.register_advance(parser_store->advance());
                ParserResult n_0 = factor(parser_store);
                if(n_0.state == -1) { return result.failure(n_0.e); }
                shared_ptr<Node> n = make_shared<Node>(); n->set_pos(t->start, n_0.node->end); n->set_type(NODE_UNARY); n->set_token(t); n->set_to_right(n_0.node);

                result.set_node(n);
                return result.success();
            }

            return power(parser_store);
        }

        ParserResult atom(shared_ptr<ParserStore> parser_store) {
            ParserResult result;
            shared_ptr<Token> t = parser_store->current_t;
            if(t->type == TT_INT) {
                result.register_advance(parser_store->advance());
                shared_ptr<Node> n = make_shared<Node>(); n->set_pos(t->start, t->end); n->set_value(t->value_int);

                result.set_node(n);
                return result.success();
            } else if(t->type == TT_FLOAT) {
                result.register_advance(parser_store->advance());
                shared_ptr<Node> n = make_shared<Node>(); n->set_pos(t->start, t->end); n->set_value(t->value_float);

                result.set_node(n);
                return result.success();
            } else if(t->type == TT_STRING) {
                result.register_advance(parser_store->advance());
                shared_ptr<Node> n = make_shared<Node>(); n->set_pos(t->start, t->end); n->set_value(t->value_string);

                result.set_node(n);
                return result.success();
            } else if(t->type == TT_IDENFIFIER) {
                result.register_advance(parser_store->advance());
                shared_ptr<Node> n = make_shared<Node>(); n->set_pos(t->start, t->end); n->set_type(NODE_ACCESS); n->set_token(t);

                result.set_node(n);
                return result.success();
            } else if(t->type == TT_LPAREN) {
                result.register_advance(parser_store->advance());
                ParserResult n_0 = expression(parser_store);
                if(n_0.state == -1) { return result.failure(n_0.e); }
                if(parser_store->current_t->type == TT_RPAREN) {
                    result.register_advance(parser_store->advance());

                    result.set_node(n_0.node);
                    return result.success();
                } else {
                    return result.failure(create_syntax_error(parser_store, "')'"));
                }
            } else if(t->type == TT_LCBRACKET) {
                ParserResult expr = map_expr(parser_store);
                if(expr.state == -1) { return result.failure(expr.e); }

                result.set_node(expr.node);
                return result.success();
            } else if(t->type == TT_LSBRACKET) {
                ParserResult expr = list_expr(parser_store);
                if(expr.state == -1) { return result.failure(expr.e); }

                result.set_node(expr.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_IF)) {
                ParserResult expr = if_expr(parser_store);
                if(expr.state == -1) { return result.failure(expr.e); }

                result.set_node(expr.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_FOR)) {
                ParserResult expr = for_expr(parser_store);
                if(expr.state == -1) { return result.failure(expr.e); }

                result.set_node(expr.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_FOREACH)) {
                ParserResult expr = foreach_expr(parser_store);
                if(expr.state == -1) { return result.failure(expr.e); }

                result.set_node(expr.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_WHILE)) {
                ParserResult expr = while_expr(parser_store);
                if(expr.state == -1) { return result.failure(expr.e); }

                result.set_node(expr.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_FUNC)) {
                ParserResult def = func_def(parser_store);
                if(def.state == -1) { return result.failure(def.e); }

                result.set_node(def.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_CONSTRUCTOR)) {
                ParserResult def = constructor_def(parser_store);
                if(def.state == -1) { return result.failure(def.e); }

                result.set_node(def.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_NEW)) {
                ParserResult def = object_def(parser_store);
                if(def.state == -1) { return result.failure(def.e); }

                result.set_node(def.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_CLASS)) {
                ParserResult def = class_def(parser_store);
                if(def.state == -1) { return result.failure(def.e); }

                result.set_node(def.node);
                return result.success();
            }

            return result.failure(create_syntax_error(parser_store, "int, float, string, list, identifier or a keyword"));
        }

        ParserResult call(shared_ptr<ParserStore> parser_store) {
            ParserResult result;
            
            ParserResult left = result.register_result(atom(parser_store));
            if(result.state == -1) { return result; }

            if(parser_store->current_t->type == TT_DOT) {
                shared_ptr<Node> chained_node = make_shared<Node>();
                chained_node->set_pos(parser_store->current_t->start, parser_store->current_t->end);
                chained_node->set_type(NODE_CHAINED);

                result.register_advance(parser_store->advance());

                ParserResult right = result.register_result(call(parser_store));
                if(result.state == -1) { return result; }

                chained_node->set_to_left(left.node);
                chained_node->set_to_right(right.node);

                if(parser_store->current_t->type == TT_EQ) {
                    result.register_advance(parser_store->advance());

                    ParserResult expr = result.register_result(expression(parser_store));
                    if(result.state == -1) { return result; }

                    chained_node->set_type(NODE_CHAINED_ASSIGNMENT);
                    chained_node->set_chained_assigment_result(expr.node);
                }

                chained_node->set_end(parser_store->current_t->start);
                result.set_node(chained_node);

                return result.success();
            } else if(parser_store->current_t->type == TT_LPAREN) {
                shared_ptr<Node> call_node = make_shared<Node>();
                call_node->set_pos(parser_store->current_t->start, parser_store->current_t->end);
                call_node->set_type(NODE_FUNC_CALL);
                list<shared_ptr<Node>> arguments;

                result.register_advance(parser_store->advance());
                
                if(parser_store->current_t->type == TT_RPAREN) {
                    result.register_advance(parser_store->advance());
                } else {
                    ParserResult arg = result.register_result(expression(parser_store));
                    if(result.state == -1) {
                        return result.failure(create_syntax_error(parser_store, "')' or an argument"));
                    }
                    arguments.push_back(arg.node);

                    while(parser_store->current_t->type == TT_COMMA) {
                        result.register_advance(parser_store->advance());

                        ParserResult arg_1 = result.register_result(expression(parser_store));
                        if(result.state == -1) { break; }
                        arguments.push_back(arg_1.node);
                    }
                    if(result.state == -1) { return result; }

                    if(parser_store->current_t->type != TT_RPAREN) {
                        return result.failure(create_syntax_error(parser_store, "',' or ')'"));
                    }
                    result.register_advance(parser_store->advance());
                }

                call_node->set_end(parser_store->current_t->start);
                call_node->set_func_call_argument_nodes_result(arguments);
                call_node->set_func_call_expression_result(left.node);
                result.set_node(call_node);

                return result.success();
            }

            shared_ptr<Node> op = make_shared<Node>();
            op->set_pos(left.node->start, left.node->end);
            op->set_type(NODE_BINARY);
            op->set_to_left(left.node);

            while(parser_store->current_t->type == TT_POW) {
                shared_ptr<Token> op_token = parser_store->current_t;
                result.register_advance(parser_store->advance());

                ParserResult right = result.register_result(factor(parser_store));
                if(result.state == -1) {
                    break;
                }

                if(op->right != nullptr) {
                    shared_ptr<Node> copy = op->copy();
                    op->set_to_left(copy);
                }
                op->set_token(op_token);
                op->set_to_right(right.node);
            }
            if(result.state == -1) { return result; }

            op->set_end(parser_store->current_t->start);
            
            result.set_node(op);
            return result.success();
        }

        ParserResult term(shared_ptr<ParserStore> parser_store) {
            ParserResult result;

            ParserResult left = result.register_result(factor(parser_store));
            if(result.state == -1) { return result; }

            shared_ptr<Node> op = make_shared<Node>();
            op->set_pos(left.node->start, left.node->end);
            op->set_type(NODE_BINARY);
            op->set_to_left(left.node);

            while(parser_store->current_t->type == TT_MUL || parser_store->current_t->type == TT_DIV || parser_store->current_t->type == TT_MOD) {
                shared_ptr<Token> op_token = parser_store->current_t;
                result.register_advance(parser_store->advance());

                ParserResult right = result.register_result(factor(parser_store));
                if(result.state == -1) {
                    break;
                }

                if(op->right != nullptr) {
                    shared_ptr<Node> copy = op->copy();
                    op->set_to_left(copy);
                }
                op->set_token(op_token);
                op->set_to_right(right.node);
            }
            if(result.state == -1) { return result; }

            op->set_end(parser_store->current_t->start);
            
            result.set_node(op);
            return result.success();
        }

        ParserResult arith_expr(shared_ptr<ParserStore> parser_store) {
            ParserResult result;

            ParserResult left = result.register_result(term(parser_store));
            if(result.state == -1) { return result; }

            shared_ptr<Node> op = make_shared<Node>();
            op->set_pos(left.node->start, left.node->end);
            op->set_type(NODE_BINARY);
            op->set_to_left(left.node);

            while(parser_store->current_t->type == TT_PLUS || parser_store->current_t->type == TT_MINUS) {
                shared_ptr<Token> op_token = parser_store->current_t;
                result.register_advance(parser_store->advance());

                ParserResult right = result.register_result(term(parser_store));
                if(result.state == -1) {
                    break;
                }

                if(op->right != nullptr) {
                    shared_ptr<Node> copy = op->copy();
                    op->set_to_left(copy);
                }
                op->set_token(op_token);
                op->set_to_right(right.node);
            }
            if(result.state == -1) { return result; }

            op->set_end(parser_store->current_t->start);
            
            result.set_node(op);
            return result.success();
        }

        ParserResult arith_expr_pre(shared_ptr<ParserStore> parser_store) {
            ParserResult result;

            ParserResult left = result.register_result(arith_expr(parser_store));
            if(result.state == -1) { return result; }

            shared_ptr<Node> op = make_shared<Node>();
            op->set_pos(left.node->start, left.node->end);
            op->set_type(NODE_BINARY);
            op->set_to_left(left.node);

            while(parser_store->current_t->type == TT_EQEQ || parser_store->current_t->type == TT_NEQ || parser_store->current_t->type == TT_LTHAN || parser_store->current_t->type == TT_GTHAN || parser_store->current_t->type == TT_LTHANEQ || parser_store->current_t->type == TT_GTHANEQ) {
                shared_ptr<Token> op_token = parser_store->current_t;
                result.register_advance(parser_store->advance());

                ParserResult right = result.register_result(term(parser_store));
                if(result.state == -1) {
                    break;
                }

                if(op->right != nullptr) {
                    shared_ptr<Node> copy = op->copy();
                    op->set_to_left(copy);
                }
                op->set_token(op_token);
                op->set_to_right(right.node);
            }
            if(result.state == -1) { return result; }

            op->set_end(parser_store->current_t->start);
            
            result.set_node(op);
            return result.success();
        }

        ParserResult comp_expr(shared_ptr<ParserStore> parser_store) {
            ParserResult result;

            if(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_NOT)) {
                shared_ptr<Token> op_token = parser_store->current_t;
                result.register_advance(parser_store->advance());

                ParserResult expr = result.register_result(comp_expr(parser_store));
                if(result.state == -1) { return result; }
                shared_ptr<Node> n = make_shared<Node>(); n->set_pos(op_token->start, expr.node->end); n->set_type(NODE_UNARY); n->set_token(op_token); n->set_to_right(expr.node);

                result.set_node(n);
                return result.success();
            }

            ParserResult arith = result.register_result(arith_expr_pre(parser_store));
            if(result.state == -1) { return result.failure(create_syntax_error(parser_store, "int, float, string, identifier, list, keyword, comparison, '+', '-' or 'NOT'")); }
            
            result.set_node(arith.node);
            return result.success();
        }

        ParserResult list_expr(shared_ptr<ParserStore> parser_store) {
            ParserResult result;
            list<shared_ptr<Node>> elements;
            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(parser_store->current_t->start, parser_store->current_t->end);
            node->set_type(NODE_LIST);

            if(parser_store->current_t->type != TT_LSBRACKET) {
                return result.failure(create_syntax_error(parser_store, "'['", 1));
            }
            result.register_advance(parser_store->advance());

            if(parser_store->current_t->type == TT_RSBRACKET) {
                result.register_advance(parser_store->advance());
            } else {
                ParserResult element = result.register_result(expression(parser_store));
                if(result.state == -1) {
                    return result.failure(create_syntax_error(parser_store, "int, float, string, identifier, list, keyword, comparison, '+', '-' or 'NOT'"));
                }
                elements.push_back(element.node);

                while(parser_store->current_t->type == TT_COMMA) {
                    result.register_advance(parser_store->advance());

                    ParserResult element_1 = result.register_result(expression(parser_store));
                    if(result.state == -1) { break; }
                    elements.push_back(element_1.node);
                }
                if(result.state == -1) { return result; }

                if(parser_store->current_t->type != TT_RSBRACKET) {
                    return result.failure(create_syntax_error(parser_store, "',' or ']'"));
                }
                result.register_advance(parser_store->advance());
            }

            node->set_end(parser_store->current_t->start);
            node->set_list_nodes_result(elements);
            
            result.set_node(node);
            return result.success();
        }

        ParserResult map_expr(shared_ptr<ParserStore> parser_store) {
            ParserResult result;
            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(parser_store->current_t->start, parser_store->current_t->end);
            node->set_type(NODE_MAP);

            if(parser_store->current_t->type != TT_LCBRACKET) {
                return result.failure(create_syntax_error(parser_store, "'{'", 1));
            }
            result.register_advance(parser_store->advance());

            if(parser_store->current_t->type != TT_RCBRACKET) {
                return result.failure(create_syntax_error(parser_store, "'}'", 1));
            }
            result.register_advance(parser_store->advance());

            node->set_end(parser_store->current_t->start);

            result.set_node(node);
            return result.success();
        }

        ParserResult if_expr(shared_ptr<ParserStore> parser_store) {
            ParserResult result;

            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(parser_store->current_t->start, parser_store->current_t->end);
            node->set_type(NODE_IF);

            ParserResult all_cases = result.register_result(if_expr_cases(parser_store, NODE_IF));
            if(result.state == -1) { return result; }

            node->set_end(parser_store->current_t->start);
            node->set_if_results(all_cases.node->if_results);
            node->set_else_result(all_cases.node->else_result);
            
            result.set_node(node);
            return result.success();
        }

        ParserResult if_expr_b(shared_ptr<ParserStore> parser_store) {
            return if_expr_cases(parser_store, KEYWORD_ELIF);
        }

        ParserResult if_expr_c(shared_ptr<ParserStore> parser_store) {
            ParserResult result;

            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(parser_store->current_t->start, parser_store->current_t->end);
            node->set_type(NODE_IF);

            if(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_ELSE)) {
                result.register_advance(parser_store->advance());

                if(parser_store->current_t->type != TT_LCBRACKET) {
                    return result.failure(create_syntax_error(parser_store, "'{'", 1));
                }
                result.register_advance(parser_store->advance());

                ParserResult all_statements = result.register_result(statements(parser_store));
                if(result.state == -1) { return result; }

                node->set_else_result(all_statements.node);

                if(parser_store->current_t->type == TT_RCBRACKET) {
                    result.register_advance(parser_store->advance());
                } else {
                    return result.failure(create_syntax_error(parser_store, "'}'", 1));
                }
            }

            node->set_end(parser_store->current_t->start);

            result.set_node(node);
            return result.success();
        }

        ParserResult if_expr_b_or_c(shared_ptr<ParserStore> parser_store) {
            ParserResult result;

            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(parser_store->current_t->start, parser_store->current_t->end);
            node->set_type(NODE_IF);

            if(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_ELIF)) {
                ParserResult all_cases = result.register_result(if_expr_b(parser_store));
                if(result.state == -1) { return result; }

                node->set_if_results(all_cases.node->if_results);
                node->set_else_result(all_cases.node->else_result);
            } else {
                ParserResult else_case = result.register_result(if_expr_c(parser_store));
                if(result.state == -1) { return result; }

                node->set_else_result(else_case.node->else_result);
            }

            node->set_end(parser_store->current_t->start);

            result.set_node(node);
            return result;
        }

        ParserResult if_expr_cases(shared_ptr<ParserStore> parser_store, string type) {
            ParserResult result;
            list<shared_ptr<Node>> cases;

            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(parser_store->current_t->start, parser_store->current_t->end);
            node->set_type(NODE_IF);

            if(parser_store->current_t->matches(TT_KEYWORD, type) == false) {
                return result.failure(create_syntax_error(parser_store, "'" + type + "'", 1));
            }
            result.register_advance(parser_store->advance());

            if(parser_store->current_t->type != TT_LPAREN) {
                return result.failure(create_syntax_error(parser_store, "'('", 1));
            }
            result.register_advance(parser_store->advance());

            ParserResult condition = result.register_result(expression(parser_store));
            if(result.state == -1) { return result; }

            if(parser_store->current_t->type != TT_RPAREN) {
                return result.failure(create_syntax_error(parser_store, "')'", 1));
            }

            result.register_advance(parser_store->advance());

            if(parser_store->current_t->type != TT_LCBRACKET) {
                return result.failure(create_syntax_error(parser_store, "'{'", 1));
            }
            result.register_advance(parser_store->advance());

            ParserResult all_statements = result.register_result(statements(parser_store));
            if(result.state == -1) { return result; }
            cases.push_back(condition.node);
            cases.push_back(all_statements.node);

            if(parser_store->current_t->type != TT_RCBRACKET) {
                return result.failure(create_syntax_error(parser_store, "'}'", 1));
            }
            result.register_advance(parser_store->advance());

            if(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_ELIF) || parser_store->current_t->matches(TT_KEYWORD, KEYWORD_ELSE)) {
                ParserResult all_cases = result.register_result(if_expr_b_or_c(parser_store));
                if(result.state == -1) { return result; }
                copy(all_cases.node->if_results.begin(), all_cases.node->if_results.end(), back_inserter(cases));
                
                node->set_else_result(all_cases.node->else_result);
            }

            node->set_end(parser_store->current_t->start);
            node->set_if_results(cases);

            result.set_node(node);
            return result.success();
        }

        ParserResult for_expr(shared_ptr<ParserStore> parser_store) {
            ParserResult result;
            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(parser_store->current_t->start, parser_store->current_t->end);
            node->set_type(NODE_FOR);

            if(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_FOR) == false) {
                return result.failure(create_syntax_error(parser_store, "'FOR'", 1));
            }
            result.register_advance(parser_store->advance());

            if(parser_store->current_t->type != TT_LPAREN) {
                return result.failure(create_syntax_error(parser_store, "'('", 1));
            }
            result.register_advance(parser_store->advance());

            ParserResult start_value = result.register_result(expression(parser_store));
            if(result.state == -1) { return result; }
            if(start_value.node->type != NODE_ASSIGNMENT) {
                return result.failure(create_syntax_error(parser_store, "assigment", 1));
            }

            if(parser_store->current_t->type != TT_RPAREN) {
                return result.failure(create_syntax_error(parser_store, "')'", 1));
            }
            result.register_advance(parser_store->advance());

            if(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_TO) == false) {
                return result.failure(create_syntax_error(parser_store, "'TO'", 1));
            }
            result.register_advance(parser_store->advance());

            ParserResult end_value = result.register_result(expression(parser_store));
            if(result.state == -1) { return result; }

            if(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_STEP)) {
                result.register_advance(parser_store->advance());

                ParserResult step_value = result.register_result(expression(parser_store));
                if(result.state == -1) { return result; }
                node->set_for_step_result(step_value.node);
            }

            if(parser_store->current_t->type != TT_LCBRACKET) {
                return result.failure(create_syntax_error(parser_store, "'{'", 1));
            }
            result.register_advance(parser_store->advance());

            ParserResult expr = result.register_result(statements(parser_store));
            if(result.state == -1) { return result; }

            node->set_for_expr_result(expr.node);

            if(parser_store->current_t->type != TT_RCBRACKET) {
                return result.failure(create_syntax_error(parser_store, "'}'", 1));
            }
            result.register_advance(parser_store->advance());

            node->set_end(parser_store->current_t->start);
            node->set_token(start_value.node->token);
            node->set_for_start_result(start_value.node);
            node->set_for_end_result(end_value.node);

            result.set_node(node);
            return result.success();
        }

        ParserResult foreach_expr(shared_ptr<ParserStore> parser_store) {
            ParserResult result;
            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(parser_store->current_t->start, parser_store->current_t->end);
            node->set_type(NODE_FOREACH);

            if(parser_store->current_t->matches(TT_KEYWORD, NODE_FOREACH) == false) {
                return result.failure(create_syntax_error(parser_store, "'FOREACH'", 1));
            }
            result.register_advance(parser_store->advance());

            if(parser_store->current_t->type != TT_LPAREN) {
                return result.failure(create_syntax_error(parser_store, "'('", 1));
            }
            result.register_advance(parser_store->advance());

            shared_ptr<Token> id = parser_store->current_t;
            if(parser_store->current_t->type != TT_IDENFIFIER) {
                return result.failure(create_syntax_error(parser_store, "identifier", 1));
            }
            result.register_advance(parser_store->advance());

            if(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_IN) == false) {
                return result.failure(create_syntax_error(parser_store, "'IN'", 1));
            }
            result.register_advance(parser_store->advance());

            ParserResult list = result.register_result(expression(parser_store));
            if(result.state == -1) { return result; }

            if(parser_store->current_t->type != TT_RPAREN) {
                return result.failure(create_syntax_error(parser_store, "')'", 1));
            }
            result.register_advance(parser_store->advance());

            if(parser_store->current_t->type != TT_LCBRACKET) {
                return result.failure(create_syntax_error(parser_store, "'{'", 1));
            }
            result.register_advance(parser_store->advance());

            ParserResult expr = result.register_result(statements(parser_store));
            if(result.state == -1) { return result; }

            node->set_for_expr_result(expr.node);

            if(parser_store->current_t->type != TT_RCBRACKET) {
                return result.failure(create_syntax_error(parser_store, "'}'", 1));
            }
            result.register_advance(parser_store->advance());

            node->set_end(parser_store->current_t->start);
            node->set_token(id);
            node->set_for_start_result(list.node);

            result.set_node(node);
            return result.success();
        }

        ParserResult while_expr(shared_ptr<ParserStore> parser_store) {
            ParserResult result;
            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(parser_store->current_t->start, parser_store->current_t->end);
            node->set_type(NODE_WHILE);

            if(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_WHILE) == false) {
                return result.failure(create_syntax_error(parser_store, "'WHILE'", 1));
            }
            result.register_advance(parser_store->advance());

            ParserResult condition = result.register_result(expression(parser_store));
            if(result.state == -1) { return result; }

            if(parser_store->current_t->type != TT_LCBRACKET) {
                return result.failure(create_syntax_error(parser_store, "'{'", 1));
            }
            result.register_advance(parser_store->advance());

            ParserResult expr = result.register_result(statements(parser_store));
            if(result.state == -1) { return result; }

            if(parser_store->current_t->type != TT_RCBRACKET) {
                return result.failure(create_syntax_error(parser_store, "'}'", 1));
            }
            result.register_advance(parser_store->advance());

            node->set_end(parser_store->current_t->start);
            node->set_while_expr_result(expr.node);
            node->set_while_condition_result(condition.node);

            result.set_node(node);
            return result.success();
        }

        ParserResult func_def(shared_ptr<ParserStore> parser_store) {
            ParserResult result;
            vector<shared_ptr<Token>> arguments;

            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(parser_store->current_t->start, parser_store->current_t->end);
            node->set_type(NODE_FUNC_DEF);

            if(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_FUNC) == false) {
                return result.failure(create_syntax_error(parser_store, "'FUNC'", 1));
            }
            result.register_advance(parser_store->advance());

            if(parser_store->current_t->type == TT_IDENFIFIER) {
                shared_ptr<Token> var_name = parser_store->current_t;
                result.register_advance(parser_store->advance());

                if(parser_store->current_t->type != TT_LPAREN) {
                    return result.failure(create_syntax_error(parser_store, "'('", 1));
                }

                node->set_token(var_name);
            } else {
                if(parser_store->current_t->type != TT_LPAREN) {
                    return result.failure(create_syntax_error(parser_store, "identifier or '('", 1));
                }
            }

            result.register_advance(parser_store->advance());

            bool identifier_errored_out = false;
            InvalidSyntaxError id_e;
            if(parser_store->current_t->type == TT_IDENFIFIER) {
                arguments.push_back(parser_store->current_t);
                result.register_advance(parser_store->advance());

                while(parser_store->current_t->type == TT_COMMA) {
                    result.register_advance(parser_store->advance());
                    
                    if(parser_store->current_t->type != TT_IDENFIFIER) {
                        id_e = create_syntax_error(parser_store, "identifier", 1);
                        identifier_errored_out = true;
                        break;
                    }

                    arguments.push_back(parser_store->current_t);
                    result.register_advance(parser_store->advance());
                }

                if(identifier_errored_out == false && parser_store->current_t->type != TT_RPAREN) {
                    id_e = create_syntax_error(parser_store, "',' or ')'", 1);
                    identifier_errored_out = true;
                }
            } else {
                if(parser_store->current_t->type != TT_RPAREN) {
                    id_e = create_syntax_error(parser_store, "identifier or ')'", 1);
                    identifier_errored_out = true;
                }
            }
            if(identifier_errored_out == true) {
                return result.failure(id_e);
            }
            result.register_advance(parser_store->advance());

            if(parser_store->current_t->type == TT_LCBRACKET) {
                result.register_advance(parser_store->advance());

                ParserResult expr = result.register_result(statements(parser_store));
                if(result.state == -1) { return result; }

                if(parser_store->current_t->type != TT_RCBRACKET) {
                    return result.failure(create_syntax_error(parser_store, "'}'", 1));
                }
                result.register_advance(parser_store->advance());

                node->set_func_def_expression_result(expr.node);
            } else if(parser_store->current_t->type == TT_ARROW) {
                result.register_advance(parser_store->advance());

                ParserResult expr = result.register_result(expression(parser_store));
                if(result.state == -1) { return result; }

                node->set_func_def_expression_result(expr.node);
            } else {
                return result.failure(create_syntax_error(parser_store, "'->' or '{'", 1));
            }

            node->set_end(parser_store->current_t->start);
            node->set_func_def_argument_tokens_result(arguments);
            
            result.set_node(node);
            return result.success();
        }

        ParserResult constructor_def(shared_ptr<ParserStore> parser_store) {
            ParserResult result;
            vector<shared_ptr<Token>> arguments;

            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(parser_store->current_t->start, parser_store->current_t->end);
            node->set_type(NODE_CONSTRUCTOR_DEF);

            if(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_CONSTRUCTOR) == false) {
                return result.failure(create_syntax_error(parser_store, "'CONSTRUCTOR'", 1));
            }
            result.register_advance(parser_store->advance());

            if(parser_store->current_t->type != TT_LPAREN) {
                return result.failure(create_syntax_error(parser_store, "'('", 1));
            }
            result.register_advance(parser_store->advance());

            bool identifier_errored_out = false;
            InvalidSyntaxError id_e;
            if(parser_store->current_t->type == TT_IDENFIFIER) {
                arguments.push_back(parser_store->current_t);
                result.register_advance(parser_store->advance());

                while(parser_store->current_t->type == TT_COMMA) {
                    result.register_advance(parser_store->advance());
                    
                    if(parser_store->current_t->type != TT_IDENFIFIER) {
                        id_e = create_syntax_error(parser_store, "identifier", 1);
                        identifier_errored_out = true;
                        break;
                    }

                    arguments.push_back(parser_store->current_t);
                    result.register_advance(parser_store->advance());
                }

                if(identifier_errored_out == false && parser_store->current_t->type != TT_RPAREN) {
                    id_e = create_syntax_error(parser_store, "',' or ')'", 1);
                    identifier_errored_out = true;
                }
            } else {
                if(parser_store->current_t->type != TT_RPAREN) {
                    id_e = create_syntax_error(parser_store, "identifier or ')'", 1);
                    identifier_errored_out = true;
                }
            }
            if(identifier_errored_out == true) {
                return result.failure(id_e);
            }
            result.register_advance(parser_store->advance());

            if(parser_store->current_t->type == TT_LCBRACKET) {
                result.register_advance(parser_store->advance());

                ParserResult expr = result.register_result(statements(parser_store));
                if(result.state == -1) { return result; }

                if(parser_store->current_t->type != TT_RCBRACKET) {
                    return result.failure(create_syntax_error(parser_store, "'}'", 1));
                }
                result.register_advance(parser_store->advance());

                node->set_func_def_expression_result(expr.node);
            } else {
                return result.failure(create_syntax_error(parser_store, "'{'", 1));
            }

            node->set_end(parser_store->current_t->start);
            node->set_func_def_argument_tokens_result(arguments);
            
            result.set_node(node);
            return result.success();
        }

        ParserResult class_def(shared_ptr<ParserStore> parser_store) {
            ParserResult result;

            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(parser_store->current_t->start, parser_store->current_t->end);
            node->set_type(NODE_CLASS_DEF);

            if(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_CLASS) == false) {
                return result.failure(create_syntax_error(parser_store, "'CLASS'", 1));
            }
            result.register_advance(parser_store->advance());

            if(parser_store->current_t->matches(TT_KEYWORD, parser_store->object_keywords)) {
                shared_ptr<Token> var_name = parser_store->current_t;
                result.register_advance(parser_store->advance());
                node->set_token(var_name);
            } else {
                return result.failure(create_syntax_error(parser_store, "identifier", 1));
            }

            if(parser_store->current_t->type == TT_LCBRACKET) {
                result.register_advance(parser_store->advance());

                ParserResult expr = result.register_result(statements(parser_store));
                if(result.state == -1) { return result; }

                if(parser_store->current_t->type != TT_RCBRACKET) {
                    return result.failure(create_syntax_error(parser_store, "'}'", 1));
                }
                result.register_advance(parser_store->advance());

                node->set_class_def_expression_result(expr.node);
            } else {
                return result.failure(create_syntax_error(parser_store, "'{'", 1));
            }
            
            node->set_end(parser_store->current_t->start);
            
            result.set_node(node);
            return result.success();
        }

        ParserResult object_def(shared_ptr<ParserStore> parser_store) {
            ParserResult result;

            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(parser_store->current_t->start, parser_store->current_t->end);
            node->set_type(NODE_OBJECT_NEW);

            if(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_NEW) == false) {
                return result.failure(create_syntax_error(parser_store, "'NEW'", 1));
            }
            result.register_advance(parser_store->advance());

            if(parser_store->current_t->matches(TT_KEYWORD, parser_store->object_keywords) == false) {
                return result.failure(create_syntax_error(parser_store, "a classname", 1));
            }
            node->set_token(parser_store->current_t);
            result.register_advance(parser_store->advance());

            if(parser_store->current_t->type != TT_LPAREN) {
                return result.failure(create_syntax_error(parser_store, "'('", 1));
            }
            result.register_advance(parser_store->advance());

            list<shared_ptr<Node>> arguments;
            
            if(parser_store->current_t->type == TT_RPAREN) {
                result.register_advance(parser_store->advance());
            } else {
                ParserResult arg = result.register_result(expression(parser_store));
                if(result.state == -1) {
                    return result.failure(create_syntax_error(parser_store, "')' or an argument"));
                }
                arguments.push_back(arg.node);

                while(parser_store->current_t->type == TT_COMMA) {
                    result.register_advance(parser_store->advance());

                    ParserResult arg_1 = result.register_result(expression(parser_store));
                    if(result.state == -1) { break; }
                    arguments.push_back(arg_1.node);
                }
                if(result.state == -1) { return result; }

                if(parser_store->current_t->type != TT_RPAREN) {
                    return result.failure(create_syntax_error(parser_store, "',' or ')'"));
                }
                result.register_advance(parser_store->advance());
            }

            node->set_end(parser_store->current_t->start);
            node->set_object_argument_nodes_result(arguments);
            
            result.set_node(node);
            return result.success();
        }

        ParserResult expression(shared_ptr<ParserStore> parser_store) {
            ParserResult result;

            if(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_VAR)) {
                result.register_advance(parser_store->advance());
                if(parser_store->current_t->type != TT_IDENFIFIER) {
                    return result.failure(create_syntax_error(parser_store, "an identifier"));
                }
                shared_ptr<Token> identifier = parser_store->current_t;
                result.register_advance(parser_store->advance());

                shared_ptr<Token> assignment_token = parser_store->current_t;

                if(assignment_token->type != TT_EQ && assignment_token->type != TT_PLUSEQ && assignment_token->type != TT_MINUSEQ) {
                    return result.failure(create_syntax_error(parser_store, "'=', '+=' or '-='"));
                }
                result.register_advance(parser_store->advance());

                ParserResult expr = result.register_result(expression(parser_store));
                if(result.state == -1) { return result; }
                shared_ptr<Node> n = make_shared<Node>(); n->set_pos(identifier->start, expr.node->end); n->set_type(NODE_ASSIGNMENT); n->set_token(identifier); n->set_assignment_token(assignment_token); n->set_to_right(expr.node);
                
                result.set_node(n);
                return result.success();
            }

            ParserResult left = result.register_result(comp_expr(parser_store));
            if(result.state == -1) {
                if(result.e.extra == 1) {
                    return result;
                } else {
                    return result.failure(create_syntax_error(parser_store, "int, float, string, identifier, list, keyword, comparison, '+', '-' or 'NOT'"));
                }
            }

            shared_ptr<Node> op = make_shared<Node>();
            op->set_pos(left.node->start, left.node->end);
            op->set_type(NODE_BINARY);
            op->set_to_left(left.node);
            
            while(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_AND) || parser_store->current_t->matches(TT_KEYWORD, KEYWORD_OR)) {
                shared_ptr<Token> op_token = parser_store->current_t;
                op->set_token(op_token);
                result.register_advance(parser_store->advance());

                ParserResult right = result.register_result(comp_expr(parser_store));
                if(result.state == -1) {
                    break;
                }

                if(op->right != nullptr) {
                    shared_ptr<Node> copy = op->copy();
                    op->set_to_left(copy);
                }
                op->set_to_right(right.node);
            }
            if(result.state == -1) { return result; }

            op->set_end(parser_store->current_t->start);
            
            result.set_node(op);
            return result.success();
        }

        ParserResult statement(shared_ptr<ParserStore> parser_store) {
            ParserResult result;
            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(parser_store->current_t->start, parser_store->current_t->end);

            if(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_RETURN)) {
                result.register_advance(parser_store->advance());

                ParserResult expr = result.register_result(expression(parser_store));
                if(result.state == -1) {
                    parser_store->unadvance(result.reverse_count);
                }
                node->set_to_right(expr.node);
                node->set_type(NODE_RETURN);
            } else if(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_CONTINUE)) {
                result.register_advance(parser_store->advance());
                node->set_type(NODE_CONTINUE);
            } else if(parser_store->current_t->matches(TT_KEYWORD, KEYWORD_BREAK)) {
                result.register_advance(parser_store->advance());
                node->set_type(NODE_BREAK);
            } else {
                ParserResult expr = result.register_result(expression(parser_store));
                if(result.state == -1) { return result.failure(create_syntax_error(parser_store, "int, float, string, identifier, list, keyword, comparison, '+', '-' or 'NOT'")); }
                node = expr.node;
            }

            result.set_node(node);
            return result.success();
        }

        ParserResult statements(shared_ptr<ParserStore> parser_store) {
            ParserResult result;
            list<shared_ptr<Node>> statements;
            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(parser_store->current_t->start, parser_store->current_t->end);
            node->set_type(NODE_STATEMENTS);

            ParserResult res_statement = result.register_result(statement(parser_store));
            if(result.state == -1) { return result; }
            statements.push_back(res_statement.node);

            if(parser_store->current_t->type == TT_NEWLINE) {
                result.register_advance(parser_store->advance());

                while(true) {
                    if(parser_store->current_t == nullptr || parser_store->current_t->type == TT_RCBRACKET || parser_store->current_t->type == TT_EOF) {
                        break;
                    }
                    ParserResult res_statement_1 = result.register_result(statement(parser_store));
                    if(result.state == -1) {
                        parser_store->unadvance(result.reverse_count);
                        break;
                    }
                    if(parser_store->current_t->type != TT_NEWLINE) {
                        ExpectedCharacterError e(parser_store->file_name, parser_store->current_t->start, parser_store->current_t->end, "Expected ';'");
                        result.failure(e);
                        break;
                    }

                    result.register_advance(parser_store->advance());
                    statements.push_back(res_statement_1.node);
                }
            }

            if(result.state == -1) { return result; }

            node->set_end(parser_store->current_t->start);
            node->set_statements_nodes_result(statements);

            result.set_node(node);
            return result.success();
        }
};