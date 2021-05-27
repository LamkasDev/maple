#include <stdio.h>
using namespace std;

int run_integer_test(Runner runner, string name, string code, int value) {
    RunResult result = runner.run(name, code);
    if(result.interpreterResult.res_int.value == value) {
        printf("Result of %s - %s (%s)\n", result.lexer.fileName.c_str(), "PASSED", (result.interpreterResult.res_int.repr() + "/" + to_string(value)).c_str());
        return 1;
    } else {
        printf("Result of %s - %s (%s)\n", result.lexer.fileName.c_str(), "FAILED", (result.interpreterResult.res_int.repr() + "/" + to_string(value)).c_str());
        return 0;
    }
}

int run_float_test(Runner runner, string name, string code, float value) {
    RunResult result = runner.run(name, code);
    if(result.interpreterResult.res_float.value == value) {
        printf("Result of %s - %s (%s)\n", result.lexer.fileName.c_str(), "PASSED", (result.interpreterResult.res_float.repr() + "/" + to_string(value)).c_str());
        return 1;
    } else {
        printf("Result of %s - %s (%s)\n", result.lexer.fileName.c_str(), "FAILED", (result.interpreterResult.res_float.repr() + "/" + to_string(value)).c_str());
        return 0;
    }
}

int run_string_test(Runner runner, string name, string code, string value) {
    RunResult result = runner.run(name, code);
    if(result.interpreterResult.res_string.value == value) {
        printf("Result of %s - %s (%s)\n", result.lexer.fileName.c_str(), "PASSED", (result.interpreterResult.res_string.repr() + "/" + value).c_str());
        return 1;
    } else {
        printf("Result of %s - %s (%s)\n", result.lexer.fileName.c_str(), "FAILED", (result.interpreterResult.res_string.repr() + "/" + value).c_str());
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
    passed += run_integer_test(runner, "Loops 1 (VAR l_1 = 1)", "VAR l_1 = 1", 1);
    passed += run_integer_test(runner, "Loops 2 (VAR l_2 = 1)", "VAR l_2 = 1", 1);
    passed += run_integer_test(runner, "Loops 3 (VAR l_3 = 1)", "VAR l_3 = 1", 1);
    passed += run_integer_test(runner, "Loops 4 (FOR (VAR i = 1) TO 6 { VAR l_1 = l_1 * i })", "FOR (VAR i = 1) TO 6 { VAR l_1 = l_1 * i }", 0);
    passed += run_integer_test(runner, "Loops 5 (FOR (VAR i = -1) TO -6 STEP -1 { VAR l_2 = l_2 * i })", "FOR (VAR i = -1) TO -6 STEP -1 { VAR l_2 = l_2 * i }", 0);
    passed += run_integer_test(runner, "Loops 6 (l_1)", "l_1", 120);
    passed += run_integer_test(runner, "Loops 7 (l_2)", "l_2", -120);
    passed += run_integer_test(runner, "Loops 8 (WHILE l_3 < 100 { VAR l_3 = l_3 + 1 })", "WHILE l_3 < 100 { VAR l_3 = l_3 + 1 }", 0);
    passed += run_integer_test(runner, "Loops 9 (l_3)", "l_3", 100);
    total_tests += 9;

    printf("----------\n");
    passed += run_integer_test(runner, "Functions 1 (FUNC f_a() -> 1)", "FUNC f_a() -> 1", 0);
    passed += run_integer_test(runner, "Functions 2 (FUNC f_b(num) -> num)", "FUNC f_b(num) -> num", 0);
    passed += run_integer_test(runner, "Functions 3 (FUNC f_c(num, num2) -> num + num2)", "FUNC f_c(num, num2) -> num + num2", 0);
    passed += run_integer_test(runner, "Functions 4 (FUNC f_d() -> f_c(20, 25))", "FUNC f_d() -> f_c(20, 25)", 0);
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
    passed += run_integer_test(runner, "Multi-line statements 1 (VAR a = 1; VAR b = 0; IF (a) { VAR a = 0; VAR b = 1 } ELSE { VAR a = 1; VAR b = 0 })", "VAR a = 1; VAR b = 0; IF (a) { VAR a = 0; VAR b = 1 } ELSE { VAR a = 1; VAR b = 0 }", 1);
    passed += run_integer_test(runner, "Multi-line statements 2 (a)", "a", 0);
    passed += run_integer_test(runner, "Multi-line statements 3 (b)", "b", 1);
    total_tests += 3;

    printf("----------\n");
    passed += run_integer_test(runner, "Special 1 (RETURN 20)", "RETURN 20", 20);
    passed += run_integer_test(runner, "Special 2 (VAR a = 0; FOR (VAR i = 0) TO 10 { IF(i == 4) { BREAK }; VAR a = a + 1 }; RETURN a)", "VAR a = 0; FOR (VAR i = 0) TO 10 { IF(i == 4) { BREAK }; VAR a = a + 1 }; RETURN a", 4);
    passed += run_integer_test(runner, "Special 3 (VAR a = 0; FOR (VAR i = 0) TO 10 { IF(i == 4) { CONTINUE }; VAR a = a + 1 }; RETURN a)", "VAR a = 0; FOR (VAR i = 0) TO 10 { IF(i == 4) { CONTINUE }; VAR a = a + 1 }; RETURN a", 9);
    total_tests += 3;

    printf("----------\n");
    passed += run_integer_test(runner, "Lists 4 (VAR list_a = [1,2,3])", "VAR list_a = [1,2,3]", 0);
    passed += run_integer_test(runner, "Lists 5 (VAR list_b = [1.25,2.5,3.0])", "VAR list_b = [1.25,2.5,3.0]", 0);
    passed += run_integer_test(runner, "Lists 5 (VAR list_c = [\"b\",\"c\",\"d\"])", "VAR list_c = [\"b\",\"c\",\"d\"]", 0);
    total_tests += 3;

    printf("----------\n");
    passed += run_integer_test(runner, "List Functions 1 (VAR a = 0)", "VAR a = 0", 0);
    passed += run_integer_test(runner, "List Functions 2 (VAR b = 0)", "VAR b = 0", 0);
    passed += run_string_test(runner, "List Functions 3 (VAR c = \"a\")", "VAR c = \"a\"", "a");
    passed += run_integer_test(runner, "List Functions 4 (FOREACH(el IN list_a) { VAR a = a + el })", "FOREACH(el IN list_a) { VAR a = a + el }", 0);
    passed += run_integer_test(runner, "List Functions 5 (FOREACH(el IN list_a) { VAR b = b + el })", "FOREACH(el IN list_b) { VAR b = b + el }", 0);
    passed += run_integer_test(runner, "List Functions 6 (FOREACH(el IN list_a) { VAR c = c + el })", "FOREACH(el IN list_c) { VAR c = c + el }", 0);
    passed += run_integer_test(runner, "List Functions 7 (a)", "a", 6);
    passed += run_float_test(runner, "List Functions 8 (b)", "b", 6.75);
    passed += run_string_test(runner, "List Functions 9 (c)", "c", "abcd");
    total_tests += 9;

    printf("----------\n");
    printf("%i PASSED, %i FAILED", passed, (total_tests - passed));
    return passed < total_tests;
}