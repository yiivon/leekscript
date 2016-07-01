#include "Util.hpp"

#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <sstream>
#include <fstream>
#include <queue>

using namespace std;

string Util::read_file(string file) {
	ifstream ifs(file.data());
	string content = string((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
	ifs.close();
	return content;
}

