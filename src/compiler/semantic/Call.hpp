#ifndef CALL_HPP_
#define CALL_HPP_

#include "../../type/Type.hpp"
#include <vector>

namespace ls {

class SemanticAnalyzer;
class CallableVersion;

class Call {
public:
	std::vector<const CallableVersion*> versions;

	Call() {}
	Call(std::initializer_list<const CallableVersion*> versions) : versions(versions) {}
	void add_version(const CallableVersion* v);
	const CallableVersion* resolve(SemanticAnalyzer* analyzer, std::vector<Type> arguments) const;
	
};

}

#endif