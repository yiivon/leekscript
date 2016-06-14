#include "Context.hpp"
#include <sstream>
#include <string.h>
#include <vector>
#include "VM.hpp"
#include "../lib/gason.h"
using namespace std;

namespace ls {

std::vector<std::string> split(const std::string &s, string delim) {
	vector<string> v;
	auto start = 0U;
	auto end = s.find(delim);
	while (end != std::string::npos) {
		v.push_back(s.substr(start, end - start));
		start = end + delim.length();
		end = s.find(delim, start);
	}
	return v;
}

Context::Context(std::string ctx) {

	char *endptr;
	JsonValue value;
	JsonAllocator allocator;
	jsonParse((char*) ctx.c_str(), &endptr, &value, allocator);

	for (auto i : value) {
		string name =  i->key;
		JsonValue var = i->value;
		vars.insert(pair<string, LSValue*>(name, ls::LSValue::parse(var)));
	}
}

Context::~Context() {}

}

