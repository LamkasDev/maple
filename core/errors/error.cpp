#pragma once
using namespace std;

const string ERROR_UNKNOWN = "UNKNOWN";
const string ERROR_DEFAULT = "DEFAULT";
const string ERROR_RUNTIME = "RUNTIME";

class Error {
    public:
        Position start;
        Position end;

        string type = ERROR_UNKNOWN;
        string name = "";
        string details = "";
        string file_name = "";
        string traceback = "";
        int extra = 0;

        Error() {
            
        }

        Error(string _file_name, Position _start, Position _end, string _name, string _details) {
            file_name = _file_name;
            start = _start;
            end = _end;
            name = _name;
            details = _details;
        }

        string as_string(string file_contents) {
            if(type != ERROR_RUNTIME) {
                string s = name + ": " + details + "\n";
                s += "File " + file_name + ", line " + to_string(start.line + 1) + ", col " + to_string(start.column) + "-" + to_string(end.column);
		        s += "\n\n" + print_error(file_contents, start, end);
                return s;
            } else {
                string s = traceback;
                s += name + ": " + details + "\n";
                return s;
            }
        }

        string print_error(string text, Position pos_start, Position pos_end) {
            string result = "";

            int idx_start = text.rfind("\n", pos_start.index); if(idx_start < 0) { idx_start = 0; }
            int idx_end = text.find('\n', idx_start + 1); if(idx_end < 0) { idx_end = text.length(); }

            int line_count = pos_end.line - pos_start.line + 1;
            for(int i = 0; i < line_count; i++) {
                string line = text.substr(idx_start, idx_end - idx_start);
                int col_start = pos_start.column; if(i != 0) { col_start = 0; }
                int col_end = pos_end.column; if(i != line_count - 1) { col_end = line.length() - 1; }

                result += line + "\n";
                result += string(col_start, ' ') + string(col_end - col_start, '^');

                idx_start = idx_end;
                idx_end = text.find("\n", idx_start + 1);
                if(idx_end < 0) { idx_end = text.length(); }
            }
            
            result.erase(remove(result.begin(), result.end(), '\t'), result.end());
            return result;
        }
};