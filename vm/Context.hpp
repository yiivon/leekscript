#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <map>
#include <string>
#include "LSValue.hpp"

class Context {
public:

	Context(std::string ctx);
	virtual ~Context();

	std::map<std::string, LSValue*> vars;
};

#endif
