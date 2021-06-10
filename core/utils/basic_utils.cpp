#pragma once
using namespace std;

bool in_array(const string &value, const vector<string> &array){
    return find(array.begin(), array.end(), value) != array.end();
}

string repeat_string(const string& input, unsigned num) {
    string ret;
    ret.reserve(input.size() * num);
    while (num--) {
        ret += input;
    }

    return ret;
}

void replace_string(string& str, const string& from, const string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == string::npos) {
        return;
    }

    str.replace(start_pos, from.length(), to);
    return;
}

void replace_string_all(string& str, const string& from, const string& to) {
    if(from.empty()) {
        return;
    }

    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

vector<string> split_string(const string& str, const string& delim) {
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == string::npos) { pos = str.length(); }
        string token = str.substr(prev, pos-prev);
        if (!token.empty()) { tokens.push_back(token); }
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

string to_lower_case_string(string& str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

string to_upper_case_string(string& str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

void ltrim_string(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !isspace(ch);
    }));
}

void rtrim_string(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !isspace(ch);
    }).base(), s.end());
}

void trim_string(string &s) {
    ltrim_string(s);
    rtrim_string(s);
}