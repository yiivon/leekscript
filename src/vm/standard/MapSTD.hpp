#ifndef VM_STANDARD_MAPSTD_HPP_
#define VM_STANDARD_MAPSTD_HPP_

#include "../Module.hpp"

namespace ls {

class MapSTD : public Module {
public:
	MapSTD();

	static Compiler::value insert_any_any(Compiler&, std::vector<Compiler::value>);
	static Compiler::value insert_any_real(Compiler&, std::vector<Compiler::value>);
	static Compiler::value insert_any_int(Compiler&, std::vector<Compiler::value>);
	static Compiler::value insert_int_any(Compiler&, std::vector<Compiler::value>);
	static Compiler::value insert_int_real(Compiler&, std::vector<Compiler::value>);
	static Compiler::value insert_int_int(Compiler&, std::vector<Compiler::value>);
};

}

#endif
