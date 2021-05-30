#include <stdio.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <stdexcept>
#include <memory>
#include <functional>

#include "runner/runner.cpp"
#include "tests/tests.cpp"
using namespace std;

const string VERSION = "0.9.0";

void print_result(RunResult result, bool prints_result, bool prints_debug) {
    if(result.makeTokensResult.state == -1) {
        printf("%s \n\n", result.makeTokensResult.e.as_string().c_str());
    } else if(result.parserResult.state == -1) {
        printf("%s \n\n", result.parserResult.e.as_string().c_str());
    } else if(result.interpreterResult.state == -1) {
        printf("%s \n", result.interpreterResult.e.as_string().c_str());
    } else {
        if(prints_debug == true) {
            printf("Tokens - %s \n", print_tree(result.makeTokensResult.tokens).c_str());
            printf("Nodes - %s \n", ("(" + result.parserResult.node->repr() + ")").c_str());
        }
        if(prints_result == true) {
            printf("Result - %s \n\n", ("[" + result.interpreterResult.type + "] " +  result.interpreterResult.repr()).c_str());
        }
    }
}

bool run_file(Runner runner, string location, bool prints_result, bool prints_debug) {
    ifstream file;
    file.open(location);
    if(!file) {
        printf("File doesn't exist-");
        return 1;
    } else {
        stringstream stream;
        stream << file.rdbuf();
        string contents = stream.str();

        RunResult result = runner.run("<file>", contents);
        print_result(result, prints_result, prints_debug);
        return 0;
    }
}

int main(int argc, char** argv) {
    Runner runner;
    bool debug_mode = false;
    if(argc >= 2) {
        if(string(argv[1]) == "-run") {
            if(argc >= 3) {
                exit(run_file(runner, string(argv[2]), false, false));
            } else {
                printf("No file name was specified-");
                exit(1);
            }
        } else if(string(argv[1]) == "-tests") {
            exit(run_tests(runner));
        } else if(string(argv[1]) == "-v") {
            printf("Maple version %s (Built by LamkasDev)-", VERSION.c_str());
            exit(0);
        } else if(string(argv[1]) == "-d") {
            debug_mode = true;
        } else {
            printf("Unknown argument '%s'", string(argv[1]).c_str());
            exit(1);
        }
    }

    printf("[Maple %s] C++ based programming language\n", VERSION.c_str());
    printf("Commands: run(), tests(), exit()\n");

    char s[8192];
    string _s;
    while(true) {
        printf("%s", ">>> ");
        scanf("%[^\n]%*c", &s);
        _s = string(s);

        if(_s == "tests()") {
            run_tests(runner);
            break;
        } else if(_s == "run()") {
            char s2[1024];
            string _s2;

            printf("%s", "Enter file location: ");
            scanf("%[^\n]%*c", &s2);
            _s2 = string(s2);

            run_file(runner, _s2, true, debug_mode);
            continue;
        } else if(_s == "exit()") {
            printf("Bye bye~");
            break;
        }
        
        RunResult result = runner.run("<stdin>", _s);
        print_result(result, true, debug_mode);
    }

    exit(0);
}