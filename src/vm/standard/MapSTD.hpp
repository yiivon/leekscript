#ifndef VM_STANDARD_MAPSTD_HPP_
#define VM_STANDARD_MAPSTD_HPP_

#include "../Module.hpp"

namespace ls {

class MapSTD : public Module {
public:
	MapSTD();

	static Compiler::value look_any_any(Compiler&, std::vector<Compiler::value>);
	static Compiler::value look_any_real(Compiler&, std::vector<Compiler::value>);
	static Compiler::value look_any_int(Compiler&, std::vector<Compiler::value>);
	static Compiler::value look_int_any(Compiler&, std::vector<Compiler::value>);
	static Compiler::value look_int_real(Compiler&, std::vector<Compiler::value>);
	static Compiler::value look_int_int(Compiler&, std::vector<Compiler::value>);

	static Compiler::value fold_left(Compiler&, std::vector<Compiler::value>);
	static Compiler::value fold_right(Compiler&, std::vector<Compiler::value>);
};

}

#endif
