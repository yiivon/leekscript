#ifndef VM_STANDARD_STRINGSTD_HPP_
#define VM_STANDARD_STRINGSTD_HPP_

#include "../Module.hpp"

namespace ls {

class LSString;
class LSNumber;

class StringSTD : public Module {
public:
	StringSTD();
	virtual ~StringSTD();

	static Compiler::value lt(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value div(Compiler& c, std::vector<Compiler::value> args);

	static LSString* add_int(LSString* s, int i);
	static LSString* add_bool(LSString* s, bool i);
	static LSString* add_real(LSString* s, double i);

	static LSString* replace(LSString*, LSString*, LSString*);
	static LSValue* v1_replace(LSString* string, LSString* from, LSString* to);
	
	static Compiler::value fold_fun(Compiler& c, std::vector<Compiler::value> args, bool);
};

}

#endif
