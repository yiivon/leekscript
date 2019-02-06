#include "Context.hpp"
#include <sstream>
#include <string.h>
#include <vector>
#include "../../lib/json.hpp"

namespace ls {

Context::Context(std::string ctx) {

	Json value = Json::parse(ctx);

	for (Json::iterator it = value.begin(); it != value.end(); ++it) {
		//vars.insert({it.key(), ls::LSValue::parse(it.value())});
	}
}

Context::~Context() {}

}
