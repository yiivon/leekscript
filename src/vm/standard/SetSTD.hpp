#ifndef VM_STANDARD_SETSTD_HPP_
#define VM_STANDARD_SETSTD_HPP_

#include "../Module.hpp"

namespace ls {

class SetSTD : public Module {
public:
	SetSTD();

	static Compiler::value in_any(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value set_add_eq(Compiler& c, std::vector<Compiler::value> args);
};

}

#endif
