#include "SetSTD.hpp"
#include "../value/LSSet.hpp"

using namespace std;

namespace ls {

SetSTD::SetSTD() : Module("Set") {

	LSSet<LSValue*>::clazz = clazz;
	LSSet<int>::clazz = clazz;
	LSSet<double>::clazz = clazz;

	/*
	 * Operators
	 */
	operator_("in", {
		{Type::CONST_SET, Type::POINTER, Type::BOOLEAN, (void*) &LSSet<LSValue*>::in_v, {}, Method::NATIVE},
		{Type::CONST_REAL_SET, Type::REAL, Type::BOOLEAN, (void*) &LSSet<double>::in_v, {}, Method::NATIVE},
		{Type::CONST_INT_SET, Type::INTEGER, Type::BOOLEAN, (void*) &LSSet<int>::in_v, {}, Method::NATIVE}
	});
	operator_("+=", {
		{Type::SET, Type::CONST_POINTER, Type::POINTER, (void*) &set_add_eq, {new WillStoreMutator()}},
		{Type::REAL_SET, Type::CONST_REAL, Type::REAL_ARRAY, (void*) &LSSet<double>::add_eq_double, {new WillStoreMutator()}, Method::NATIVE},
		{Type::INT_SET, Type::CONST_INTEGER, Type::INT_ARRAY, (void*) &LSSet<int>::add_eq_int, {new WillStoreMutator()}, Method::NATIVE}
	});

	/*
	 * Methods
	 */
	method("size", {
		{Type::INTEGER, {Type::CONST_SET}, (void*) &LSSet<LSValue*>::ls_size, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_REAL_SET}, (void*) &LSSet<double>::ls_size, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_SET}, (void*) &LSSet<int>::ls_size, Method::NATIVE},
	});
	method("insert", {
		{Type::BOOLEAN, {Type::SET, Type::POINTER}, (void*) &LSSet<LSValue*>::ls_insert, Method::NATIVE},
		{Type::BOOLEAN, {Type::REAL_SET, Type::REAL}, (void*) &LSSet<double>::ls_insert, Method::NATIVE},
		{Type::BOOLEAN, {Type::INT_SET, Type::INTEGER}, (void*) &LSSet<int>::ls_insert, Method::NATIVE},
	});
	method("clear", {
		{Type::SET, {Type::SET}, (void*) &LSSet<LSValue*>::ls_clear, Method::NATIVE},
		{Type::REAL_SET, {Type::REAL_SET}, (void*) &LSSet<double>::ls_clear, Method::NATIVE},
		{Type::INT_SET, {Type::INT_SET}, (void*) &LSSet<int>::ls_clear, Method::NATIVE},
	});
	method("erase", {
		{Type::BOOLEAN, {Type::SET, Type::POINTER}, (void*) &LSSet<LSValue*>::ls_erase, Method::NATIVE},
		{Type::BOOLEAN, {Type::REAL_SET, Type::REAL}, (void*) &LSSet<double>::ls_erase, Method::NATIVE},
		{Type::BOOLEAN, {Type::INT_SET, Type::INTEGER}, (void*) &LSSet<int>::ls_erase, Method::NATIVE},
	});
	method("contains", {
		{Type::BOOLEAN, {Type::CONST_SET, Type::POINTER}, (void*) &LSSet<LSValue*>::ls_contains, Method::NATIVE},
		{Type::BOOLEAN, {Type::CONST_REAL_SET, Type::REAL}, (void*) &LSSet<double>::ls_contains, Method::NATIVE},
		{Type::BOOLEAN, {Type::CONST_INT_SET, Type::INTEGER}, (void*) &LSSet<int>::ls_contains, Method::NATIVE},
	});
}

Compiler::value SetSTD::set_add_eq(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::POINTER, {args[0], c.insn_to_any(args[1])}, (void*) +[](LSValue* x, LSValue* y) {
		return x->add_eq(y);
	});
}

}
