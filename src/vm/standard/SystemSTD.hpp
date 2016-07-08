#ifndef SYSTEMSTD_HPP
#define SYSTEMSTD_HPP

#include "../VM.hpp"
#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "../../vm/Program.hpp"
#include "../Module.hpp"

namespace ls {

class SystemSTD : public Module {
public:
	SystemSTD();
};

void System_print(LSValue* v);
void System_print_int(int v);
void System_print_long(long v);
void System_print_bool(bool v);
void System_print_float(double v);

}

#endif
