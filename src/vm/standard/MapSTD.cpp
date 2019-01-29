#include "MapSTD.hpp"
#include "../value/LSMap.hpp"

using namespace std;

namespace ls {

int map_size(const LSMap<LSValue*,LSValue*>* map) {
	int r = map->size();
	if (map->refs == 0) delete map;
	return r;
}

MapSTD::MapSTD() : Module("Map") {

	LSMap<LSValue*, LSValue*>::clazz = clazz;
	LSMap<LSValue*, int>::clazz = clazz;
	LSMap<LSValue*, double>::clazz = clazz;
	LSMap<int, LSValue*>::clazz = clazz;
	LSMap<int, int>::clazz = clazz;
	LSMap<int, double>::clazz = clazz;
	LSMap<double, LSValue*>::clazz = clazz;
	LSMap<double, int>::clazz = clazz;
	LSMap<double, double>::clazz = clazz;

	operator_("in", {
		{Type::const_map(Type::any(), Type::any()), Type::any(), Type::boolean(), (void*) &LSMap<LSValue*, LSValue*>::in, {}, Method::NATIVE},
		{Type::const_map(Type::any(), Type::real()), Type::any(), Type::boolean(), (void*) &LSMap<LSValue*, double>::in, {}, Method::NATIVE},
		{Type::const_map(Type::any(), Type::integer()), Type::any(), Type::boolean(), (void*) &LSMap<LSValue*, int>::in, {}, Method::NATIVE},
		{Type::const_map(Type::real(), Type::any()), Type::real(), Type::boolean(), (void*) &LSMap<double, LSValue*>::in, {}, Method::NATIVE},
		{Type::const_map(Type::real(), Type::real()), Type::real(), Type::boolean(), (void*) &LSMap<double, double>::in, {}, Method::NATIVE},
		{Type::const_map(Type::real(), Type::integer()), Type::real(), Type::boolean(), (void*) &LSMap<double, int>::in, {}, Method::NATIVE},
		{Type::const_map(Type::integer(), Type::any()), Type::integer(), Type::boolean(), (void*) &LSMap<int, LSValue*>::in, {}, Method::NATIVE},
		{Type::const_map(Type::integer(), Type::real()), Type::integer(), Type::boolean(), (void*) &LSMap<int, double>::in, {}, Method::NATIVE},
		{Type::const_map(Type::integer(), Type::integer()), Type::long_(), Type::boolean(), (void*) &LSMap<int, int>::in, {}, Method::NATIVE},
	});

	method("size", {
		{Type::integer(), {Type::const_map(Type::any(), Type::any())}, (void*) map_size, Method::NATIVE},
		{Type::integer(), {Type::const_map(Type::any(), Type::real())}, (void*) map_size, Method::NATIVE},
		{Type::integer(), {Type::const_map(Type::any(), Type::integer())}, (void*) map_size, Method::NATIVE},
		{Type::integer(), {Type::const_map(Type::integer(), Type::any())}, (void*) map_size, Method::NATIVE},
		{Type::integer(), {Type::const_map(Type::integer(), Type::real())}, (void*) map_size, Method::NATIVE},
		{Type::integer(), {Type::const_map(Type::integer(), Type::integer())}, (void*) map_size, Method::NATIVE},
    });

	method("values", {
		{Type::array(Type::any()), {Type::const_map(Type::any(), Type::any())}, (void*) &LSMap<LSValue*, LSValue*>::values, Method::NATIVE},
		{Type::array(Type::real()), {Type::const_map(Type::any(), Type::real())}, (void*) &LSMap<LSValue*, double>::values, Method::NATIVE},
		{Type::array(Type::integer()), {Type::const_map(Type::any(), Type::integer())}, (void*) &LSMap<LSValue*, int>::values, Method::NATIVE},
		{Type::array(Type::any()), {Type::const_map(Type::real(), Type::any())}, (void*) &LSMap<double, LSValue*>::values, Method::NATIVE},
		{Type::array(Type::real()), {Type::const_map(Type::real(), Type::real())}, (void*) &LSMap<double, double>::values, Method::NATIVE},
		{Type::array(Type::integer()), {Type::const_map(Type::real(), Type::integer())}, (void*) &LSMap<double, int>::values, Method::NATIVE},
		{Type::array(Type::any()), {Type::const_map(Type::integer(), Type::any())}, (void*) &LSMap<int, LSValue*>::values, Method::NATIVE},
		{Type::array(Type::real()), {Type::const_map(Type::integer(), Type::real())}, (void*) &LSMap<int, double>::values, Method::NATIVE},
		{Type::array(Type::integer()), {Type::const_map(Type::integer(), Type::integer())}, (void*) &LSMap<int, int>::values, Method::NATIVE}
	});

	method("insert", {
		{Type::boolean(), {Type::map(Type::any(), Type::any()), Type::any(), Type::any()}, (void*) &insert_any_any},
		{Type::boolean(), {Type::map(Type::any(), Type::real()), Type::any(), Type::real()}, (void*) &insert_any_real},
		{Type::boolean(), {Type::map(Type::any(), Type::integer()), Type::any(), Type::integer()}, (void*) &insert_any_int},
		{Type::boolean(), {Type::map(Type::integer(), Type::any()), Type::integer(), Type::any()}, (void*) &insert_int_any},
		{Type::boolean(), {Type::map(Type::integer(), Type::real()), Type::integer(), Type::real()}, (void*) &insert_int_real},
		{Type::boolean(), {Type::map(Type::integer(), Type::integer()), Type::integer(), Type::integer()}, (void*) &insert_int_int},
    });

	method("clear", {
		{Type::map(Type::any(), Type::any()), {Type::map()}, (void*) &LSMap<LSValue*,LSValue*>::ls_clear, Method::NATIVE},
		{Type::map(Type::any(), Type::real()), {Type::map(Type::any(), Type::real())}, (void*) &LSMap<LSValue*,double>::ls_clear, Method::NATIVE},
		{Type::map(Type::any(), Type::integer()), {Type::map(Type::any(), Type::integer())}, (void*) &LSMap<LSValue*,int>::ls_clear, Method::NATIVE},
		{Type::map(Type::integer(), Type::any()), {Type::map(Type::integer(), Type::any())}, (void*) &LSMap<int,LSValue*>::ls_clear, Method::NATIVE},
		{Type::map(Type::integer(), Type::real()), {Type::map(Type::integer(), Type::real())}, (void*) &LSMap<int,double>::ls_clear, Method::NATIVE},
		{Type::map(Type::integer(), Type::integer()), {Type::map(Type::integer(), Type::integer())}, (void*) &LSMap<int,int>::ls_clear, Method::NATIVE},
	});

	method("erase", {
		{Type::boolean(), {Type::map(Type::any(), Type::any()), Type::any()}, (void*) &LSMap<LSValue*,LSValue*>::ls_erase, Method::NATIVE},
		{Type::boolean(), {Type::map(Type::any(), Type::real()), Type::any()}, (void*) &LSMap<LSValue*,double>::ls_erase, Method::NATIVE},
		{Type::boolean(), {Type::map(Type::any(), Type::integer()), Type::any()}, (void*) &LSMap<LSValue*,int>::ls_erase, Method::NATIVE},
		{Type::boolean(), {Type::map(Type::integer(), Type::any()), Type::integer()}, (void*) &LSMap<int,LSValue*>::ls_erase, Method::NATIVE},
		{Type::boolean(), {Type::map(Type::integer(), Type::real()), Type::integer()}, (void*) &LSMap<int,double>::ls_erase, Method::NATIVE},
		{Type::boolean(), {Type::map(Type::integer(), Type::integer()), Type::integer()}, (void*) &LSMap<int,int>::ls_erase, Method::NATIVE},
	});

	// V Map<K, V>::look(K, V)
	method("look", {
		{Type::any(), {Type::const_map(Type::any(), Type::any()), Type::any(), Type::any()}, (void*) &LSMap<LSValue*,LSValue*>::ls_look, Method::NATIVE},
		{Type::any(), {Type::const_map(Type::any(), Type::any()), Type::any(), Type::any()}, (void*) &look_any_any},
		{Type::real(), {Type::const_map(Type::any(), Type::real()), Type::any(), Type::real()}, (void*) &look_any_real},
		{Type::integer(), {Type::const_map(Type::any(), Type::integer()), Type::any(), Type::integer()}, (void*) &look_any_int},
		{Type::any(), {Type::const_map(Type::integer(), Type::any()), Type::integer(), Type::any()}, (void*) &look_int_any},
		{Type::real(), {Type::const_map(Type::integer(), Type::real()), Type::integer(), Type::real()}, (void*) &look_int_real},
		{Type::integer(), {Type::const_map(Type::integer(), Type::integer()), Type::integer(), Type::integer()}, (void*) &look_int_int},
	});

	method("min", {
		{Type::any(), {Type::const_map(Type::any(), Type::any())}, (void*) &LSMap<LSValue*,LSValue*>::ls_min, Method::NATIVE},
		{Type::real(), {Type::const_map(Type::any(), Type::real())}, (void*) &LSMap<LSValue*,double>::ls_min, Method::NATIVE},
		{Type::integer(), {Type::const_map(Type::any(), Type::integer())}, (void*) &LSMap<LSValue*,int>::ls_min, Method::NATIVE},
		{Type::any(), {Type::const_map(Type::integer(), Type::any())}, (void*) &LSMap<int,LSValue*>::ls_min, Method::NATIVE},
		{Type::real(), {Type::const_map(Type::integer(), Type::real())}, (void*) &LSMap<int,double>::ls_min, Method::NATIVE},
		{Type::integer(), {Type::const_map(Type::integer(), Type::integer())}, (void*) &LSMap<int,int>::ls_min, Method::NATIVE},
	});

	method("minKey", {
		{Type::any(), {Type::const_map(Type::any(), Type::any())}, (void*) &LSMap<LSValue*,LSValue*>::ls_minKey, Method::NATIVE},
		{Type::any(), {Type::const_map(Type::any(), Type::real())}, (void*) &LSMap<LSValue*,double>::ls_minKey, Method::NATIVE},
		{Type::any(), {Type::const_map(Type::any(), Type::integer())}, (void*) &LSMap<LSValue*,int>::ls_minKey, Method::NATIVE},
		{Type::integer(), {Type::const_map(Type::integer(), Type::any())}, (void*) &LSMap<int,LSValue*>::ls_minKey, Method::NATIVE},
		{Type::integer(), {Type::const_map(Type::integer(), Type::real())}, (void*) &LSMap<int,double>::ls_minKey, Method::NATIVE},
		{Type::integer(), {Type::const_map(Type::integer(), Type::integer())}, (void*) &LSMap<int,int>::ls_minKey, Method::NATIVE},
	});

	method("max", {
		{Type::any(), {Type::const_map(Type::any(), Type::any())}, (void*) &LSMap<LSValue*,LSValue*>::ls_max, Method::NATIVE},
		{Type::real(), {Type::const_map(Type::any(), Type::real())}, (void*) &LSMap<LSValue*,double>::ls_max, Method::NATIVE},
		{Type::integer(), {Type::const_map(Type::any(), Type::integer())}, (void*) &LSMap<LSValue*,int>::ls_max, Method::NATIVE},
		{Type::any(), {Type::const_map(Type::integer(), Type::any())}, (void*) &LSMap<int,LSValue*>::ls_max, Method::NATIVE},
		{Type::real(), {Type::const_map(Type::integer(), Type::real())}, (void*) &LSMap<int,double>::ls_max, Method::NATIVE},
		{Type::integer(), {Type::const_map(Type::integer(), Type::integer())}, (void*) &LSMap<int,int>::ls_max, Method::NATIVE},
	});

	method("maxKey", {
		{Type::any(), {Type::const_map(Type::any(), Type::any())}, (void*) &LSMap<LSValue*,LSValue*>::ls_maxKey, Method::NATIVE},
		{Type::any(), {Type::const_map(Type::any(), Type::real())}, (void*) &LSMap<LSValue*,double>::ls_maxKey, Method::NATIVE},
		{Type::any(), {Type::const_map(Type::any(), Type::integer())}, (void*) &LSMap<LSValue*,int>::ls_maxKey, Method::NATIVE},
		{Type::integer(), {Type::const_map(Type::integer(), Type::any())}, (void*) &LSMap<int,LSValue*>::ls_maxKey, Method::NATIVE},
		{Type::integer(), {Type::const_map(Type::integer(), Type::real())}, (void*) &LSMap<int,double>::ls_maxKey, Method::NATIVE},
		{Type::integer(), {Type::const_map(Type::integer(), Type::integer())}, (void*) &LSMap<int,int>::ls_maxKey, Method::NATIVE},
	});

	auto iter_ptr_ptr = Type::fun({}, {Type::any(), Type::any()});
	auto iter_ptr_real = Type::fun({}, {Type::any(), Type::real()});
	auto iter_ptr_int = Type::fun({}, {Type::any(), Type::integer()});
	auto iter_int_ptr = Type::fun({}, {Type::integer(), Type::any()});
	auto iter_int_real = Type::fun({}, {Type::integer(), Type::real()});
	auto iter_int_int = Type::fun({}, {Type::integer(), Type::integer()});

	auto iter_ptr_ptr_fun = &LSMap<LSValue*, LSValue*>::ls_iter<LSFunction*>;
	auto iter_ptr_real_fun = &LSMap<LSValue*, double>::ls_iter<LSFunction*>;
	auto iter_ptr_int_fun = &LSMap<LSValue*, int>::ls_iter<LSFunction*>;
	auto iter_int_ptr_fun = &LSMap<int, LSValue*>::ls_iter<LSFunction*>;
	auto iter_int_real_fun = &LSMap<int, double>::ls_iter<LSFunction*>;
	auto iter_int_int_fun = &LSMap<int, int>::ls_iter<LSFunction*>;

	method("iter", {
		{{}, {Type::const_map(Type::any(), Type::any()), iter_ptr_ptr}, (void*) iter_ptr_ptr_fun, Method::NATIVE},
		{{}, {Type::const_map(Type::any(), Type::real()), iter_ptr_real}, (void*) iter_ptr_real_fun, Method::NATIVE},
		{{}, {Type::const_map(Type::any(), Type::integer()), iter_ptr_int}, (void*) iter_ptr_int_fun, Method::NATIVE},
		{{}, {Type::const_map(Type::integer(), Type::any()), iter_int_ptr}, (void*) iter_int_ptr_fun, Method::NATIVE},
		{{}, {Type::const_map(Type::integer(), Type::real()), iter_int_real}, (void*) iter_int_real_fun, Method::NATIVE},
		{{}, {Type::const_map(Type::integer(), Type::integer()), iter_int_int}, (void*) iter_int_int_fun, Method::NATIVE},
	});
}

Compiler::value MapSTD::insert_any_any(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::boolean(), {args[0], c.insn_to_any(args[1]), c.insn_to_any(args[2])}, (void*) &LSMap<LSValue*, LSValue*>::ls_insert);
}
Compiler::value MapSTD::insert_any_real(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::boolean(), {args[0], c.insn_to_any(args[1]), c.to_real(args[2])}, (void*) &LSMap<LSValue*, double>::ls_insert);
}
Compiler::value MapSTD::insert_any_int(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::boolean(), {args[0], c.insn_to_any(args[1]), c.to_int(args[2])}, (void*) &LSMap<LSValue*, int>::ls_insert);
}
Compiler::value MapSTD::insert_int_any(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::boolean(), {args[0], c.to_int(args[1]), c.insn_to_any(args[2])}, (void*) &LSMap<int, LSValue*>::ls_insert);
}
Compiler::value MapSTD::insert_int_real(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::boolean(), {args[0], c.to_int(args[1]), c.to_real(args[2])}, (void*) &LSMap<int, double>::ls_insert);
}
Compiler::value MapSTD::insert_int_int(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::boolean(), {args[0], c.to_int(args[1]), c.to_int(args[2])}, (void*) &LSMap<int, int>::ls_insert);
}

Compiler::value MapSTD::look_any_any(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::any(), {args[0], c.insn_to_any(args[1]), c.insn_to_any(args[2])}, (void*) &LSMap<LSValue*, LSValue*>::ls_look);
}
Compiler::value MapSTD::look_any_real(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::real(), {args[0], c.insn_to_any(args[1]), c.to_real(args[2])}, (void*) &LSMap<LSValue*, double>::ls_look);
}
Compiler::value MapSTD::look_any_int(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::integer(), {args[0], c.insn_to_any(args[1]), c.to_int(args[2])}, (void*) &LSMap<LSValue*, int>::ls_look);
}
Compiler::value MapSTD::look_int_any(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::any(), {args[0], c.to_int(args[1]), c.insn_to_any(args[2])}, (void*) &LSMap<int, LSValue*>::ls_look);
}
Compiler::value MapSTD::look_int_real(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::real(), {args[0], c.to_int(args[1]), c.to_real(args[2])}, (void*) &LSMap<int, double>::ls_look);
}
Compiler::value MapSTD::look_int_int(Compiler& c, std::vector<Compiler::value> args) {
	return c.insn_call(Type::integer(), {args[0], c.to_int(args[1]), c.to_int(args[2])}, (void*) &LSMap<int, int>::ls_look);
}

}
