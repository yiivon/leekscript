#ifndef CALLABLE_HPP_
#define CALLABLE_HPP_

#include "../../type/Type.hpp"

namespace ls {

class Value;
class CallableVersion;
class SemanticAnalyzer;

class Callable {
public:
	std::vector<const CallableVersion*> versions;

	Callable() {}
	Callable(std::vector<const CallableVersion*> versions) : versions(versions) {}
	Callable(std::initializer_list<const CallableVersion*> versions) : versions(versions) {}

	const CallableVersion* resolve(SemanticAnalyzer* analyzer, std::vector<Type> arguments) const;
	bool is_compatible(int argument_count);
	void add_version(const CallableVersion* version);
};

}

namespace std {
	std::ostream& operator << (std::ostream&, const ls::Callable*);
}

#endif