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
