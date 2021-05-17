#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;

int runIntegerTest(Runner* runner, string name, string code, int value) {
    RunResult result = runner->run(name, code);
    if(result.interpreterResult.res_int.value == value) {
        printf("Result of %s - %s (%s)\n", result.lexer.fileName.c_str(), "PASSED", (result.interpreterResult.res_int.repr() + "/" + to_string(value)).c_str());
        return 1;
    } else {
        printf("Result of %s - %s (%s)\n", result.lexer.fileName.c_str(), "FAILED", (result.interpreterResult.res_int.repr() + "/" + to_string(value)).c_str());
        return 0;
    }
}

int runFloatTest(Runner* runner, string name, string code, float value) {
    RunResult result = runner->run(name, code);
    if(result.interpreterResult.res_float.value == value) {
        printf("Result of %s - %s (%s)\n", result.lexer.fileName.c_str(), "PASSED", (result.interpreterResult.res_float.repr() + "/" + to_string(value)).c_str());
        return 1;
    } else {
        printf("Result of %s - %s (%s)\n", result.lexer.fileName.c_str(), "FAILED", (result.interpreterResult.res_float.repr() + "/" + to_string(value)).c_str());
        return 0;
    }
}

bool runTests(Runner* runner) {
    int totalTests = 0, passed = 0;

    printf("----------\n");
    passed += runIntegerTest(runner, "Integer Math 1 (1+1)", "1+1", 2);
    passed += runIntegerTest(runner, "Integer Math 2 (1*2)", "1*2", 2);
    passed += runFloatTest(runner, "Integer Math 3 (2/1)", "2/1", 2);
    passed += runIntegerTest(runner, "Integer Math 4 (2-1)", "2-1", 1);
    passed += runIntegerTest(runner, "Integer Math 5 (10+10+10)", "10+10+10", 30);
    passed += runIntegerTest(runner, "Integer Math 6 (10+(10+10))", "10+(10+10)", 30);
    passed += runIntegerTest(runner, "Integer Math 7 (10*10+10)", "10*10+10", 110);
    passed += runIntegerTest(runner, "Integer Math 8 (10*(10+10))", "10*(10+10)", 200);
    passed += runIntegerTest(runner, "Integer Math 9 (-10+10)", "-10+10", 0);
    passed += runIntegerTest(runner, "Integer Math 10 (-10+(10+10))", "-10+(10+10)", 10);
    passed += runIntegerTest(runner, "Integer Math 11 (-10*10)", "-10*10", -100);
    passed += runIntegerTest(runner, "Integer Math 12 (-10*(10+10))", "-10*(10+10)", -200);
    totalTests += 12;

    printf("----------\n");
    passed += runFloatTest(runner, "Float Math 1 (1.5+2)", "1.5+2", 3.5);
    passed += runFloatTest(runner, "Float Math 2 (5*2.5)", "5*2.5", 12.5);
    passed += runFloatTest(runner, "Float Math 3 (10.25/5)", "10.25/5", 2.05);
    passed += runFloatTest(runner, "Float Math 4 (7-5.25)", "7-5.25", 1.75);
    totalTests += 4;

    printf("----------\n");
    passed += runIntegerTest(runner, "Extra Math 1 (2^4)", "16", 16);
    passed += runIntegerTest(runner, "Extra Math 2 ((5*2)^2)", "(5*2)^2", 100);
    passed += runIntegerTest(runner, "Extra Math 3 (5%2)", "5%2", 1);
    passed += runIntegerTest(runner, "Extra Math 4 ((5*2)%3)", "(5*2)%3", 1);
    totalTests += 4;

    printf("----------\n");
    passed += runIntegerTest(runner, "Variables 1 (NULL)", "NULL", 0);
    passed += runIntegerTest(runner, "Variables 2 (VAR a = 5)", "VAR a = 5", 5);
    passed += runIntegerTest(runner, "Variables 3 (VAR b = 10)", "VAR b = 10", 10);
    passed += runIntegerTest(runner, "Variables 4 (a + b)", "a + b", 15);
    passed += runIntegerTest(runner, "Variables 5 (a * b)", "a * b", 50);
    passed += runIntegerTest(runner, "Variables 6 (VAR c = a + b * 10)", "VAR c = a + b * 10", 105);
    totalTests += 6;

    printf("----------\n");
    passed += runIntegerTest(runner, "Logical 1 (1 == 1)", "1 == 1", 1);
    passed += runIntegerTest(runner, "Logical 2 (1 < 2)", "1 < 2", 1);
    passed += runIntegerTest(runner, "Logical 3 (2 > 1)", "2 > 1", 1);
    passed += runIntegerTest(runner, "Logical 4 (1 != 1)", "1 != 1", 0);
    passed += runIntegerTest(runner, "Logical 5 (2 <= 2)", "2 <= 2", 1);
    passed += runIntegerTest(runner, "Logical 6 (2 >= 2)", "2 >= 2", 1);
    passed += runIntegerTest(runner, "Logical 7 (1 AND 1)", "1 AND 1", 1);
    passed += runIntegerTest(runner, "Logical 8 (1 AND 0)", "1 AND 0", 0);
    passed += runIntegerTest(runner, "Logical 9 (1 OR 0)", "1 OR 0", 1);
    passed += runIntegerTest(runner, "Logical 10 (0 OR 0)", "0 OR 0", 0);
    passed += runIntegerTest(runner, "Logical 11 (NOT 1)", "NOT 1", 0);
    totalTests += 11;

    printf("----------\n");
    printf("%i PASSED, %i FAILED", passed, (totalTests - passed));
    return passed < totalTests;
}