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
		{Type::CONST_PTR_PTR_MAP, Type::POINTER, Type::BOOLEAN, (void*) &LSMap<LSValue*, LSValue*>::in, {}, Method::NATIVE},
		{Type::CONST_PTR_REAL_MAP, Type::POINTER, Type::BOOLEAN, (void*) &LSMap<LSValue*, double>::in, {}, Method::NATIVE},
		{Type::CONST_PTR_INT_MAP, Type::POINTER, Type::BOOLEAN, (void*) &LSMap<LSValue*, int>::in, {}, Method::NATIVE},
		{Type::CONST_REAL_PTR_MAP, Type::REAL, Type::BOOLEAN, (void*) &LSMap<double, LSValue*>::in, {}, Method::NATIVE},
		{Type::CONST_REAL_REAL_MAP, Type::REAL, Type::BOOLEAN, (void*) &LSMap<double, double>::in, {}, Method::NATIVE},
		{Type::CONST_REAL_INT_MAP, Type::REAL, Type::BOOLEAN, (void*) &LSMap<double, int>::in, {}, Method::NATIVE},
		{Type::CONST_INT_PTR_MAP, Type::INTEGER, Type::BOOLEAN, (void*) &LSMap<int, LSValue*>::in, {}, Method::NATIVE},
		{Type::CONST_INT_REAL_MAP, Type::INTEGER, Type::BOOLEAN, (void*) &LSMap<int, double>::in, {}, Method::NATIVE},
		{Type::CONST_INT_INT_MAP, Type::LONG, Type::BOOLEAN, (void*) &LSMap<int, int>::in, {}, Method::NATIVE},
	});

	method("size", {
		{Type::INTEGER, {Type::CONST_PTR_PTR_MAP}, (void*) map_size, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_PTR_REAL_MAP}, (void*) map_size, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_PTR_INT_MAP}, (void*) map_size, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_PTR_MAP}, (void*) map_size, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_REAL_MAP}, (void*) map_size, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_INT_MAP}, (void*) map_size, Method::NATIVE},
    });

	method("values", {
		{Type::PTR_ARRAY, {Type::CONST_PTR_PTR_MAP}, (void*) &LSMap<LSValue*, LSValue*>::values, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::CONST_PTR_REAL_MAP}, (void*) &LSMap<LSValue*, double>::values, Method::NATIVE},
		{Type::INT_ARRAY, {Type::CONST_PTR_INT_MAP}, (void*) &LSMap<LSValue*, int>::values, Method::NATIVE},
		{Type::PTR_ARRAY, {Type::CONST_REAL_PTR_MAP}, (void*) &LSMap<double, LSValue*>::values, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::CONST_REAL_REAL_MAP}, (void*) &LSMap<double, double>::values, Method::NATIVE},
		{Type::INT_ARRAY, {Type::CONST_REAL_INT_MAP}, (void*) &LSMap<double, int>::values, Method::NATIVE},
		{Type::PTR_ARRAY, {Type::CONST_INT_PTR_MAP}, (void*) &LSMap<int, LSValue*>::values, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::CONST_INT_REAL_MAP}, (void*) &LSMap<int, double>::values, Method::NATIVE},
		{Type::INT_ARRAY, {Type::CONST_INT_INT_MAP}, (void*) &LSMap<int, int>::values, Method::NATIVE}
	});

	method("insert", {
		{Type::BOOLEAN, {Type::PTR_PTR_MAP, Type::POINTER, Type::POINTER}, (void*) &LSMap<LSValue*,LSValue*>::ls_insert, Method::NATIVE},
		{Type::BOOLEAN, {Type::PTR_REAL_MAP, Type::POINTER, Type::REAL}, (void*) &LSMap<LSValue*,double>::ls_insert, Method::NATIVE},
		{Type::BOOLEAN, {Type::PTR_INT_MAP, Type::POINTER, Type::INTEGER}, (void*) &LSMap<LSValue*,int>::ls_insert, Method::NATIVE},
		{Type::BOOLEAN, {Type::INT_PTR_MAP, Type::INTEGER, Type::POINTER}, (void*) &LSMap<int,LSValue*>::ls_insert, Method::NATIVE},
		{Type::BOOLEAN, {Type::INT_REAL_MAP, Type::INTEGER, Type::REAL}, (void*) &LSMap<int,double>::ls_insert, Method::NATIVE},
		{Type::BOOLEAN, {Type::INT_INT_MAP, Type::INTEGER, Type::INTEGER}, (void*) &LSMap<int,int>::ls_insert, Method::NATIVE},
    });

	method("clear", {
		{Type::PTR_PTR_MAP, {Type::MAP}, (void*) &LSMap<LSValue*,LSValue*>::ls_clear, Method::NATIVE},
		{Type::PTR_REAL_MAP, {Type::PTR_REAL_MAP}, (void*) &LSMap<LSValue*,double>::ls_clear, Method::NATIVE},
		{Type::PTR_INT_MAP, {Type::PTR_INT_MAP}, (void*) &LSMap<LSValue*,int>::ls_clear, Method::NATIVE},
		{Type::INT_PTR_MAP, {Type::INT_PTR_MAP}, (void*) &LSMap<int,LSValue*>::ls_clear, Method::NATIVE},
		{Type::INT_REAL_MAP, {Type::INT_REAL_MAP}, (void*) &LSMap<int,double>::ls_clear, Method::NATIVE},
		{Type::INT_INT_MAP, {Type::INT_INT_MAP}, (void*) &LSMap<int,int>::ls_clear, Method::NATIVE},
	});

	method("erase", {
		{Type::BOOLEAN, {Type::PTR_PTR_MAP, Type::POINTER}, (void*) &LSMap<LSValue*,LSValue*>::ls_erase, Method::NATIVE},
		{Type::BOOLEAN, {Type::PTR_REAL_MAP, Type::POINTER}, (void*) &LSMap<LSValue*,double>::ls_erase, Method::NATIVE},
		{Type::BOOLEAN, {Type::PTR_INT_MAP, Type::POINTER}, (void*) &LSMap<LSValue*,int>::ls_erase, Method::NATIVE},
		{Type::BOOLEAN, {Type::INT_PTR_MAP, Type::INTEGER}, (void*) &LSMap<int,LSValue*>::ls_erase, Method::NATIVE},
		{Type::BOOLEAN, {Type::INT_REAL_MAP, Type::INTEGER}, (void*) &LSMap<int,double>::ls_erase, Method::NATIVE},
		{Type::BOOLEAN, {Type::INT_INT_MAP, Type::INTEGER}, (void*) &LSMap<int,int>::ls_erase, Method::NATIVE},
	});

	method("look", {
		{Type::POINTER, {Type::CONST_PTR_PTR_MAP, Type::POINTER, Type::POINTER}, (void*) &LSMap<LSValue*,LSValue*>::ls_look, Method::NATIVE},
		{Type::REAL, {Type::CONST_PTR_REAL_MAP, Type::POINTER, Type::REAL}, (void*) &LSMap<LSValue*,double>::ls_look, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_PTR_INT_MAP, Type::POINTER, Type::INTEGER}, (void*) &LSMap<LSValue*,int>::ls_look, Method::NATIVE},
		{Type::POINTER, {Type::CONST_INT_PTR_MAP, Type::INTEGER, Type::POINTER}, (void*) &LSMap<int,LSValue*>::ls_look, Method::NATIVE},
		{Type::REAL, {Type::CONST_INT_REAL_MAP, Type::INTEGER, Type::REAL}, (void*) &LSMap<int,double>::ls_look, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_INT_MAP, Type::INTEGER, Type::INTEGER}, (void*) &LSMap<int,int>::ls_look, Method::NATIVE},
	});

	method("min", {
		{Type::POINTER, {Type::CONST_PTR_PTR_MAP}, (void*) &LSMap<LSValue*,LSValue*>::ls_min, Method::NATIVE},
		{Type::REAL, {Type::CONST_PTR_REAL_MAP}, (void*) &LSMap<LSValue*,double>::ls_min, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_PTR_INT_MAP}, (void*) &LSMap<LSValue*,int>::ls_min, Method::NATIVE},
		{Type::POINTER, {Type::CONST_INT_PTR_MAP}, (void*) &LSMap<int,LSValue*>::ls_min, Method::NATIVE},
		{Type::REAL, {Type::CONST_INT_REAL_MAP}, (void*) &LSMap<int,double>::ls_min, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_INT_MAP}, (void*) &LSMap<int,int>::ls_min, Method::NATIVE},
	});

	method("minKey", {
		{Type::POINTER, {Type::CONST_PTR_PTR_MAP}, (void*) &LSMap<LSValue*,LSValue*>::ls_minKey, Method::NATIVE},
		{Type::POINTER, {Type::CONST_PTR_REAL_MAP}, (void*) &LSMap<LSValue*,double>::ls_minKey, Method::NATIVE},
		{Type::POINTER, {Type::CONST_PTR_INT_MAP}, (void*) &LSMap<LSValue*,int>::ls_minKey, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_PTR_MAP}, (void*) &LSMap<int,LSValue*>::ls_minKey, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_REAL_MAP}, (void*) &LSMap<int,double>::ls_minKey, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_INT_MAP}, (void*) &LSMap<int,int>::ls_minKey, Method::NATIVE},
	});

	method("max", {
		{Type::POINTER, {Type::CONST_PTR_PTR_MAP}, (void*) &LSMap<LSValue*,LSValue*>::ls_max, Method::NATIVE},
		{Type::REAL, {Type::CONST_PTR_REAL_MAP}, (void*) &LSMap<LSValue*,double>::ls_max, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_PTR_INT_MAP}, (void*) &LSMap<LSValue*,int>::ls_max, Method::NATIVE},
		{Type::POINTER, {Type::CONST_INT_PTR_MAP}, (void*) &LSMap<int,LSValue*>::ls_max, Method::NATIVE},
		{Type::REAL, {Type::CONST_INT_REAL_MAP}, (void*) &LSMap<int,double>::ls_max, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_INT_MAP}, (void*) &LSMap<int,int>::ls_max, Method::NATIVE},
	});

	method("maxKey", {
		{Type::POINTER, {Type::CONST_PTR_PTR_MAP}, (void*) &LSMap<LSValue*,LSValue*>::ls_maxKey, Method::NATIVE},
		{Type::POINTER, {Type::CONST_PTR_REAL_MAP}, (void*) &LSMap<LSValue*,double>::ls_maxKey, Method::NATIVE},
		{Type::POINTER, {Type::CONST_PTR_INT_MAP}, (void*) &LSMap<LSValue*,int>::ls_maxKey, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_PTR_MAP}, (void*) &LSMap<int,LSValue*>::ls_maxKey, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_REAL_MAP}, (void*) &LSMap<int,double>::ls_maxKey, Method::NATIVE},
		{Type::INTEGER, {Type::CONST_INT_INT_MAP}, (void*) &LSMap<int,int>::ls_maxKey, Method::NATIVE},
	});

	Type iter_ptr_ptr = Type::FUNCTION_P;
	iter_ptr_ptr.setArgumentType(0, Type::POINTER);
	iter_ptr_ptr.setArgumentType(1, Type::POINTER);
	iter_ptr_ptr.setReturnType({});

	Type iter_ptr_real = Type::FUNCTION_P;
	iter_ptr_real.setArgumentType(0, Type::POINTER);
	iter_ptr_real.setArgumentType(1, Type::REAL);
	iter_ptr_real.setReturnType({});

	Type iter_ptr_int = Type::FUNCTION_P;
	iter_ptr_int.setArgumentType(0, Type::POINTER);
	iter_ptr_int.setArgumentType(1, Type::INTEGER);
	iter_ptr_int.setReturnType({});

	Type iter_int_ptr = Type::FUNCTION_P;
	iter_int_ptr.setArgumentType(0, Type::INTEGER);
	iter_int_ptr.setArgumentType(1, Type::POINTER);
	iter_int_ptr.setReturnType({});

	Type iter_int_real = Type::FUNCTION_P;
	iter_int_real.setArgumentType(0, Type::INTEGER);
	iter_int_real.setArgumentType(1, Type::REAL);
	iter_int_real.setReturnType({});

	Type iter_int_int = Type::FUNCTION_P;
	iter_int_int.setArgumentType(0, Type::INTEGER);
	iter_int_int.setArgumentType(1, Type::INTEGER);
	iter_int_int.setReturnType({});

	auto iter_ptr_ptr_fun = &LSMap<LSValue*, LSValue*>::ls_iter<LSFunction*>;
	auto iter_ptr_real_fun = &LSMap<LSValue*, double>::ls_iter<LSFunction*>;
	auto iter_ptr_int_fun = &LSMap<LSValue*, int>::ls_iter<LSFunction*>;
	auto iter_int_ptr_fun = &LSMap<int, LSValue*>::ls_iter<LSFunction*>;
	auto iter_int_real_fun = &LSMap<int, double>::ls_iter<LSFunction*>;
	auto iter_int_int_fun = &LSMap<int, int>::ls_iter<LSFunction*>;

	method("iter", {
		{{}, {Type::CONST_PTR_PTR_MAP, iter_ptr_ptr}, (void*) iter_ptr_ptr_fun, Method::NATIVE},
		{{}, {Type::CONST_PTR_REAL_MAP, iter_ptr_real}, (void*) &iter_ptr_real_fun, Method::NATIVE},
		{{}, {Type::CONST_PTR_INT_MAP, iter_ptr_int}, (void*) &iter_ptr_int_fun, Method::NATIVE},
		{{}, {Type::CONST_INT_PTR_MAP, iter_int_ptr}, (void*) &iter_int_ptr_fun, Method::NATIVE},
		{{}, {Type::CONST_INT_REAL_MAP, iter_int_real}, (void*) &iter_int_real_fun, Method::NATIVE},
		{{}, {Type::CONST_INT_INT_MAP, iter_int_int}, (void*) &iter_int_int_fun, Method::NATIVE},
	});
}

}
