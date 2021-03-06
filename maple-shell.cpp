using namespace std;

const string VERSION = "1.1.0";

void print_result(RunResult result, bool prints_result, bool prints_debug) {
    if(result.makeTokensResult.state == -1) {
        printf("%s \n\n", result.makeTokensResult.e.as_string(result.file_contents).c_str());
    } else if(result.parserResult.state == -1) {
        printf("%s \n\n", result.parserResult.e.as_string(result.file_contents).c_str());
    } else if(result.interpreterResult.state == -1) {
        printf("%s \n", result.interpreterResult.e.as_string(result.file_contents).c_str());
    } else {
        if(prints_debug == true) {
            printf("[Debug] Tokens - %s \n", print_tree(result.makeTokensResult.tokens).c_str());
            printf("[Debug] Nodes - %s \n", ("(" + result.parserResult.node->repr() + ")").c_str());
        }
        if(prints_result == true) {
            printf("Result - %s \n\n", ("[" + result.interpreterResult.type + "] " +  result.interpreterResult.repr()).c_str());
        }
    }
}

int run(int argc, char** argv) {
    Runner runner;

    bool debug_mode = argc >= 2 && string(argv[1]) == "-d";
    runner.debug_mode = debug_mode;
    RunResult pkg_result = runner.load_packages();
    if(pkg_result.state == -1) {
        print_result(pkg_result, false, debug_mode);
        exit(1);
    }

    string file_contents = "";
    if(argc >= 2) {
        if(string(argv[1]) == "-d") {
            printf("[Debug] Running Maple in Debug Mode...\n");
        } else if(string(argv[1]) == "-tests") {
            exit(run_tests(runner));
        } else if(string(argv[1]) == "-v") {
            printf("Maple version %s (Built by LamkasDev)-", VERSION.c_str());
            exit(0);
        } else if(string(argv[1]) == "-run") {
            if(argc >= 3) {
                file_contents = string(argv[2]);
                RunResult result = runner.run_file(file_contents);
                if(result.state == -1) {
                    print_result(result, false, debug_mode);
                    exit(1);
                } else if(result.state == -2) {
                    printf("File doesn't exist-");
                    exit(1);
                } else {
                    exit(0);
                } 
            } else {
                printf("No file name was specified-");
                exit(1);
            }
        } else {
            printf("Unknown argument '%s'", string(argv[1]).c_str());
            exit(1);
        }
    }

    printf("[Maple %s] C++ based programming language\n", VERSION.c_str());
    printf("Commands: run(), tests(), exit()\n");

    char s[8192];
    while(true) {
        printf("%s", ">>> ");
        scanf("%[^\n]%*c", &s);
        file_contents = string(s);

        if(file_contents == "tests()") {
            run_tests(runner);
            break;
        } else if(file_contents == "run()") {
            char s2[1024];
            string _s2;

            printf("%s", "Enter file location: ");
            scanf("%[^\n]%*c", &s2);
            _s2 = string(s2);

            RunResult result = runner.run_file(_s2);
            if(result.state == -1) {
                print_result(result, false, debug_mode);
            } else if(result.state == -2) {
                printf("File doesn't exist-");
            } else {
                print_result(result, true, debug_mode);
            }
            continue;
        } else if(file_contents == "exit()") {
            printf("Bye bye~");
            break;
        }
        
        RunResult result = runner.run("<stdin>", file_contents);
        print_result(result, true, debug_mode);
    }

    exit(0);
}