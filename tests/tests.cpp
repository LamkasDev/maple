#include <stdio.h>
#include <iostream>
#include <string>
#include "../maple.cpp"
using namespace std;

int runIntegerTest(string name, string code, int value) {
    RunResult result = run(name, code);
    if(result.interpreterResult.res_int.value == value) {
        printf("Result of %s - %s (%s)\n", result.lexer.fileName.c_str(), "PASSED", (result.interpreterResult.res_int.repr() + "/" + to_string(value)).c_str());
        return 1;
    } else {
        printf("Result of %s - %s (%s)\n", result.lexer.fileName.c_str(), "FAILED", (result.interpreterResult.res_int.repr() + "/" + to_string(value)).c_str());
        return 0;
    }
}

int runFloatTest(string name, string code, float value) {
    RunResult result = run(name, code);
    if(result.interpreterResult.res_float.value == value) {
        printf("Result of %s - %s (%s)\n", result.lexer.fileName.c_str(), "PASSED", (result.interpreterResult.res_float.repr() + "/" + to_string(value)).c_str());
        return 1;
    } else {
        printf("Result of %s - %s (%s)\n", result.lexer.fileName.c_str(), "FAILED", (result.interpreterResult.res_float.repr() + "/" + to_string(value)).c_str());
        return 0;
    }
}

void runTests() {
    int totalTests = 0, passed = 0;

    printf("----------\n");
    passed += runIntegerTest("Integer Math 1 (1+1)", "1+1", 2);
    passed += runIntegerTest("Integer Math 2 (1*2)", "1*2", 2);
    passed += runIntegerTest("Integer Math 3 (2/1)", "2/1", 2);
    passed += runIntegerTest("Integer Math 4 (2-1)", "2-1", 1);
    passed += runIntegerTest("Integer Math 5 (10+10+10)", "10+10+10", 30);
    passed += runIntegerTest("Integer Math 6 (10+(10+10))", "10+(10+10)", 30);
    passed += runIntegerTest("Integer Math 7 (10*10+10)", "10*10+10", 110);
    passed += runIntegerTest("Integer Math 8 (10*(10+10))", "10*(10+10)", 200);
    passed += runIntegerTest("Integer Math 9 (-10+10)", "-10+10", 0);
    passed += runIntegerTest("Integer Math 10 (-10+(10+10))", "-10+(10+10)", 10);
    passed += runIntegerTest("Integer Math 11 (-10*10)", "-10*10", -100);
    passed += runIntegerTest("Integer Math 12 (-10*(10+10))", "-10*(10+10)", -200);
    totalTests += 12;

    printf("----------\n");
    passed += runFloatTest("Float Math 1 (1.5+2)", "1.5+2", 3.5);
    passed += runFloatTest("Float Math 2 (5*2.5)", "5*2.5", 12.5);
    passed += runFloatTest("Float Math 3 (10.25/5)", "10.25/5", 2.05);
    passed += runFloatTest("Float Math 4 (7-5.25)", "7-5.25", 1.75);
    totalTests += 4;

    printf("----------\n");
    printf("%i PASSED, %i FAILED", passed, (totalTests - passed));
}