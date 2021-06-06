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

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../libs/httplib.h"
#include "../libs/dirent.h"
#include <Windows.h>

#define MAPLE_OS 3
#include "../runner/runner.cpp"
#include "../tests/tests.cpp"
#include "../maple-shell.cpp"
using namespace std;

int main(int argc, char** argv) {
   return run(argc, argv);
}