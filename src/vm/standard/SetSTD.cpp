#include "SetSTD.hpp"
#include "../value/LSSet.hpp"

namespace ls {
	
const std::_Rb_tree_node_base* iterator_end(LSSet<int>* set) {
	return set->end()._M_node;
}
LSSet<int>::iterator iterator_inc(LSSet<int>::iterator it) {
	it++;
	return it;
}

SetSTD::SetSTD() : Module("Set") {

	LSSet<LSValue*>::clazz = clazz;
	LSSet<int>::clazz = clazz;
	LSSet<double>::clazz = clazz;

	/*
	 * Constructor
	 */
	constructor_({
		{Type::tmp_set(Type::any()), {Type::integer()}, (void*) &LSSet<LSValue*>::constructor, Method::NATIVE},
		{Type::tmp_set(Type::real()), {Type::integer()}, (void*) &LSSet<double>::constructor, Method::NATIVE},
		{Type::tmp_set(Type::integer()), {Type::integer()}, (void*) &LSSet<int>::constructor, Method::NATIVE},
	});

	/*
	 * Operators
	 */
	operator_("in", {
		{Type::const_set(), Type::any(), Type::boolean(), (void*) &LSSet<LSValue*>::in_v, {}, Method::NATIVE},
		{Type::const_set(), Type::integer(), Type::boolean(), (void*) &in_any},
		{Type::const_set(Type::real()), Type::real(), Type::boolean(), (void*) &LSSet<double>::in_v, {}, Method::NATIVE},
		{Type::const_set(Type::integer()), Type::integer(), Type::boolean(), (void*) &LSSet<int>::in_v, {}, Method::NATIVE}
	});
	operator_("+=", {
		{Type::set(), Type::const_any(), Type::any(), (void*) &set_add_eq, {new WillStoreMutator()}},
		{Type::set(Type::real()), Type::const_real(), Type::array(Type::real()), (void*) &LSSet<double>::add_eq_double, {new WillStoreMutator()}, Method::NATIVE},
		{Type::set(Type::integer()), Type::const_integer(), Type::array(Type::integer()), (void*) &LSSet<int>::add_eq_int, {new WillStoreMutator()}, Method::NATIVE}
	});

	/*
	 * Methods
	 */
	method("size", {
		{Type::integer(), {Type::const_set()}, (void*) &LSSet<LSValue*>::ls_size, Method::NATIVE},
		{Type::integer(), {Type::const_set(Type::real())}, (void*) &LSSet<double>::ls_size, Method::NATIVE},
		{Type::integer(), {Type::const_set(Type::integer())}, (void*) &LSSet<int>::ls_size, Method::NATIVE},
	});
	method("insert", {
		{Type::boolean(), {Type::set(Type::any()), Type::any()}, (void*) &LSSet<LSValue*>::ls_insert_ptr, Method::NATIVE},
		{Type::boolean(), {Type::set(Type::any()), Type::any()}, (void*) &insert_any},
		{Type::boolean(), {Type::set(Type::real()), Type::real()}, (void*) &insert_real},
		{Type::boolean(), {Type::set(Type::integer()), Type::integer()}, (void*) &insert_int},
	});
	method("clear", {
		{Type::set(), {Type::set(Type::any())}, (void*) &LSSet<LSValue*>::ls_clear, Method::NATIVE},
		{Type::set(Type::real()), {Type::set(Type::real())}, (void*) &LSSet<double>::ls_clear, Method::NATIVE},
		{Type::set(Type::integer()), {Type::set(Type::integer())}, (void*) &LSSet<int>::ls_clear, Method::NATIVE},
	});
	method("erase", {
		{Type::boolean(), {Type::set(), Type::any()}, (void*) &LSSet<LSValue*>::ls_erase, Method::NATIVE},
		{Type::boolean(), {Type::set(Type::real()), Type::real()}, (void*) &LSSet<double>::ls_erase, Method::NATIVE},
		{Type::boolean(), {Type::set(Type::integer()), Type::integer()}, (void*) &LSSet<int>::ls_erase, Method::NATIVE},
	});
	method("contains", {
		{Type::boolean(), {Type::const_set(), Type::any()}, (void*) &LSSet<LSValue*>::ls_contains, Method::NATIVE},
		{Type::boolean(), {Type::const_set(Type::real()), Type::real()}, (void*) &LSSet<double>::ls_contains, Method::NATIVE},
		{Type::boolean(), {Type::const_set(Type::integer()), Type::integer()}, (void*) &LSSet<int>::ls_contains, Method::NATIVE},
	});

	/** Internal **/
	method("vinsert", {
		{{}, {Type::const_set(Type::any()), Type::any()}, (void*) &LSSet<LSValue*>::vinsert, Method::NATIVE},
		{{}, {Type::const_set(Type::real()), Type::real()}, (void*) &LSSet<double>::vinsert, Method::NATIVE},
		{{}, {Type::const_set(Type::integer()), Type::integer()}, (void*) &LSSet<int>::vinsert, Method::NATIVE},
	});
	method("iterator_end", {
		{Type::set().iterator(), {Type::set()}, (void*) iterator_end, Method::NATIVE}
	});
	method("iterator_inc", {
		{Type::set().iterator(), {Type::set().iterator()}, (void*) iterator_inc, Method::NATIVE}
	});
}

Compiler::value SetSTD::in_any(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::any(), {args[0], c.insn_to_any(args[1])}, (void*) +[](LSValue* x, LSValue* y) {
		return x->in(y);
	});
}

Compiler::value SetSTD::set_add_eq(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::any(), {args[0], c.insn_to_any(args[1])}, (void*) +[](LSValue* x, LSValue* y) {
		return x->add_eq(y);
	});
}

Compiler::value SetSTD::insert_any(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::boolean(), {args[0], c.insn_to_any(args[1])}, (void*) &LSSet<LSValue*>::ls_insert);
}
Compiler::value SetSTD::insert_real(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::boolean(), {args[0], c.to_real(args[1])}, (void*) &LSSet<double>::ls_insert);
}
Compiler::value SetSTD::insert_int(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::boolean(), {args[0], c.to_int(args[1])}, (void*) &LSSet<int>::ls_insert);
}

}
