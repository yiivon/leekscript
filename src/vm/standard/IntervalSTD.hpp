#ifndef INTERVAL_STD_HPP
#define INTERVAL_STD_HPP

#include "../Module.hpp"

namespace ls {

class IntervalSTD : public Module {
public:
	IntervalSTD();
	virtual ~IntervalSTD();

	static Compiler::value map(Compiler& c, std::vector<Compiler::value> args, bool);
};

}

#endif
