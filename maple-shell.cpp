#include <stdio.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "runner/runner.cpp"
#include "tests/tests.cpp"
using namespace std;

const string VERSION = "0.5";

void printResult(RunResult result) {
    string r = result.interpreterResult.type == NODE_INT ? to_string(result.interpreterResult.res_int.value) : to_string(result.interpreterResult.res_float.value);
    printf("Result - %s \n\n", ("[" + result.interpreterResult.type + "] " +  r).c_str());
}

int main(int argc, char** argv) {
    Runner* runner = new Runner();
    runner->initialize_global_symbol_table();

    if(argc >= 2) {
        if(string(argv[1]) == "-run") {
            if(argc >= 3) {
                ifstream file;
                file.open(string(argv[2]));
                if(!file) {
                    printf("File doesn't exist-");
                    exit(1);
                } else {
                    stringstream stream;
                    stream << file.rdbuf();
                    string contents = stream.str();

                    RunResult result = runner->run("<file>", contents);
                    printResult(result);

                    exit(0);
                }
            } else {
                printf("No file name was specified-");
                exit(1);
            }
        } else if(string(argv[1]) == "-tests") {
            exit(runTests(runner));
        } else if(string(argv[1]) == "-v") {
            printf("Maple version %s (Built by LamkasDev)-", VERSION.c_str());
            exit(0);
        } else {
            printf("Unknown argument '%s'", string(argv[1]).c_str());
            exit(1);
        }
    }

    printf("[Maple %s] C++ based programming language\n", VERSION.c_str());
    printf("Commands: exit(), tests()\n");

    char s[512];
    string _s;
    while(true) {
        printf("%s", ">>> ");
        scanf("%[^\n]%*c", &s);
        _s = string(s);

        if(_s == "exit()") {
            printf("Bye bye~");
            break;
        } else if(_s == "tests()") {
            runTests(runner);
            break;
        }
        
        RunResult result = runner->run("<stdin>", _s);
        if(result.makeTokensResult.state == -1) {
            printf("%s \n\n", result.makeTokensResult.e.as_string().c_str());
        } else if(result.parserResult.state == -1) {
            printf("%s \n\n", result.parserResult.e.as_string().c_str());
        } else if(result.interpreterResult.state == -1) {
            printf("%s \n", result.interpreterResult.e.as_string().c_str());
        } else {
            printf("Tokens - %s \n", print_tree(result.makeTokensResult.tokens).c_str());
            printf("Nodes - %s \n", ("(" + print_node(result.parserResult) + ")").c_str());
            printResult(result);
        }
    }

    exit(0);
}