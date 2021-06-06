#pragma once
using namespace std;

string print_tree(list<shared_ptr<Token>> tokens) {
    string s = "(";
    for (shared_ptr<Token> t : tokens) {
        s += t->repr() + ", ";
    }

    s = s.length() > 1 ? s.substr(0, s.length() - 2) + ")" : s;
    return s;
}

vector<string> get_files(string path) {
    vector<string> paths;

    struct dirent *entry;
    DIR *dir = opendir(path.c_str());
    
    if (dir == NULL) {
        return paths;
    }
    while ((entry = readdir(dir)) != NULL) {
        string file_name = string(entry->d_name);
        if(file_name != "." && file_name != "..") {
            paths.push_back(file_name);
        }
    }
    closedir(dir);

    return paths;
}