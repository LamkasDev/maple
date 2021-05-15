#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "runner/runner.cpp"
#include "tests/tests.cpp"
using namespace std;

void printResult(RunResult result) {
    string r = result.interpreterResult.type == NODE_INT ? to_string(result.interpreterResult.res_int.value) : to_string(result.interpreterResult.res_float.value);
    printf("Result - %s \n\n", ("[" + result.interpreterResult.type + "] " +  r).c_str());
}

int main(int argc, char** argv) {
    if(argc >= 2) {
        if(argc >= 3) {
            if(string(argv[1]) == "-run") {
                ifstream file;
                file.open(string(argv[2]));
                if(!file) {
                    printf("File doesn't exist-");
                    return 0;
                } else {
                    stringstream stream;
                    stream << file.rdbuf();
                    string contents = stream.str();

                    RunResult result = run("<file>", contents);
                    printResult(result);

                    return 1;
                }
            }
        }

        if(string(argv[1]) == "-tests") {
            runTests();
            return 1;
        }
    }

    printf("[Maple 0.0.1] C++ based programming language\n");
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
            runTests();
            break;
        }
        
        RunResult result = run("<stdin>", _s);
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

    return 1;
}