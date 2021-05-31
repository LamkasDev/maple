#pragma once
using namespace std;

class Parser {
    public:
        list<shared_ptr<Token>> tokens;
        shared_ptr<Token> current_t = nullptr;
        vector<string> object_keywords;
        int index = 0;

        Parser(vector<string> _object_keywords, list<shared_ptr<Token>> _tokens) {
            object_keywords = _object_keywords;
            tokens = _tokens;
            index = 1;
            advance();
        }

        int advance() {
            index++;
            update_current_token();
            return 1;
        }

        int unadvance(int ammount) {
            index -= ammount;
            update_current_token();
            return 1;
        }

        void update_current_token() {
            if(tokens.size() > 0) {
                current_t = tokens.front();
                tokens.pop_front();
            } else {
                current_t = nullptr;
            }
        }

        ParserResult parse() {
            ParserResult res = statements();
            if(res.state != -1 && current_t->type != TT_EOF) {
                if(res.e.extra == 1) {
                    return res;
                } else {
                    return res.failure(create_syntax_error("'+', '-', '*', '/', '^', '==', '!=', '<', '>', '<=', '>=', 'AND' or 'OR'"));
                }
            }

            return res;
        }

        ParserResult atom() {
            ParserResult result;
            shared_ptr<Token> t = current_t;
            if(t->type == TT_INT) {
                result.register_advance(advance());
                shared_ptr<Node> n = make_shared<Node>(); n->set_pos(t->start, t->end); n->set_value(t->value_int);

                result.set_node(n);
                return result.success();
            } else if(t->type == TT_FLOAT) {
                result.register_advance(advance());
                shared_ptr<Node> n = make_shared<Node>(); n->set_pos(t->start, t->end); n->set_value(t->value_float);

                result.set_node(n);
                return result.success();
            } else if(t->type == TT_STRING) {
                result.register_advance(advance());
                shared_ptr<Node> n = make_shared<Node>(); n->set_pos(t->start, t->end); n->set_value(t->value_string);

                result.set_node(n);
                return result.success();
            } else if(t->type == TT_IDENFIFIER) {
                result.register_advance(advance());
                shared_ptr<Node> n = make_shared<Node>(); n->set_pos(t->start, t->end); n->set_type(NODE_ACCESS); n->set_token(t);

                result.set_node(n);
                return result.success();
            } else if(t->type == TT_LPAREN) {
                result.register_advance(advance());
                ParserResult n_0 = expression();
                if(n_0.state == -1) { return result.failure(n_0.e); }
                if(current_t->type == TT_RPAREN) {
                    result.register_advance(advance());

                    result.set_node(n_0.node);
                    return result.success();
                } else {
                    return result.failure(create_syntax_error("')'"));
                }
            } else if(t->type == TT_LSBRACKET) {
                ParserResult expr = list_expr();
                if(expr.state == -1) { return result.failure(expr.e); }

                result.set_node(expr.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_IF)) {
                ParserResult expr = if_expr();
                if(expr.state == -1) { return result.failure(expr.e); }

                result.set_node(expr.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_FOR)) {
                ParserResult expr = for_expr();
                if(expr.state == -1) { return result.failure(expr.e); }

                result.set_node(expr.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_FOREACH)) {
                ParserResult expr = foreach_expr();
                if(expr.state == -1) { return result.failure(expr.e); }

                result.set_node(expr.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_WHILE)) {
                ParserResult expr = while_expr();
                if(expr.state == -1) { return result.failure(expr.e); }

                result.set_node(expr.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_FUNC)) {
                ParserResult def = func_def();
                if(def.state == -1) { return result.failure(def.e); }

                result.set_node(def.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_CONSTRUCTOR)) {
                ParserResult def = constructor_def();
                if(def.state == -1) { return result.failure(def.e); }

                result.set_node(def.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_NEW)) {
                ParserResult def = object_def();
                if(def.state == -1) { return result.failure(def.e); }

                result.set_node(def.node);
                return result.success();
            } else if(t->matches(TT_KEYWORD, KEYWORD_CLASS)) {
                ParserResult def = class_def();
                if(def.state == -1) { return result.failure(def.e); }

                result.set_node(def.node);
                return result.success();
            }

            return result.failure(create_syntax_error("int, float, string, list, identifier or a keyword"));
        }

        ParserResult call() {
            ParserResult result;
            
            ParserResult left = result.register_result(atom());
            if(result.state == -1) { return result; }

            if(current_t->type == TT_DOT) {
                shared_ptr<Node> chained_node = make_shared<Node>();
                chained_node->set_pos(current_t->start, current_t->end);
                chained_node->set_type(NODE_CHAINED);

                result.register_advance(advance());

                ParserResult right = result.register_result(call());
                if(result.state == -1) { return result; }

                chained_node->set_to_left(left.node);
                chained_node->set_to_right(right.node);

                if(current_t->type == TT_EQ) {
                    result.register_advance(advance());

                    ParserResult expr = result.register_result(expression());
                    if(result.state == -1) { return result; }

                    chained_node->set_type(NODE_CHAINED_ASSIGNMENT);
                    chained_node->set_chained_assigment_result(expr.node);
                }

                chained_node->set_end(current_t->start);
                result.set_node(chained_node);

                return result.success();
            } else if(current_t->type == TT_LPAREN) {
                shared_ptr<Node> call_node = make_shared<Node>();
                call_node->set_pos(current_t->start, current_t->end);
                call_node->set_type(NODE_FUNC_CALL);
                list<shared_ptr<Node>> arguments;

                result.register_advance(advance());
                
                if(current_t->type == TT_RPAREN) {
                    result.register_advance(advance());
                } else {
                    ParserResult arg = result.register_result(expression());
                    if(result.state == -1) {
                        return result.failure(create_syntax_error("')' or an argument"));
                    }
                    arguments.push_back(arg.node);

                    while(current_t->type == TT_COMMA) {
                        result.register_advance(advance());

                        ParserResult arg_1 = result.register_result(expression());
                        if(result.state == -1) { break; }
                        arguments.push_back(arg_1.node);
                    }
                    if(result.state == -1) { return result; }

                    if(current_t->type != TT_RPAREN) {
                        return result.failure(create_syntax_error("',' or ')'"));
                    }
                    result.register_advance(advance());
                }

                call_node->set_end(current_t->start);
                call_node->set_func_call_argument_nodes_result(arguments);
                call_node->set_func_call_expression_result(left.node);
                result.set_node(call_node);

                return result.success();
            }

            shared_ptr<Node> op = make_shared<Node>();
            op->set_pos(left.node->start, left.node->end);
            op->set_type(NODE_BINARY);
            op->set_to_left(left.node);

            while(current_t->type == TT_POW) {
                shared_ptr<Token> op_token = current_t;
                result.register_advance(advance());

                ParserResult right = result.register_result(factor());
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

            op->set_end(current_t->start);
            
            result.set_node(op);
            return result.success();
        }

        ParserResult power() {
            ParserResult result;
            
            ParserResult left = result.register_result(call());
            if(result.state == -1) { return result; }

            shared_ptr<Node> op = make_shared<Node>();
            op->set_pos(left.node->start, left.node->end);
            op->set_type(NODE_BINARY);
            op->set_to_left(left.node);

            while(current_t->type == TT_POW) {
                shared_ptr<Token> op_token = current_t;
                result.register_advance(advance());

                ParserResult right = result.register_result(factor());
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

            op->set_end(current_t->start);
            
            result.set_node(op);
            return result.success();
        }

        ParserResult factor() {
            ParserResult result;
            shared_ptr<Token> t = current_t;
            if(t->type == TT_PLUS || t->type == TT_MINUS) {
                result.register_advance(advance());
                ParserResult n_0 = factor();
                if(n_0.state == -1) { return result.failure(n_0.e); }
                shared_ptr<Node> n = make_shared<Node>(); n->set_pos(t->start, n_0.node->end); n->set_type(NODE_UNARY); n->set_token(t); n->set_to_right(n_0.node);

                result.set_node(n);
                return result.success();
            }

            return power();
        }

        ParserResult term() {
            ParserResult result;

            ParserResult left = result.register_result(factor());
            if(result.state == -1) { return result; }

            shared_ptr<Node> op = make_shared<Node>();
            op->set_pos(left.node->start, left.node->end);
            op->set_type(NODE_BINARY);
            op->set_to_left(left.node);

            while(current_t->type == TT_MUL || current_t->type == TT_DIV || current_t->type == TT_MOD) {
                shared_ptr<Token> op_token = current_t;
                result.register_advance(advance());

                ParserResult right = result.register_result(factor());
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

            op->set_end(current_t->start);
            
            result.set_node(op);
            return result.success();
        }

        ParserResult arith_expr() {
            ParserResult result;

            ParserResult left = result.register_result(term());
            if(result.state == -1) { return result; }

            shared_ptr<Node> op = make_shared<Node>();
            op->set_pos(left.node->start, left.node->end);
            op->set_type(NODE_BINARY);
            op->set_to_left(left.node);

            while(current_t->type == TT_PLUS || current_t->type == TT_MINUS) {
                shared_ptr<Token> op_token = current_t;
                result.register_advance(advance());

                ParserResult right = result.register_result(term());
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

            op->set_end(current_t->start);
            
            result.set_node(op);
            return result.success();
        }

        ParserResult arith_expr_pre() {
            ParserResult result;

            ParserResult left = result.register_result(arith_expr());
            if(result.state == -1) { return result; }

            shared_ptr<Node> op = make_shared<Node>();
            op->set_pos(left.node->start, left.node->end);
            op->set_type(NODE_BINARY);
            op->set_to_left(left.node);

            while(current_t->type == TT_EQEQ || current_t->type == TT_NEQ || current_t->type == TT_LTHAN || current_t->type == TT_GTHAN || current_t->type == TT_LTHANEQ || current_t->type == TT_GTHANEQ) {
                shared_ptr<Token> op_token = current_t;
                result.register_advance(advance());

                ParserResult right = result.register_result(term());
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

            op->set_end(current_t->start);
            
            result.set_node(op);
            return result.success();
        }

        ParserResult comp_expr() {
            ParserResult result;

            if(current_t->matches(TT_KEYWORD, KEYWORD_NOT)) {
                shared_ptr<Token> op_token = current_t;
                result.register_advance(advance());

                ParserResult expr = result.register_result(comp_expr());
                if(result.state == -1) { return result; }
                shared_ptr<Node> n = make_shared<Node>(); n->set_pos(op_token->start, expr.node->end); n->set_type(NODE_UNARY); n->set_token(op_token); n->set_to_right(expr.node);

                result.set_node(n);
                return result.success();
            }

            ParserResult arith = result.register_result(arith_expr_pre());
            if(result.state == -1) { return result.failure(create_syntax_error("int, float, string, identifier, list, keyword, comparison, '+', '-' or 'NOT'")); }
            
            result.set_node(arith.node);
            return result.success();
        }

        ParserResult statements() {
            ParserResult result;
            list<shared_ptr<Node>> statements;
            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(current_t->start, current_t->end);
            node->set_type(NODE_STATEMENTS);

            ParserResult res_statement = result.register_result(statement());
            if(result.state == -1) { return result; }
            statements.push_back(res_statement.node);

            if(current_t->type == TT_NEWLINE) {
                result.register_advance(advance());

                while(true) {
                    if(current_t == nullptr || current_t->type == TT_RCBRACKET || current_t->type == TT_EOF) {
                        break;
                    }
                    ParserResult res_statement_1 = result.register_result(statement());
                    if(result.state == -1) {
                        unadvance(result.reverse_count);
                        break;
                    }
                    if(current_t->type != TT_NEWLINE) {
                        ExpectedCharacterError e(current_t->start, current_t->end, "Expected ';'");
                        result.failure(e);
                        break;
                    }

                    result.register_advance(advance());
                    statements.push_back(res_statement_1.node);
                }
            }

            if(result.state == -1) { return result; }

            node->set_end(current_t->start);
            node->set_statements_nodes_result(statements);

            result.set_node(node);
            return result.success();
        }

        ParserResult statement() {
            ParserResult result;
            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(current_t->start, current_t->end);

            if(current_t->matches(TT_KEYWORD, KEYWORD_RETURN)) {
                result.register_advance(advance());

                ParserResult expr = result.register_result(expression());
                if(result.state == -1) {
                    unadvance(result.reverse_count);
                }
                node->set_to_right(expr.node);
                node->set_type(NODE_RETURN);
            } else if(current_t->matches(TT_KEYWORD, KEYWORD_CONTINUE)) {
                result.register_advance(advance());
                node->set_type(NODE_CONTINUE);
            } else if(current_t->matches(TT_KEYWORD, KEYWORD_BREAK)) {
                result.register_advance(advance());
                node->set_type(NODE_BREAK);
            } else {
                ParserResult expr = result.register_result(expression());
                if(result.state == -1) { return result.failure(create_syntax_error("int, float, string, identifier, list, keyword, comparison, '+', '-' or 'NOT'")); }
                node = expr.node;
            }

            result.set_node(node);
            return result.success();
        }
        
        ParserResult expression() {
            ParserResult result;

            if(current_t->matches(TT_KEYWORD, KEYWORD_VAR)) {
                result.register_advance(advance());
                if(current_t->type != TT_IDENFIFIER) {
                    return result.failure(create_syntax_error("an identifier"));
                }
                shared_ptr<Token> identifier = current_t;
                result.register_advance(advance());

                if(current_t->type != TT_EQ) {
                    return result.failure(create_syntax_error("'='"));
                }
                result.register_advance(advance());

                ParserResult expr = result.register_result(expression());
                if(result.state == -1) { return result; }
                shared_ptr<Node> n = make_shared<Node>(); n->set_pos(identifier->start, expr.node->end); n->set_type(NODE_ASSIGNMENT); n->set_token(identifier); n->set_to_right(expr.node);
                
                result.set_node(n);
                return result.success();
            }

            ParserResult left = result.register_result(comp_expr());
            if(result.state == -1) {
                if(result.e.extra == 1) {
                    return result;
                } else {
                    return result.failure(create_syntax_error("int, float, string, identifier, list, keyword, comparison, '+', '-' or 'NOT'"));
                }
            }

            shared_ptr<Node> op = make_shared<Node>();
            op->set_pos(left.node->start, left.node->end);
            op->set_type(NODE_BINARY);
            op->set_to_left(left.node);
            
            while(current_t->matches(TT_KEYWORD, KEYWORD_AND) || current_t->matches(TT_KEYWORD, KEYWORD_OR)) {
                shared_ptr<Token> op_token = current_t;
                op->set_token(op_token);
                result.register_advance(advance());

                ParserResult right = result.register_result(comp_expr());
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

            op->set_end(current_t->start);
            
            result.set_node(op);
            return result.success();
        }

        ParserResult list_expr() {
            ParserResult result;
            list<shared_ptr<Node>> elements;
            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(current_t->start, current_t->end);
            node->set_type(NODE_LIST);

            if(current_t->type != TT_LSBRACKET) {
                return result.failure(create_syntax_error("'['", 1));
            }
            result.register_advance(advance());

            if(current_t->type == TT_RSBRACKET) {
                result.register_advance(advance());
            } else {
                ParserResult element = result.register_result(expression());
                if(result.state == -1) {
                    return result.failure(create_syntax_error("int, float, string, identifier, list, keyword, comparison, '+', '-' or 'NOT'"));
                }
                elements.push_back(element.node);

                while(current_t->type == TT_COMMA) {
                    result.register_advance(advance());

                    ParserResult element_1 = result.register_result(expression());
                    if(result.state == -1) { break; }
                    elements.push_back(element_1.node);
                }
                if(result.state == -1) { return result; }

                if(current_t->type != TT_RSBRACKET) {
                    return result.failure(create_syntax_error("',' or ']'"));
                }
                result.register_advance(advance());
            }

            node->set_end(current_t->start);
            node->set_list_nodes_result(elements);
            
            result.set_node(node);
            return result.success();
        }

        ParserResult if_expr() {
            ParserResult result;

            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(current_t->start, current_t->end);
            node->set_type(NODE_IF);

            ParserResult all_cases = result.register_result(if_expr_cases(NODE_IF));
            if(result.state == -1) { return result; }

            node->set_end(current_t->start);
            node->set_if_results(all_cases.node->if_results);
            node->set_else_result(all_cases.node->else_result);
            
            result.set_node(node);
            return result.success();
        }

        ParserResult if_expr_b() {
            return if_expr_cases(KEYWORD_ELIF);
        }

        ParserResult if_expr_c() {
            ParserResult result;

            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(current_t->start, current_t->end);
            node->set_type(NODE_IF);

            if(current_t->matches(TT_KEYWORD, KEYWORD_ELSE)) {
                result.register_advance(advance());

                if(current_t->type != TT_LCBRACKET) {
                    return result.failure(create_syntax_error("'{'", 1));
                }
                result.register_advance(advance());

                ParserResult all_statements = result.register_result(statements());
                if(result.state == -1) { return result; }

                node->set_else_result(all_statements.node);

                if(current_t->type == TT_RCBRACKET) {
                    result.register_advance(advance());
                } else {
                    return result.failure(create_syntax_error("'}'", 1));
                }
            }

            node->set_end(current_t->start);

            result.set_node(node);
            return result.success();
        }

        ParserResult if_expr_b_or_c() {
            ParserResult result;

            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(current_t->start, current_t->end);
            node->set_type(NODE_IF);

            if(current_t->matches(TT_KEYWORD, KEYWORD_ELIF)) {
                ParserResult all_cases = result.register_result(if_expr_b());
                if(result.state == -1) { return result; }

                node->set_if_results(all_cases.node->if_results);
                node->set_else_result(all_cases.node->else_result);
            } else {
                ParserResult else_case = result.register_result(if_expr_c());
                if(result.state == -1) { return result; }

                node->set_else_result(else_case.node->else_result);
            }

            node->set_end(current_t->start);

            result.set_node(node);
            return result;
        }

        ParserResult if_expr_cases(string type) {
            ParserResult result;
            list<shared_ptr<Node>> cases;

            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(current_t->start, current_t->end);
            node->set_type(NODE_IF);

            if(current_t->matches(TT_KEYWORD, type) == false) {
                return result.failure(create_syntax_error("'" + type + "'", 1));
            }
            result.register_advance(advance());

            if(current_t->type != TT_LPAREN) {
                return result.failure(create_syntax_error("'('", 1));
            }
            result.register_advance(advance());

            ParserResult condition = result.register_result(expression());
            if(result.state == -1) { return result; }

            if(current_t->type != TT_RPAREN) {
                return result.failure(create_syntax_error("')'", 1));
            }

            result.register_advance(advance());

            if(current_t->type != TT_LCBRACKET) {
                return result.failure(create_syntax_error("'{'", 1));
            }
            result.register_advance(advance());

            ParserResult all_statements = result.register_result(statements());
            if(result.state == -1) { return result; }
            cases.push_back(condition.node);
            cases.push_back(all_statements.node);

            if(current_t->type != TT_RCBRACKET) {
                return result.failure(create_syntax_error("'}'", 1));
            }
            result.register_advance(advance());

            if(current_t->matches(TT_KEYWORD, KEYWORD_ELIF) || current_t->matches(TT_KEYWORD, KEYWORD_ELSE)) {
                ParserResult all_cases = result.register_result(if_expr_b_or_c());
                if(result.state == -1) { return result; }
                copy(all_cases.node->if_results.begin(), all_cases.node->if_results.end(), back_inserter(cases));
                
                node->set_else_result(all_cases.node->else_result);
            }

            node->set_end(current_t->start);
            node->set_if_results(cases);

            result.set_node(node);
            return result.success();
        }

        ParserResult for_expr() {
            ParserResult result;
            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(current_t->start, current_t->end);
            node->set_type(NODE_FOR);

            if(current_t->matches(TT_KEYWORD, KEYWORD_FOR) == false) {
                return result.failure(create_syntax_error("'FOR'", 1));
            }
            result.register_advance(advance());

            if(current_t->type != TT_LPAREN) {
                return result.failure(create_syntax_error("'('", 1));
            }
            result.register_advance(advance());

            ParserResult start_value = result.register_result(expression());
            if(result.state == -1) { return result; }
            if(start_value.node->type != NODE_ASSIGNMENT) {
                return result.failure(create_syntax_error("assigment", 1));
            }

            if(current_t->type != TT_RPAREN) {
                return result.failure(create_syntax_error("')'", 1));
            }
            result.register_advance(advance());

            if(current_t->matches(TT_KEYWORD, KEYWORD_TO) == false) {
                return result.failure(create_syntax_error("'TO'", 1));
            }
            result.register_advance(advance());

            ParserResult end_value = result.register_result(expression());
            if(result.state == -1) { return result; }

            if(current_t->matches(TT_KEYWORD, KEYWORD_STEP)) {
                result.register_advance(advance());

                ParserResult step_value = result.register_result(expression());
                if(result.state == -1) { return result; }
                node->set_for_step_result(step_value.node);
            }

            if(current_t->type != TT_LCBRACKET) {
                return result.failure(create_syntax_error("'{'", 1));
            }
            result.register_advance(advance());

            ParserResult expr = result.register_result(statements());
            if(result.state == -1) { return result; }

            node->set_for_expr_result(expr.node);

            if(current_t->type != TT_RCBRACKET) {
                return result.failure(create_syntax_error("'}'", 1));
            }
            result.register_advance(advance());

            node->set_end(current_t->start);
            node->set_token(start_value.node->token);
            node->set_for_start_result(start_value.node);
            node->set_for_end_result(end_value.node);

            result.set_node(node);
            return result.success();
        }

        ParserResult foreach_expr() {
            ParserResult result;
            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(current_t->start, current_t->end);
            node->set_type(NODE_FOREACH);

            if(current_t->matches(TT_KEYWORD, NODE_FOREACH) == false) {
                return result.failure(create_syntax_error("'FOREACH'", 1));
            }
            result.register_advance(advance());

            if(current_t->type != TT_LPAREN) {
                return result.failure(create_syntax_error("'('", 1));
            }
            result.register_advance(advance());

            shared_ptr<Token> id = current_t;
            if(current_t->type != TT_IDENFIFIER) {
                return result.failure(create_syntax_error("identifier", 1));
            }
            result.register_advance(advance());

            if(current_t->matches(TT_KEYWORD, KEYWORD_IN) == false) {
                return result.failure(create_syntax_error("'IN'", 1));
            }
            result.register_advance(advance());

            ParserResult list = result.register_result(expression());
            if(result.state == -1) { return result; }

            if(current_t->type != TT_RPAREN) {
                return result.failure(create_syntax_error("')'", 1));
            }
            result.register_advance(advance());

            if(current_t->type != TT_LCBRACKET) {
                return result.failure(create_syntax_error("'{'", 1));
            }
            result.register_advance(advance());

            ParserResult expr = result.register_result(statements());
            if(result.state == -1) { return result; }

            node->set_for_expr_result(expr.node);

            if(current_t->type != TT_RCBRACKET) {
                return result.failure(create_syntax_error("'}'", 1));
            }
            result.register_advance(advance());

            node->set_end(current_t->start);
            node->set_token(id);
            node->set_for_start_result(list.node);

            result.set_node(node);
            return result.success();
        }

        ParserResult while_expr() {
            ParserResult result;
            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(current_t->start, current_t->end);
            node->set_type(NODE_WHILE);

            if(current_t->matches(TT_KEYWORD, KEYWORD_WHILE) == false) {
                return result.failure(create_syntax_error("'WHILE'", 1));
            }
            result.register_advance(advance());

            ParserResult condition = result.register_result(expression());
            if(result.state == -1) { return result; }

            if(current_t->type != TT_LCBRACKET) {
                return result.failure(create_syntax_error("'{'", 1));
            }
            result.register_advance(advance());

            ParserResult expr = result.register_result(statements());
            if(result.state == -1) { return result; }

            if(current_t->type != TT_RCBRACKET) {
                return result.failure(create_syntax_error("'}'", 1));
            }
            result.register_advance(advance());

            node->set_end(current_t->start);
            node->set_while_expr_result(expr.node);
            node->set_while_condition_result(condition.node);

            result.set_node(node);
            return result.success();
        }

        ParserResult func_def() {
            ParserResult result;
            vector<shared_ptr<Token>> arguments;

            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(current_t->start, current_t->end);
            node->set_type(NODE_FUNC_DEF);

            if(current_t->matches(TT_KEYWORD, KEYWORD_FUNC) == false) {
                return result.failure(create_syntax_error("'FUNC'", 1));
            }
            result.register_advance(advance());

            if(current_t->type == TT_IDENFIFIER) {
                shared_ptr<Token> var_name = current_t;
                result.register_advance(advance());

                if(current_t->type != TT_LPAREN) {
                    return result.failure(create_syntax_error("'('", 1));
                }

                node->set_token(var_name);
            } else {
                if(current_t->type != TT_LPAREN) {
                    return result.failure(create_syntax_error("identifier or '('", 1));
                }
            }

            result.register_advance(advance());

            bool identifier_errored_out = false;
            InvalidSyntaxError id_e;
            if(current_t->type == TT_IDENFIFIER) {
                arguments.push_back(current_t);
                result.register_advance(advance());

                while(current_t->type == TT_COMMA) {
                    result.register_advance(advance());
                    
                    if(current_t->type != TT_IDENFIFIER) {
                        id_e = create_syntax_error("identifier", 1);
                        identifier_errored_out = true;
                        break;
                    }

                    arguments.push_back(current_t);
                    result.register_advance(advance());
                }

                if(identifier_errored_out == false && current_t->type != TT_RPAREN) {
                    id_e = create_syntax_error("',' or ')'", 1);
                    identifier_errored_out = true;
                }
            } else {
                if(current_t->type != TT_RPAREN) {
                    id_e = create_syntax_error("identifier or ')'", 1);
                    identifier_errored_out = true;
                }
            }
            if(identifier_errored_out == true) {
                return result.failure(id_e);
            }
            result.register_advance(advance());

            if(current_t->type == TT_LCBRACKET) {
                result.register_advance(advance());

                ParserResult expr = result.register_result(statements());
                if(result.state == -1) { return result; }

                if(current_t->type != TT_RCBRACKET) {
                    return result.failure(create_syntax_error("'}'", 1));
                }
                result.register_advance(advance());

                node->set_func_def_expression_result(expr.node);
            } else if(current_t->type == TT_ARROW) {
                result.register_advance(advance());

                ParserResult expr = result.register_result(expression());
                if(result.state == -1) { return result; }

                node->set_func_def_expression_result(expr.node);
            } else {
                return result.failure(create_syntax_error("'->' or '{'", 1));
            }

            node->set_end(current_t->start);
            node->set_func_def_argument_tokens_result(arguments);
            
            result.set_node(node);
            return result.success();
        }

        ParserResult constructor_def() {
            ParserResult result;
            vector<shared_ptr<Token>> arguments;

            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(current_t->start, current_t->end);
            node->set_type(NODE_CONSTRUCTOR_DEF);

            if(current_t->matches(TT_KEYWORD, KEYWORD_CONSTRUCTOR) == false) {
                return result.failure(create_syntax_error("'CONSTRUCTOR'", 1));
            }
            result.register_advance(advance());

            if(current_t->type != TT_LPAREN) {
                return result.failure(create_syntax_error("'('", 1));
            }
            result.register_advance(advance());

            bool identifier_errored_out = false;
            InvalidSyntaxError id_e;
            if(current_t->type == TT_IDENFIFIER) {
                arguments.push_back(current_t);
                result.register_advance(advance());

                while(current_t->type == TT_COMMA) {
                    result.register_advance(advance());
                    
                    if(current_t->type != TT_IDENFIFIER) {
                        id_e = create_syntax_error("identifier", 1);
                        identifier_errored_out = true;
                        break;
                    }

                    arguments.push_back(current_t);
                    result.register_advance(advance());
                }

                if(identifier_errored_out == false && current_t->type != TT_RPAREN) {
                    id_e = create_syntax_error("',' or ')'", 1);
                    identifier_errored_out = true;
                }
            } else {
                if(current_t->type != TT_RPAREN) {
                    id_e = create_syntax_error("identifier or ')'", 1);
                    identifier_errored_out = true;
                }
            }
            if(identifier_errored_out == true) {
                return result.failure(id_e);
            }
            result.register_advance(advance());

            if(current_t->type == TT_LCBRACKET) {
                result.register_advance(advance());

                ParserResult expr = result.register_result(statements());
                if(result.state == -1) { return result; }

                if(current_t->type != TT_RCBRACKET) {
                    return result.failure(create_syntax_error("'}'", 1));
                }
                result.register_advance(advance());

                node->set_func_def_expression_result(expr.node);
            } else {
                return result.failure(create_syntax_error("'{'", 1));
            }

            node->set_end(current_t->start);
            node->set_func_def_argument_tokens_result(arguments);
            
            result.set_node(node);
            return result.success();
        }

        ParserResult class_def() {
            ParserResult result;

            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(current_t->start, current_t->end);
            node->set_type(NODE_CLASS_DEF);

            if(current_t->matches(TT_KEYWORD, KEYWORD_CLASS) == false) {
                return result.failure(create_syntax_error("'CLASS'", 1));
            }
            result.register_advance(advance());

            if(current_t->matches(TT_KEYWORD, object_keywords)) {
                shared_ptr<Token> var_name = current_t;
                result.register_advance(advance());
                node->set_token(var_name);
            } else {
                return result.failure(create_syntax_error("identifier", 1));
            }

            if(current_t->type == TT_LCBRACKET) {
                result.register_advance(advance());

                ParserResult expr = result.register_result(statements());
                if(result.state == -1) { return result; }

                if(current_t->type != TT_RCBRACKET) {
                    return result.failure(create_syntax_error("'}'", 1));
                }
                result.register_advance(advance());

                node->set_class_def_expression_result(expr.node);
            } else {
                return result.failure(create_syntax_error("'{'", 1));
            }
            
            node->set_end(current_t->start);
            
            result.set_node(node);
            return result.success();
        }

        ParserResult object_def() {
            ParserResult result;

            shared_ptr<Node> node = make_shared<Node>();
            node->set_pos(current_t->start, current_t->end);
            node->set_type(NODE_OBJECT_NEW);

            if(current_t->matches(TT_KEYWORD, KEYWORD_NEW) == false) {
                return result.failure(create_syntax_error("'NEW'", 1));
            }
            result.register_advance(advance());

            if(current_t->matches(TT_KEYWORD, object_keywords) == false) {
                return result.failure(create_syntax_error("a classname", 1));
            }
            node->set_token(current_t);
            result.register_advance(advance());

            if(current_t->type != TT_LPAREN) {
                return result.failure(create_syntax_error("'('", 1));
            }
            result.register_advance(advance());

            list<shared_ptr<Node>> arguments;
            
            if(current_t->type == TT_RPAREN) {
                result.register_advance(advance());
            } else {
                ParserResult arg = result.register_result(expression());
                if(result.state == -1) {
                    return result.failure(create_syntax_error("')' or an argument"));
                }
                arguments.push_back(arg.node);

                while(current_t->type == TT_COMMA) {
                    result.register_advance(advance());

                    ParserResult arg_1 = result.register_result(expression());
                    if(result.state == -1) { break; }
                    arguments.push_back(arg_1.node);
                }
                if(result.state == -1) { return result; }

                if(current_t->type != TT_RPAREN) {
                    return result.failure(create_syntax_error("',' or ')'"));
                }
                result.register_advance(advance());
            }

            node->set_end(current_t->start);
            node->set_object_argument_nodes_result(arguments);
            
            result.set_node(node);
            return result.success();
        }

        InvalidSyntaxError create_syntax_error(string exp, int extra = 0) {
            InvalidSyntaxError e(current_t->start, current_t->end, "Expected " + exp);
            e.extra = extra;
            return e;
        }
};