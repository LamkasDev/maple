#pragma once

#include "../core/lexer/lexer.cpp"
#include "../core/parser/parser.cpp"
#include "../core/lexer/lexer.cpp"
#include "../core/interpreter/interpreter.cpp"
#include "../core/utils/other_utils.cpp"
#include "run_result.cpp"

using namespace std;

class Runner {
    public:
        shared_ptr<Interpreter> interpreter = nullptr;
        vector<string> object_keywords;
        bool debug_mode = false;

        Runner() {
            interpreter = make_shared<Interpreter>();
            object_keywords.push_back("OBJECT");
        }

        RunResult load_packages() {
            #if MAPLE_OS == 1
                string packages_path = "usr/bin/maple/packages";
            #endif
            #if MAPLE_OS == 3
                string packages_path = "C:\\Program Files\\Maple\\packages";
            #endif

            RunResult result;
            if(debug_mode == true) { printf("[Debug] Loading default packages...\n"); }
            vector<string> default_packages = get_files(packages_path);
            for (string package : default_packages) {
                if(debug_mode == true) { printf("[Debug] Loading default package - '%s'...\n", package.c_str()); }
                
                result = run_file(packages_path + "\\" + package);
                if(result.state != 0) {
                    return result;
                }
            }

            return result;
        }

        RunResult run(string _file_name, string _file_contents) {
            RunResult result;
            result.set_file_contents(_file_contents);

            Lexer lexer(object_keywords, _file_name, _file_contents);
            result.set_lexer(lexer);
            MakeTokensResult makeTokensResult = lexer.make_tokens();
            result.set_make_tokens_result(makeTokensResult);
            if(makeTokensResult.state == -1) {
                result.state = -1;
                return result;
            }

            object_keywords = lexer.object_keywords;

            Parser parser(object_keywords, _file_name, makeTokensResult.tokens);
            ParserResult parserResult = parser.parse();
            result.set_parser_result(parserResult);
            if(parserResult.state == -1) {
                result.state = -1;
                return result;
            }

            interpreter->context->file_name = _file_name;

            InterpreterResult interpreterResult;
            interpreterResult = interpreter->visit_node(parserResult.node, interpreter->context);
            result.set_interpreter_result(interpreterResult);
            if(interpreterResult.state == -1) {
                result.state = -1;
                return result;
            }
            
            return result;
        }

        RunResult run_file(string location) {
            RunResult result;

            ifstream file;
            file.open(location);
            if(!file) {
                result.state = -2;
                return result;
            } else {
                stringstream stream;
                stream << file.rdbuf();
                string contents = stream.str();

                result = run("'" + location + "'", contents);
                return result;
            }
        }
};