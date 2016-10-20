#ifndef OPERATOR_PROTOTYPES_HPP
#define OPERATOR_PROTOTYPES_HPP

#include "../vm/Type.hpp"
#include "../vm/Module.hpp"
#include <string>

namespace ls {

class OperatorPrototypes {
public:

	class Prototype {
	public:
		std::string name;
		int precedence;
		bool left_assignment = false;
		bool right_assignment = false;
	};

	class Method {
	public:
		Type type;
		void* addr;
		bool native;
		Method(std::initializer_list<Type> args, Type return_type, void* addr, bool native = false);
		static bool NATIVE;
	};

	/** Create all available operators prototypes */
	static void initialize();

	/** Add an operator prototype */
	static void prototype(std::initializer_list<std::string> names, int precedence, bool left_assignment,
		bool right_assignment, std::initializer_list<Method>);

	/*
	 * Operators methods
	 */
	static bool not_pointer(LSValue*);
	static jit_value_t not_value(Compiler& c, jit_value_t v);

	static jit_value_t add_real_real(Compiler& c, jit_value_t a, jit_value_t b);
	static jit_value_t add_equals_real_real(Compiler& c, jit_value_t a, jit_value_t b);
};

}

#endif
