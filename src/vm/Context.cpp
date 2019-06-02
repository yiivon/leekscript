#include "Context.hpp"
#include <sstream>
#include <string.h>
#include <vector>
#include "../../lib/json.hpp"

namespace ls {

Context::Context() {}

Context::Context(std::string ctx) {

	Json value = Json::parse(ctx);

	for (Json::iterator it = value.begin(); it != value.end(); ++it) {
		//vars.insert({it.key(), ls::LSValue::parse(it.value())});
	}
}

Context::~Context() {}

void Context::add_variable(char* name, void* v, const Type* type) {
	vars.insert({ name, { v, type } });
}

}

namespace std {
	std::ostream& operator << (std::ostream& os, const ls::Context* context) {
		os << "{";
		int i = 0;
		for (const auto& v : context->vars) {
			if (i++ > 0) os << ", ";
			os << v.first << ": " << v.second.value << " " << v.second.type;
		}
		os << "}";
		return os;
	}
}
