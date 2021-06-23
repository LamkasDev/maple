#pragma once
using namespace std;

int run_integer_test(Runner runner, string name, string code, int value) {
    RunResult result = runner.run(name, code);
    if(result.interpreterResult.res_int.value == value) {
        printf("Result of %s - %s (%s)\n", result.lexer.file_name.c_str(), "PASSED", (result.interpreterResult.res_int.repr() + "/" + to_string(value)).c_str());
        return 1;
    } else {
        printf("Result of %s - %s (%s)\n", result.lexer.file_name.c_str(), "FAILED", (result.interpreterResult.res_int.repr() + "/" + to_string(value)).c_str());
        return 0;
    }
}

int run_float_test(Runner runner, string name, string code, float value) {
    RunResult result = runner.run(name, code);
    if(result.interpreterResult.res_float.value == value) {
        printf("Result of %s - %s (%s)\n", result.lexer.file_name.c_str(), "PASSED", (result.interpreterResult.res_float.repr() + "/" + to_string(value)).c_str());
        return 1;
    } else {
        printf("Result of %s - %s (%s)\n", result.lexer.file_name.c_str(), "FAILED", (result.interpreterResult.res_float.repr() + "/" + to_string(value)).c_str());
        return 0;
    }
}

int run_string_test(Runner runner, string name, string code, string value) {
    RunResult result = runner.run(name, code);
    if(result.interpreterResult.res_string.value == value) {
        printf("Result of %s - %s (%s)\n", result.lexer.file_name.c_str(), "PASSED", (result.interpreterResult.res_string.repr() + "/" + value).c_str());
        return 1;
    } else {
        printf("Result of %s - %s (%s)\n", result.lexer.file_name.c_str(), "FAILED", (result.interpreterResult.res_string.repr() + "/" + value).c_str());
        return 0;
    }
}

bool run_tests(Runner runner) {
    int total_tests = 0, passed = 0;

    printf("----------\n");
    passed += run_integer_test(runner, "Integer Math 1 (1+1)", "1+1", 2);
    passed += run_integer_test(runner, "Integer Math 2 (1*2)", "1*2", 2);
    passed += run_float_test(runner, "Integer Math 3 (2/1)", "2/1", 2);
    passed += run_integer_test(runner, "Integer Math 4 (2-1)", "2-1", 1);
    passed += run_integer_test(runner, "Integer Math 5 (10+10+10)", "10+10+10", 30);
    passed += run_integer_test(runner, "Integer Math 6 (10+(10+10))", "10+(10+10)", 30);
    passed += run_integer_test(runner, "Integer Math 7 (10*10+10)", "10*10+10", 110);
    passed += run_integer_test(runner, "Integer Math 8 (10*(10+10))", "10*(10+10)", 200);
    passed += run_integer_test(runner, "Integer Math 9 (-10+10)", "-10+10", 0);
    passed += run_integer_test(runner, "Integer Math 10 (-10+(10+10))", "-10+(10+10)", 10);
    passed += run_integer_test(runner, "Integer Math 11 (-10*10)", "-10*10", -100);
    passed += run_integer_test(runner, "Integer Math 12 (-10*(10+10))", "-10*(10+10)", -200);
    total_tests += 12;

    printf("----------\n");
    passed += run_float_test(runner, "Float Math 1 (1.5+2)", "1.5+2", 3.5);
    passed += run_float_test(runner, "Float Math 2 (5*2.5)", "5*2.5", 12.5);
    passed += run_float_test(runner, "Float Math 3 (10.25/5)", "10.25/5", 2.05);
    passed += run_float_test(runner, "Float Math 4 (7-5.25)", "7-5.25", 1.75);
    total_tests += 4;

    printf("----------\n");
    passed += run_integer_test(runner, "Extra Math 1 (2^4)", "16", 16);
    passed += run_integer_test(runner, "Extra Math 2 ((5*2)^2)", "(5*2)^2", 100);
    passed += run_integer_test(runner, "Extra Math 3 (5%2)", "5%2", 1);
    passed += run_integer_test(runner, "Extra Math 4 ((5*2)%3)", "(5*2)%3", 1);
    total_tests += 4;

    printf("----------\n");
    passed += run_integer_test(runner, "Variables 1 (NULL)", "NULL", 0);
    passed += run_integer_test(runner, "Variables 2 (VAR a = 5)", "VAR a = 5", 5);
    passed += run_integer_test(runner, "Variables 3 (VAR b = 10)", "VAR b = 10", 10);
    passed += run_integer_test(runner, "Variables 4 (a + b)", "a + b", 15);
    passed += run_integer_test(runner, "Variables 5 (a * b)", "a * b", 50);
    passed += run_integer_test(runner, "Variables 6 (VAR c = a + b * 10)", "VAR c = a + b * 10", 105);
    total_tests += 6;

    printf("----------\n");
    passed += run_integer_test(runner, "Logical 1 (1 == 1)", "1 == 1", 1);
    passed += run_integer_test(runner, "Logical 2 (1 < 2)", "1 < 2", 1);
    passed += run_integer_test(runner, "Logical 3 (2 > 1)", "2 > 1", 1);
    passed += run_integer_test(runner, "Logical 4 (1 != 1)", "1 != 1", 0);
    passed += run_integer_test(runner, "Logical 5 (2 <= 2)", "2 <= 2", 1);
    passed += run_integer_test(runner, "Logical 6 (2 >= 2)", "2 >= 2", 1);
    passed += run_integer_test(runner, "Logical 7 (1 AND 1)", "1 AND 1", 1);
    passed += run_integer_test(runner, "Logical 8 (1 AND 0)", "1 AND 0", 0);
    passed += run_integer_test(runner, "Logical 9 (1 OR 0)", "1 OR 0", 1);
    passed += run_integer_test(runner, "Logical 10 (0 OR 0)", "0 OR 0", 0);
    passed += run_integer_test(runner, "Logical 11 (NOT 1)", "NOT 1", 0);
    total_tests += 11;

    printf("----------\n");
    passed += run_integer_test(runner, "Conditional 1 (IF (1) { 0 })", "IF (1) { 0 }", 0);
    passed += run_integer_test(runner, "Conditional 2 (IF (0) { 1 } ELSE { 0 })", "IF (0) { 1 } ELSE { 0 }", 0);
    passed += run_integer_test(runner, "Conditional 3 (IF (a == 5) { 1 } ELSE { 0 })", "IF (a == 5) { 1 } ELSE { 0 }", 1);
    passed += run_integer_test(runner, "Conditional 4 (IF (a AND b) { 1 } ELSE { 0 })", "IF (a AND b) { 1 } ELSE { 0 }", 1);
    passed += run_integer_test(runner, "Conditional 5 (IF (NOT a) { 1 } ELIF (NOT b) { 1 } ELSE { 0 })", "IF (NOT a) { 1 } ELIF (NOT b) { 1 } ELSE { 0 }", 0);
    passed += run_integer_test(runner, "Conditional 6 (IF (1) { VAR d = 10 } ELSE { VAR d = 5 })", "IF (1) { VAR d = 10 } ELSE { VAR d = 5 }", 10);
    total_tests += 6;

    printf("----------\n");
    passed += run_integer_test(runner, "Loops 1 (VAR l_1 = 1; FOR (VAR i = 1) TO 6 { VAR l_1 = l_1 * i };)", "VAR l_1 = 1; FOR (VAR i = 1) TO 6 { VAR l_1 = l_1 * i };", 1);
    passed += run_integer_test(runner, "Loops 2 (VAR l_2 = 1; FOR (VAR i = -1) TO -6 STEP -1 { VAR l_2 = l_2 * i })", "VAR l_2 = 1; FOR (VAR i = -1) TO -6 STEP -1 { VAR l_2 = l_2 * i };", 1);
    passed += run_integer_test(runner, "Loops 3 (l_1)", "l_1", 120);
    passed += run_integer_test(runner, "Loops 4 (l_2)", "l_2", -120);
    passed += run_integer_test(runner, "Loops 5 (VAR l_3 = 1; WHILE l_3 < 100 { VAR l_3 = l_3 + 1 };)", "VAR l_3 = 1; WHILE l_3 < 100 { VAR l_3 = l_3 + 1 };", 1);
    passed += run_integer_test(runner, "Loops 6 (l_3)", "l_3", 100);
    total_tests += 6;

    printf("----------\n");
    passed += run_integer_test(runner, "Functions 1 (FUNC f_a() => 1)", "FUNC f_a() => 1", 0);
    passed += run_integer_test(runner, "Functions 2 (FUNC f_b(num) => num)", "FUNC f_b(num) => num", 0);
    passed += run_integer_test(runner, "Functions 3 (FUNC f_c(num, num2) => num + num2)", "FUNC f_c(num, num2) => num + num2", 0);
    passed += run_integer_test(runner, "Functions 4 (FUNC f_d() => f_c(20, 25))", "FUNC f_d() => f_c(20, 25)", 0);
    passed += run_integer_test(runner, "Functions 5 (f_a())", "f_a()", 1);
    passed += run_integer_test(runner, "Functions 6 (f_b(5))", "f_b(5)", 5);
    passed += run_integer_test(runner, "Functions 7 (f_c(5, 10))", "f_c(5, 10)", 15);
    passed += run_integer_test(runner, "Functions 8 (f_d())", "f_d()", 45);
    total_tests += 8;

    printf("----------\n");
    passed += run_string_test(runner, "Strings 1 (\"a\")", "\"a\"", "a");
    passed += run_string_test(runner, "Strings 2 (\"a\" + \"b\" + \"c\")", "\"a\" + \"b\" + \"c\"", "abc");
    total_tests += 2;

    printf("----------\n");
    passed += run_integer_test(runner, "Multi-line statements 1 (VAR a = 1; VAR b = 0; IF (a) { VAR a = 0; VAR b = 1; } ELSE { VAR a = 1; VAR b = 0; };)", "VAR a = 1; VAR b = 0; IF (a) { VAR a = 0; VAR b = 1; } ELSE { VAR a = 1; VAR b = 0; };", 1);
    passed += run_integer_test(runner, "Multi-line statements 2 (a)", "a", 0);
    passed += run_integer_test(runner, "Multi-line statements 3 (b)", "b", 1);
    total_tests += 3;

    printf("----------\n");
    passed += run_integer_test(runner, "Special 1 (RETURN 20)", "RETURN 20", 20);
    passed += run_integer_test(runner, "Special 2 (VAR a = 0; FOR (VAR i = 0) TO 10 { IF(i == 4) { BREAK; }; VAR a = a + 1; }; RETURN a;)", "VAR a = 0; FOR (VAR i = 0) TO 10 { IF(i == 4) { BREAK; }; VAR a = a + 1; }; RETURN a;", 4);
    passed += run_integer_test(runner, "Special 3 (VAR a = 0; FOR (VAR i = 0) TO 10 { IF(i == 4) { CONTINUE; }; VAR a = a + 1; }; RETURN a;)", "VAR a = 0; FOR (VAR i = 0) TO 10 { IF(i == 4) { CONTINUE; }; VAR a = a + 1; }; RETURN a;", 9);
    total_tests += 3;

    printf("----------\n");
    passed += run_integer_test(runner, "Lists 1 (VAR list_a = [1,2,3])", "VAR list_a = [1,2,3]", 0);
    passed += run_integer_test(runner, "Lists 2 (VAR list_b = [1.25,2.5,3.0])", "VAR list_b = [1.25,2.5,3.0]", 0);
    passed += run_integer_test(runner, "Lists 3 (VAR list_c = [\"b\",\"c\",\"d\"])", "VAR list_c = [\"b\",\"c\",\"d\"]", 0);
    passed += run_integer_test(runner, "Lists 4 (VAR list_d = [[1,2,3],[4,5,6],[7,8,9]])", "VAR list_d = [[1,2,3],[4,5,6],[7,8,9]]", 0);
    passed += run_integer_test(runner, "Lists 5 (VAR list_e = [NEW OBJECT(), NEW OBJECT(), NEW OBJECT()])", "VAR list_e = [NEW OBJECT(), NEW OBJECT(), NEW OBJECT()]", 0);
    total_tests += 5;

    printf("----------\n");
    passed += run_integer_test(runner, "List Functions 1 (VAR a = 0; FOREACH(el IN list_a) { VAR a = a + el };)", "VAR a = 0; FOREACH(el IN list_a) { VAR a = a + el };", 0);
    passed += run_integer_test(runner, "List Functions 2 (VAR b = 0; FOREACH(el IN list_a) { VAR b = b + el };)", "VAR b = 0; FOREACH(el IN list_b) { VAR b = b + el };", 0);
    passed += run_integer_test(runner, "List Functions 3 (VAR c = \"a\"; FOREACH(el IN list_a) { VAR c = c + el };)", "VAR c = \"a\"; FOREACH(el IN list_c) { VAR c = c + el };", 0);
    passed += run_integer_test(runner, "List Functions 4 (a)", "a", 6);
    passed += run_float_test(runner, "List Functions 5 (b)", "b", 6.75);
    passed += run_string_test(runner, "List Functions 6 (c)", "c", "abcd");
    total_tests += 6;

    printf("----------\n");
    passed += run_integer_test(runner, "Built-in Functions 1 (parse_int(\"100\"))", "parse_int(\"100\")", 100);
    passed += run_float_test(runner, "Built-in Functions 2 (parse_float(\"100.55\"))", "parse_float(\"100.55\")", 100.55);
    total_tests += 2;

    printf("----------\n");
    passed += run_string_test(runner, "Built-in String Functions 1 (\"abcd\".char_at(1))", "\"abcd\".char_at(1)", "b");
    passed += run_string_test(runner, "Built-in String Functions 2 (\"ab\".concat(\"cd\"))", "\"ab\".concat(\"cd\")", "abcd");
    passed += run_integer_test(runner, "Built-in String Functions 3 (\"abcd\".starts_with(\"ab\"))", "\"abcd\".starts_with(\"ab\")", 1);
    passed += run_integer_test(runner, "Built-in String Functions 4 (\"abcd\".ends_with(\"cd\"))", "\"abcd\".ends_with(\"cd\")", 1);
    passed += run_integer_test(runner, "Built-in String Functions 5 (\"abcd\".includes(\"c\"))", "\"abcd\".includes(\"c\")", 1);
    passed += run_integer_test(runner, "Built-in String Functions 6 (\"abcd\".index_of(\"c\"))", "\"abcd\".index_of(\"c\")", 2);
    passed += run_integer_test(runner, "Built-in String Functions 7 (\"abcdcd\".last_index_of(\"c\"))", "\"abcdcd\".last_index_of(\"c\")", 4);
    passed += run_string_test(runner, "Built-in String Functions 8 (\"abcd\".replace(\"cd\", \"11\"))", "\"abcd\".replace(\"cd\", \"11\")", "ab11");
    passed += run_string_test(runner, "Built-in String Functions 9 (\"1111\".replace_all(\"1\", \"2\"))", "\"1111\".replace_all(\"1\", \"2\")", "2222");
    /*passed += run_string_test(runner, "Built-in String Functions 10 (VAR a = \"-\")", "VAR a = \"-\"", "-");
    passed += run_integer_test(runner, "Built-in String Functions 11 (VAR list_a = \"ab.cd\".split(\".\"))", "VAR list_a = \"ab.cd\".split(\".\")", 0);
    passed += run_string_test(runner, "Built-in String Functions 12 (FOREACH(el IN list_a) { VAR a = a + el })", "FOREACH(el IN list_a) { VAR a = a + el }", "-abcd");*/
    passed += run_string_test(runner, "Built-in String Functions 13 (\"abcd\".substring(1,3))", "\"abcd\".substring(1,3)", "bc");
    passed += run_string_test(runner, "Built-in String Functions 14 (\"aBCd\".to_lower_case())", "\"aBCd\".to_lower_case()", "abcd");
    passed += run_string_test(runner, "Built-in String Functions 15 (\"aBCd\".to_upper_case())", "\"aBCd\".to_upper_case()", "ABCD");
    passed += run_string_test(runner, "Built-in String Functions 16 (\" ababa \".trim())", "\" ababa \".trim()", "ababa");
    passed += run_string_test(runner, "Built-in String Functions 17 (\" ababa \".trim_start())", "\" ababa \".trim_start()", "ababa ");
    passed += run_string_test(runner, "Built-in String Functions 18 (\" ababa \".trim_end())", "\" ababa \".trim_end()", " ababa");
    passed += run_integer_test(runner, "Built-in String Functions 19 (\"ababa\".length())", "\"ababa\".length()", 5);
    total_tests += 16;

    printf("----------\n");
    passed += run_integer_test(runner, "Built-in List Functions 1 (VAR list_a = [1,2,3]; VAR list_b = [4,5,6]; VAR list_c = list_a.concat(list_b);)", "VAR list_a = [1,2,3]; VAR list_b = [4,5,6]; VAR list_c = list_a.concat(list_b);", 0);
    passed += run_integer_test(runner, "Built-in List Functions 2 (VAR a = 0; FOREACH(el IN list_c) { VAR a = a + el };)", "VAR a = 0; FOREACH(el IN list_c) { VAR a = a + el };", 0);
    passed += run_integer_test(runner, "Built-in List Functions 3 (a)", "a", 21);
    passed += run_integer_test(runner, "Built-in List Functions 4 (VAR list_a = [1,2,3]; list_a.push(4);)", "VAR list_a = [1,2,3]; list_a.push(4);", 0);
    passed += run_integer_test(runner, "Built-in List Functions 5 (VAR a = 0; FOREACH(el IN list_a) { VAR a = a + el };)", "VAR a = 0; FOREACH(el IN list_a) { VAR a = a + el };", 0);
    passed += run_integer_test(runner, "Built-in List Functions 6 (a)", "a", 10);
    passed += run_integer_test(runner, "Built-in List Functions 7 (VAR list_a = [1,2,3])", "VAR list_a = [1,2,3]", 0);
    passed += run_integer_test(runner, "Built-in List Functions 8 (list_a.pop())", "list_a.pop()", 3);
    passed += run_integer_test(runner, "Built-in List Functions 9 (VAR list_a = [1,2,3])", "VAR list_a = [1,2,3]", 0);
    passed += run_integer_test(runner, "Built-in List Functions 10 (list_a.shift())", "list_a.shift()", 1);
    passed += run_integer_test(runner, "Built-in List Functions 11 (VAR list_a = [1,2,3]; list_a.unshift(4);)", "VAR list_a = [1,2,3]; list_a.unshift(4);", 0);
    passed += run_integer_test(runner, "Built-in List Functions 12 (VAR a = 0; FOREACH(el IN list_a) { VAR a = a + el };)", "VAR a = 0; FOREACH(el IN list_a) { VAR a = a + el };", 0);
    passed += run_integer_test(runner, "Built-in List Functions 13 (a)", "a", 10);
    total_tests += 13;

    printf("----------\n");
    passed += run_integer_test(runner, "Objects 1 (VAR obj = NEW OBJECT())", "VAR obj = NEW OBJECT()", 0);
    passed += run_integer_test(runner, "Objects 2 (obj.a = 1)", "obj.a = 1", 0);
    passed += run_integer_test(runner, "Objects 3 (obj.b = \"b\")", "obj.b = \"b\"", 0);
    passed += run_integer_test(runner, "Objects 4 (obj.c = [1,2,3])", "obj.c = [1,2,3]", 0);
    passed += run_integer_test(runner, "Objects 5 (obj.d = FUNC d() { RETURN \"d\" })", "obj.d = FUNC d() { RETURN \"d\" }", 0);
    passed += run_integer_test(runner, "Objects 6 (obj.e = NEW OBJECT())", "obj.e = NEW OBJECT()", 0);
    passed += run_integer_test(runner, "Objects 7 (obj.e.a = 1)", "obj.e.a = 1", 0);
    passed += run_integer_test(runner, "Objects 8 (obj.a)", "obj.a", 1);
    passed += run_string_test(runner, "Objects 9 (obj.b)", "obj.b", "b");
    passed += run_string_test(runner, "Objects 10 (obj.d())", "obj.d()", "d");
    passed += run_integer_test(runner, "Objects 11 (obj.e.a)", "obj.e.a", 1);
    total_tests += 11;

    printf("----------\n");
    passed += run_integer_test(runner, "Classes 1 (CLASS class {})", "CLASS class {}", 0);
    total_tests += 1;

    printf("----------\n");
    passed += run_string_test(runner, "Equals math 1 (VAR a = \"a\"; VAR a += \"b\";)", "VAR a = \"a\"; VAR a += \"b\";", "ab");
    passed += run_integer_test(runner, "Equals math 2 (VAR a = 2; VAR a -= 1;)", "VAR a = 2; VAR a -= 1;", 1);
    total_tests += 2;

    printf("----------\n");
    printf("%i PASSED, %i FAILED", passed, (total_tests - passed));
    return passed < total_tests;
}