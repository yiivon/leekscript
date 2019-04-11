#ifndef SYSTEMSTD_HPP
#define SYSTEMSTD_HPP

#include "../Module.hpp"

namespace ls {

class SystemSTD : public Module {
public:
	SystemSTD();

	static void throw_(int type, char* function, size_t line);
};

}

#endif
