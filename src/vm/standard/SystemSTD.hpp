#ifndef SYSTEMSTD_HPP
#define SYSTEMSTD_HPP

#include "../Module.hpp"

namespace ls {

class SystemSTD : public Module {
public:
	SystemSTD();

	static void throw1(int type, char* function, size_t line);
	static void throw2(void** ex, char* function, size_t line);
};

}

#endif
