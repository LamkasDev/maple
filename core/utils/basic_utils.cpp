#pragma once
using namespace std;

bool in_array(const string &value, const vector<string> &array){
    return find(array.begin(), array.end(), value) != array.end();
}

