#include "Documentation.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../standard/ArraySTD.hpp"
#include "../standard/NumberSTD.hpp"
#include "../standard/ObjectSTD.hpp"
#include "../standard/StringSTD.hpp"
using namespace std;

Documentation::Documentation() {}

Documentation::~Documentation() {}

void Documentation::generate(ostream& os) {

	string lang = "fr";

	vector<Module*> modules;
	modules.push_back(new ArraySTD());
	modules.push_back(new NumberSTD());
	modules.push_back(new ObjectSTD());
	modules.push_back(new StringSTD());

	os << "{";

	for (unsigned m = 0; m < modules.size(); ++m) {
		if (m > 0) os << ",";

		Module* mod = modules[m];
		ifstream f;
		f.open("vm/doc/" + mod->name + "_" + lang + ".json");
		stringstream j;
		j << f.rdbuf();
		string str = j.str();
		f.close();

		// Erase tabs
		str.erase(std::remove(str.begin(), str.end(), '	'), str.end());

		// Parse json
		char *endptr;
		JsonValue value(JSON_NULL, nullptr);
		JsonAllocator allocator;
		jsonParse((char*) str.c_str(), &endptr, &value, allocator);

		mod->generate_doc(os, value);
	}

	os << "}\n";
}
