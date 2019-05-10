#include "MapSTD.hpp"
#include "../value/LSMap.hpp"

namespace ls {

int map_size(const LSMap<LSValue*,LSValue*>* map) {
	int r = map->size();
	if (map->refs == 0) delete map;
	return r;
}
void* iterator_end(LSMap<int, int>* map) {
	return map->end()._M_node;
}
LSMap<int, int>::iterator iterator_inc(LSMap<int, int>::iterator it) {
	it++;
	return it;
}
LSMap<int, int>::iterator iterator_dec(LSMap<int, int>::iterator it) {
	it--;
	return it;
}
void* iterator_rkey(std::map<void*, void*>::iterator it) {
	return std::map<void*, void*>::reverse_iterator(it)->first;
}
int iterator_rget_ii(std::map<int, int>::iterator it) {
	return std::map<int, int>::reverse_iterator(it)->second;
}
int iterator_rget_vi(std::map<void*, int>::iterator it) {
	return std::map<void*, int>::reverse_iterator(it)->second;
}
double iterator_rget_ir(std::map<int, double>::iterator it) {
	return std::map<int, double>::reverse_iterator(it)->second;
}
void* iterator_rget_vv(std::map<void*, void*>::iterator it) {
	return std::map<void*, void*>::reverse_iterator(it)->second;
}
std::map<int, int>::iterator end(LSMap<int, int>* map) {
	return map->end();
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

	/*
	 * Constructor
	 */
	constructor_({
		{Type::tmp_map(Type::any(), Type::any()), {}, (void*) &LSMap<LSValue*, LSValue*>::constructor},
		{Type::tmp_map(Type::any(), Type::real()), {}, (void*) &LSMap<LSValue*, double>::constructor},
		{Type::tmp_map(Type::any(), Type::integer()), {}, (void*) &LSMap<LSValue*, int>::constructor},
		{Type::tmp_map(Type::real(), Type::any()), {}, (void*) &LSMap<double, LSValue*>::constructor},
		{Type::tmp_map(Type::real(), Type::real()), {}, (void*) &LSMap<double, double>::constructor},
		{Type::tmp_map(Type::real(), Type::integer()), {}, (void*) &LSMap<double, int>::constructor},
		{Type::tmp_map(Type::integer(), Type::any()), {}, (void*) &LSMap<int, LSValue*>::constructor},
		{Type::tmp_map(Type::integer(), Type::real()), {}, (void*) &LSMap<int, double>::constructor},
		{Type::tmp_map(Type::integer(), Type::integer()), {}, (void*) &LSMap<int, int>::constructor},
	});

	operator_("in", {
		{Type::const_map(Type::any(), Type::any()), Type::any(), Type::boolean(), (void*) &LSMap<LSValue*, LSValue*>::in},
		{Type::const_map(Type::any(), Type::real()), Type::any(), Type::boolean(), (void*) &LSMap<LSValue*, double>::in},
		{Type::const_map(Type::any(), Type::integer()), Type::any(), Type::boolean(), (void*) &LSMap<LSValue*, int>::in},
		{Type::const_map(Type::real(), Type::any()), Type::real(), Type::boolean(), (void*) &LSMap<double, LSValue*>::in},
		{Type::const_map(Type::real(), Type::real()), Type::real(), Type::boolean(), (void*) &LSMap<double, double>::in},
		{Type::const_map(Type::real(), Type::integer()), Type::real(), Type::boolean(), (void*) &LSMap<double, int>::in},
		{Type::const_map(Type::integer(), Type::any()), Type::integer(), Type::boolean(), (void*) &LSMap<int, LSValue*>::in},
		{Type::const_map(Type::integer(), Type::real()), Type::integer(), Type::boolean(), (void*) &LSMap<int, double>::in},
		{Type::const_map(Type::integer(), Type::integer()), Type::long_(), Type::boolean(), (void*) &LSMap<int, int>::in},
	});

	method("size", {
		{Type::integer(), {Type::const_map(Type::any(), Type::any())}, (void*) map_size},
		{Type::integer(), {Type::const_map(Type::any(), Type::real())}, (void*) map_size},
		{Type::integer(), {Type::const_map(Type::any(), Type::integer())}, (void*) map_size},
		{Type::integer(), {Type::const_map(Type::integer(), Type::any())}, (void*) map_size},
		{Type::integer(), {Type::const_map(Type::integer(), Type::real())}, (void*) map_size},
		{Type::integer(), {Type::const_map(Type::integer(), Type::integer())}, (void*) map_size},
    });

	method("values", {
		{Type::array(Type::any()), {Type::const_map(Type::any(), Type::any())}, (void*) &LSMap<LSValue*, LSValue*>::values},
		{Type::array(Type::real()), {Type::const_map(Type::any(), Type::real())}, (void*) &LSMap<LSValue*, double>::values},
		{Type::array(Type::integer()), {Type::const_map(Type::any(), Type::integer())}, (void*) &LSMap<LSValue*, int>::values},
		{Type::array(Type::any()), {Type::const_map(Type::real(), Type::any())}, (void*) &LSMap<double, LSValue*>::values},
		{Type::array(Type::real()), {Type::const_map(Type::real(), Type::real())}, (void*) &LSMap<double, double>::values},
		{Type::array(Type::integer()), {Type::const_map(Type::real(), Type::integer())}, (void*) &LSMap<double, int>::values},
		{Type::array(Type::any()), {Type::const_map(Type::integer(), Type::any())}, (void*) &LSMap<int, LSValue*>::values},
		{Type::array(Type::real()), {Type::const_map(Type::integer(), Type::real())}, (void*) &LSMap<int, double>::values},
		{Type::array(Type::integer()), {Type::const_map(Type::integer(), Type::integer())}, (void*) &LSMap<int, int>::values}
	});

	method("insert", {
		{Type::boolean(), {Type::map(Type::any(), Type::any()), Type::any(), Type::any()}, (void*) &LSMap<LSValue*, LSValue*>::ls_insert},
		{Type::boolean(), {Type::map(Type::any(), Type::real()), Type::any(), Type::real()}, (void*) &LSMap<LSValue*, double>::ls_insert},
		{Type::boolean(), {Type::map(Type::any(), Type::integer()), Type::any(), Type::integer()}, (void*) &LSMap<LSValue*, int>::ls_insert},
		{Type::boolean(), {Type::map(Type::integer(), Type::any()), Type::integer(), Type::any()}, (void*) &LSMap<int, LSValue*>::ls_insert},
		{Type::boolean(), {Type::map(Type::integer(), Type::real()), Type::integer(), Type::real()}, (void*) &LSMap<int, double>::ls_insert},
		{Type::boolean(), {Type::map(Type::integer(), Type::integer()), Type::integer(), Type::integer()}, (void*) &LSMap<int, int>::ls_insert},
    });

	method("clear", {
		{Type::map(Type::any(), Type::any()), {Type::map()}, (void*) &LSMap<LSValue*,LSValue*>::ls_clear},
		{Type::map(Type::any(), Type::real()), {Type::map(Type::any(), Type::real())}, (void*) &LSMap<LSValue*,double>::ls_clear},
		{Type::map(Type::any(), Type::integer()), {Type::map(Type::any(), Type::integer())}, (void*) &LSMap<LSValue*,int>::ls_clear},
		{Type::map(Type::integer(), Type::any()), {Type::map(Type::integer(), Type::any())}, (void*) &LSMap<int,LSValue*>::ls_clear},
		{Type::map(Type::integer(), Type::real()), {Type::map(Type::integer(), Type::real())}, (void*) &LSMap<int,double>::ls_clear},
		{Type::map(Type::integer(), Type::integer()), {Type::map(Type::integer(), Type::integer())}, (void*) &LSMap<int,int>::ls_clear},
	});

	method("erase", {
		{Type::boolean(), {Type::map(Type::any(), Type::any()), Type::any()}, (void*) &LSMap<LSValue*,LSValue*>::ls_erase},
		{Type::boolean(), {Type::map(Type::any(), Type::real()), Type::any()}, (void*) &LSMap<LSValue*,double>::ls_erase},
		{Type::boolean(), {Type::map(Type::any(), Type::integer()), Type::any()}, (void*) &LSMap<LSValue*,int>::ls_erase},
		{Type::boolean(), {Type::map(Type::integer(), Type::any()), Type::integer()}, (void*) &LSMap<int,LSValue*>::ls_erase},
		{Type::boolean(), {Type::map(Type::integer(), Type::real()), Type::integer()}, (void*) &LSMap<int,double>::ls_erase},
		{Type::boolean(), {Type::map(Type::integer(), Type::integer()), Type::integer()}, (void*) &LSMap<int,int>::ls_erase},
	});

	// V Map<K, V>::look(K, V)
	method("look", {
		{Type::any(), {Type::const_map(Type::any(), Type::any()), Type::any(), Type::any()}, (void*) &LSMap<LSValue*,LSValue*>::ls_look},
		{Type::any(), {Type::const_map(Type::any(), Type::any()), Type::any(), Type::any()}, look_any_any},
		{Type::real(), {Type::const_map(Type::any(), Type::real()), Type::any(), Type::real()}, look_any_real},
		{Type::integer(), {Type::const_map(Type::any(), Type::integer()), Type::any(), Type::integer()}, look_any_int},
		{Type::any(), {Type::const_map(Type::integer(), Type::any()), Type::integer(), Type::any()}, look_int_any},
		{Type::real(), {Type::const_map(Type::integer(), Type::real()), Type::integer(), Type::real()}, look_int_real},
		{Type::integer(), {Type::const_map(Type::integer(), Type::integer()), Type::integer(), Type::integer()}, look_int_int},
	});

	method("min", {
		{Type::any(), {Type::const_map(Type::any(), Type::any())}, (void*) &LSMap<LSValue*,LSValue*>::ls_min, THROWS},
		{Type::real(), {Type::const_map(Type::any(), Type::real())}, (void*) &LSMap<LSValue*,double>::ls_min, THROWS},
		{Type::integer(), {Type::const_map(Type::any(), Type::integer())}, (void*) &LSMap<LSValue*,int>::ls_min, THROWS},
		{Type::any(), {Type::const_map(Type::integer(), Type::any())}, (void*) &LSMap<int,LSValue*>::ls_min, THROWS},
		{Type::real(), {Type::const_map(Type::integer(), Type::real())}, (void*) &LSMap<int,double>::ls_min, THROWS},
		{Type::integer(), {Type::const_map(Type::integer(), Type::integer())}, (void*) &LSMap<int,int>::ls_min, THROWS},
	});

	method("minKey", {
		{Type::any(), {Type::const_map(Type::any(), Type::any())}, (void*) &LSMap<LSValue*,LSValue*>::ls_minKey, THROWS},
		{Type::any(), {Type::const_map(Type::any(), Type::real())}, (void*) &LSMap<LSValue*,double>::ls_minKey, THROWS},
		{Type::any(), {Type::const_map(Type::any(), Type::integer())}, (void*) &LSMap<LSValue*,int>::ls_minKey, THROWS},
		{Type::integer(), {Type::const_map(Type::integer(), Type::any())}, (void*) &LSMap<int,LSValue*>::ls_minKey, THROWS},
		{Type::integer(), {Type::const_map(Type::integer(), Type::real())}, (void*) &LSMap<int,double>::ls_minKey, THROWS},
		{Type::integer(), {Type::const_map(Type::integer(), Type::integer())}, (void*) &LSMap<int,int>::ls_minKey, THROWS},
	});

	method("max", {
		{Type::any(), {Type::const_map(Type::any(), Type::any())}, (void*) &LSMap<LSValue*,LSValue*>::ls_max, THROWS},
		{Type::real(), {Type::const_map(Type::any(), Type::real())}, (void*) &LSMap<LSValue*,double>::ls_max, THROWS},
		{Type::integer(), {Type::const_map(Type::any(), Type::integer())}, (void*) &LSMap<LSValue*,int>::ls_max, THROWS},
		{Type::any(), {Type::const_map(Type::integer(), Type::any())}, (void*) &LSMap<int,LSValue*>::ls_max, THROWS},
		{Type::real(), {Type::const_map(Type::integer(), Type::real())}, (void*) &LSMap<int,double>::ls_max, THROWS},
		{Type::integer(), {Type::const_map(Type::integer(), Type::integer())}, (void*) &LSMap<int,int>::ls_max, THROWS},
	});

	method("maxKey", {
		{Type::any(), {Type::const_map(Type::any(), Type::any())}, (void*) &LSMap<LSValue*,LSValue*>::ls_maxKey, THROWS},
		{Type::any(), {Type::const_map(Type::any(), Type::real())}, (void*) &LSMap<LSValue*,double>::ls_maxKey, THROWS},
		{Type::any(), {Type::const_map(Type::any(), Type::integer())}, (void*) &LSMap<LSValue*,int>::ls_maxKey, THROWS},
		{Type::integer(), {Type::const_map(Type::integer(), Type::any())}, (void*) &LSMap<int,LSValue*>::ls_maxKey, THROWS},
		{Type::integer(), {Type::const_map(Type::integer(), Type::real())}, (void*) &LSMap<int,double>::ls_maxKey, THROWS},
		{Type::integer(), {Type::const_map(Type::integer(), Type::integer())}, (void*) &LSMap<int,int>::ls_maxKey, THROWS},
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
		{{}, {Type::const_map(Type::any(), Type::any()), iter_ptr_ptr}, (void*) iter_ptr_ptr_fun},
		{{}, {Type::const_map(Type::any(), Type::real()), iter_ptr_real}, (void*) iter_ptr_real_fun},
		{{}, {Type::const_map(Type::any(), Type::integer()), iter_ptr_int}, (void*) iter_ptr_int_fun},
		{{}, {Type::const_map(Type::integer(), Type::any()), iter_int_ptr}, (void*) iter_int_ptr_fun},
		{{}, {Type::const_map(Type::integer(), Type::real()), iter_int_real}, (void*) iter_int_real_fun},
		{{}, {Type::const_map(Type::integer(), Type::integer()), iter_int_int}, (void*) iter_int_int_fun},
	});

	auto flT = Type::template_("T");
	auto flK = Type::template_("K");
	auto flR = Type::template_("R");
	template_(flT, flK, flR).
	method("foldLeft", {
		{flR, {Type::const_map(flK, flT), Type::fun(flR, {flR, flK, flT}), flR}, fold_left},
	});

	auto frT = Type::template_("T");
	auto frK = Type::template_("K");
	auto frR = Type::template_("R");
	template_(frT, frK, frR).
	method("foldRight", {
		{frR, {Type::const_map(frK, frT), Type::fun(frR, {frK, frT, frR}), frR}, fold_right},
	});

	/** Internal **/
	method("at", {
		{Type::integer(), {Type::map(Type::integer(), Type::integer()), Type::integer()}, (void*) &LSMap<int, int>::at},
		{Type::real(), {Type::map(Type::integer(), Type::real()), Type::integer()}, (void*) &LSMap<int, double>::at},
		{Type::any(), {Type::map(Type::integer(), Type::any()), Type::integer()}, (void*) &LSMap<int, LSValue*>::at},
		{Type::integer(), {Type::map(Type::real(), Type::integer()), Type::real()}, (void*) &LSMap<double, int>::at},
		{Type::real(), {Type::map(Type::real(), Type::real()), Type::real()}, (void*) &LSMap<double, double>::at},
		{Type::any(), {Type::map(Type::real(), Type::any()), Type::real()}, (void*) &LSMap<double, LSValue*>::at},
		{Type::integer(), {Type::map(Type::any(), Type::integer()), Type::any()}, (void*) &LSMap<LSValue*, int>::at},
		{Type::real(), {Type::map(Type::any(), Type::real()), Type::any()}, (void*) &LSMap<LSValue*, double>::at},
		{Type::any(), {Type::map(Type::any(), Type::any()), Type::any()}, (void*) &LSMap<LSValue*, LSValue*>::at},
	});
	method("insert_fun", {
		{{}, {Type::map(Type::any(), Type::any()), Type::any(), Type::any()}, (void*) &LSMap<LSValue*, LSValue*>::ls_emplace},
		{{}, {Type::map(Type::any(), Type::real()), Type::any(), Type::real()}, (void*) &LSMap<LSValue*, double>::ls_emplace},
		{{}, {Type::map(Type::any(), Type::integer()), Type::any(), Type::integer()}, (void*) &LSMap<LSValue*, int>::ls_emplace},
		{{}, {Type::map(Type::real(), Type::any()), Type::real(), Type::any()}, (void*) &LSMap<double, LSValue*>::ls_emplace},
		{{}, {Type::map(Type::real(), Type::real()), Type::real(), Type::real()}, (void*) &LSMap<double, double>::ls_emplace},
		{{}, {Type::map(Type::real(), Type::integer()), Type::real(), Type::integer()}, (void*) &LSMap<double, int>::ls_emplace},
		{{}, {Type::map(Type::integer(), Type::any()), Type::integer(), Type::any()}, (void*) &LSMap<int, LSValue*>::ls_emplace},
		{{}, {Type::map(Type::integer(), Type::real()), Type::integer(), Type::real()}, (void*) &LSMap<int, double>::ls_emplace},
		{{}, {Type::map(Type::integer(), Type::integer()), Type::integer(), Type::integer()}, (void*) &LSMap<int, int>::ls_emplace},
	});
	method("atL", {
		{{}, {Type::map(Type::any(), Type::any()), Type::any(), Type::any()}, (void*) &LSMap<LSValue*, LSValue*>::atL_base},
		{{}, {Type::map(Type::any(), Type::real()), Type::any(), Type::real()}, (void*) &LSMap<LSValue*, double>::atL_base},
		{{}, {Type::map(Type::any(), Type::integer()), Type::any(), Type::integer()}, (void*) &LSMap<LSValue*, int>::atL_base},
		{{}, {Type::map(Type::real(), Type::any()), Type::real(), Type::any()}, (void*) &LSMap<double, LSValue*>::atL_base},
		{{}, {Type::map(Type::real(), Type::real()), Type::real(), Type::real()}, (void*) &LSMap<double, double>::atL_base},
		{{}, {Type::map(Type::real(), Type::integer()), Type::real(), Type::integer()}, (void*) &LSMap<double, int>::atL_base},
		{{}, {Type::map(Type::integer(), Type::any()), Type::integer(), Type::any()}, (void*) &LSMap<int, LSValue*>::atL_base},
		{{}, {Type::map(Type::integer(), Type::real()), Type::integer(), Type::real()}, (void*) &LSMap<int, double>::atL_base},
		{{}, {Type::map(Type::integer(), Type::integer()), Type::integer(), Type::integer()}, (void*) &LSMap<int, int>::atL_base},
	});
	// std::map<int, int>::iterator (LSMap<int, int>::*mapend)() = &LSMap<int, int>::end;
	method("end", {
		{Type::map().iterator(), {Type::map()}, (void*) end}
	});
	method("iterator_end", {
		{Type::map().iterator(), {Type::map()}, (void*) iterator_end}
	});
	method("iterator_inc", {
		{Type::map().iterator(), {Type::map().iterator()}, (void*) iterator_inc}
	});
	method("iterator_dec", {
		{Type::map().iterator(), {Type::map().iterator()}, (void*) iterator_dec}
	});
	method("iterator_rkey", {
		{Type::i8().pointer(), {Type::map().iterator()}, (void*) iterator_rkey}
	});
	method("iterator_rget", {
		{Type::integer(), {Type::map().iterator()}, (void*) iterator_rget_ii},
		{Type::integer(), {Type::map().iterator()}, (void*) iterator_rget_vi},
		{Type::real(), {Type::map().iterator()}, (void*) iterator_rget_ir},
		{Type::any(), {Type::map().iterator()}, (void*) iterator_rget_vv},
	});
	method("look_fun", {
		{Type::any(), {Type::const_map(Type::any(), Type::any()), Type::any(), Type::any()}, (void*) &LSMap<LSValue*, LSValue*>::ls_look},
		{Type::real(), {Type::const_map(Type::any(), Type::real()), Type::any(), Type::real()}, (void*) &LSMap<LSValue*, double>::ls_look},
		{Type::integer(), {Type::const_map(Type::any(), Type::integer()), Type::any(), Type::integer()}, (void*) &LSMap<LSValue*, int>::ls_look},
		{Type::any(), {Type::const_map(Type::integer(), Type::any()), Type::integer(), Type::any()}, (void*) &LSMap<int, LSValue*>::ls_look},
		{Type::real(), {Type::const_map(Type::integer(), Type::real()), Type::integer(), Type::real()}, (void*) &LSMap<int, double>::ls_look},
		{Type::integer(), {Type::const_map(Type::integer(), Type::integer()), Type::integer(), Type::integer()}, (void*) &LSMap<int, int>::ls_look},
	});
}

Compiler::value MapSTD::look_any_any(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_call(Type::any(), {args[0], c.insn_to_any(args[1]), c.insn_to_any(args[2])}, "Map.look_fun");
}
Compiler::value MapSTD::look_any_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_call(Type::real(), {args[0], c.insn_to_any(args[1]), c.to_real(args[2])}, "Map.look_fun.1");
}
Compiler::value MapSTD::look_any_int(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_call(Type::integer(), {args[0], c.insn_to_any(args[1]), c.to_int(args[2])}, "Map.look_fun.2");
}
Compiler::value MapSTD::look_int_any(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_call(Type::any(), {args[0], c.to_int(args[1]), c.insn_to_any(args[2])}, "Map.look_fun.3");
}
Compiler::value MapSTD::look_int_real(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_call(Type::real(), {args[0], c.to_int(args[1]), c.to_real(args[2])}, "Map.look_fun.4");
}
Compiler::value MapSTD::look_int_int(Compiler& c, std::vector<Compiler::value> args, bool) {
	return c.insn_call(Type::integer(), {args[0], c.to_int(args[1]), c.to_int(args[2])}, "Map.look_fun.5");
}

Compiler::value MapSTD::fold_left(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto function = args[1];
	auto result = c.create_and_add_var("r", args[2].t);
	c.insn_store(result, c.insn_move(args[2]));
	c.insn_foreach(args[0], {}, "v", "k", [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		c.insn_store(result, c.insn_call(function.t.return_type(), {c.insn_load(result), k, v}, function));
		return {};
	});
	return c.insn_load(result);
}

Compiler::value MapSTD::fold_right(Compiler& c, std::vector<Compiler::value> args, bool) {
	auto function = args[1];
	auto result = c.create_and_add_var("r", args[2].t);
	c.insn_store(result, c.insn_move(args[2]));
	c.insn_foreach(args[0], {}, "v", "k", [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		c.insn_store(result, c.insn_call(function.t.return_type(), {k, v, c.insn_load(result)}, function));
		return {};
	}, true);
	return c.insn_load(result);
}

}
