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
		{Type::PTR_PTR_MAP, Type::POINTER, Type::BOOLEAN, (void*) &LSMap<LSValue*, LSValue*>::in, Method::NATIVE},
		{Type::PTR_REAL_MAP, Type::POINTER, Type::BOOLEAN, (void*) &LSMap<LSValue*, double>::in, Method::NATIVE},
		{Type::PTR_INT_MAP, Type::POINTER, Type::BOOLEAN, (void*) &LSMap<LSValue*, int>::in, Method::NATIVE},
		{Type::REAL_PTR_MAP, Type::REAL, Type::BOOLEAN, (void*) &LSMap<double, LSValue*>::in, Method::NATIVE},
		{Type::REAL_REAL_MAP, Type::REAL, Type::BOOLEAN, (void*) &LSMap<double, double>::in, Method::NATIVE},
		{Type::REAL_INT_MAP, Type::REAL, Type::BOOLEAN, (void*) &LSMap<double, int>::in, Method::NATIVE},
		{Type::INT_PTR_MAP, Type::INTEGER, Type::BOOLEAN, (void*) &LSMap<int, LSValue*>::in, Method::NATIVE},
		{Type::INT_REAL_MAP, Type::INTEGER, Type::BOOLEAN, (void*) &LSMap<int, double>::in, Method::NATIVE},
		{Type::INT_INT_MAP, Type::LONG, Type::BOOLEAN, (void*) &LSMap<int, int>::in, Method::NATIVE},
	});

	method("size", {
		{Type::INTEGER, {Type::PTR_PTR_MAP}, (void*) map_size, Method::NATIVE},
		{Type::INTEGER, {Type::PTR_REAL_MAP}, (void*) map_size, Method::NATIVE},
		{Type::INTEGER, {Type::PTR_INT_MAP}, (void*) map_size, Method::NATIVE},
		{Type::INTEGER, {Type::INT_PTR_MAP}, (void*) map_size, Method::NATIVE},
		{Type::INTEGER, {Type::INT_REAL_MAP}, (void*) map_size, Method::NATIVE},
		{Type::INTEGER, {Type::INT_INT_MAP}, (void*) map_size, Method::NATIVE},
    });

	method("values", {
		{Type::PTR_ARRAY, {Type::PTR_PTR_MAP}, (void*) &LSMap<LSValue*, LSValue*>::values, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::PTR_REAL_MAP}, (void*) &LSMap<LSValue*, double>::values, Method::NATIVE},
		{Type::INT_ARRAY, {Type::PTR_INT_MAP}, (void*) &LSMap<LSValue*, int>::values, Method::NATIVE},
		{Type::PTR_ARRAY, {Type::REAL_PTR_MAP}, (void*) &LSMap<double, LSValue*>::values, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::REAL_REAL_MAP}, (void*) &LSMap<double, double>::values, Method::NATIVE},
		{Type::INT_ARRAY, {Type::REAL_INT_MAP}, (void*) &LSMap<double, int>::values, Method::NATIVE},
		{Type::PTR_ARRAY, {Type::INT_PTR_MAP}, (void*) &LSMap<int, LSValue*>::values, Method::NATIVE},
		{Type::REAL_ARRAY, {Type::INT_REAL_MAP}, (void*) &LSMap<int, double>::values, Method::NATIVE},
		{Type::INT_ARRAY, {Type::INT_INT_MAP}, (void*) &LSMap<int, int>::values, Method::NATIVE}
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
		{Type::PTR_PTR_MAP, {Type::PTR_PTR_MAP}, (void*) &LSMap<LSValue*,LSValue*>::ls_clear, Method::NATIVE},
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
		{Type::POINTER, {Type::PTR_PTR_MAP, Type::POINTER, Type::POINTER}, (void*) &LSMap<LSValue*,LSValue*>::ls_look, Method::NATIVE},
		{Type::REAL, {Type::PTR_REAL_MAP, Type::POINTER, Type::REAL}, (void*) &LSMap<LSValue*,double>::ls_look, Method::NATIVE},
		{Type::INTEGER, {Type::PTR_INT_MAP, Type::POINTER, Type::INTEGER}, (void*) &LSMap<LSValue*,int>::ls_look, Method::NATIVE},
		{Type::POINTER, {Type::INT_PTR_MAP, Type::INTEGER, Type::POINTER}, (void*) &LSMap<int,LSValue*>::ls_look, Method::NATIVE},
		{Type::REAL, {Type::INT_REAL_MAP, Type::INTEGER, Type::REAL}, (void*) &LSMap<int,double>::ls_look, Method::NATIVE},
		{Type::INTEGER, {Type::INT_INT_MAP, Type::INTEGER, Type::INTEGER}, (void*) &LSMap<int,int>::ls_look, Method::NATIVE},
	});

	method("min", {
		{Type::POINTER, {Type::PTR_PTR_MAP}, (void*) &LSMap<LSValue*,LSValue*>::ls_min, Method::NATIVE},
		{Type::REAL, {Type::PTR_REAL_MAP}, (void*) &LSMap<LSValue*,double>::ls_min, Method::NATIVE},
		{Type::INTEGER, {Type::PTR_INT_MAP}, (void*) &LSMap<LSValue*,int>::ls_min, Method::NATIVE},
		{Type::POINTER, {Type::INT_PTR_MAP}, (void*) &LSMap<int,LSValue*>::ls_min, Method::NATIVE},
		{Type::REAL, {Type::INT_REAL_MAP}, (void*) &LSMap<int,double>::ls_min, Method::NATIVE},
		{Type::INTEGER, {Type::INT_INT_MAP}, (void*) &LSMap<int,int>::ls_min, Method::NATIVE},
	});

	method("minKey", {
		{Type::POINTER, {Type::PTR_PTR_MAP}, (void*) &LSMap<LSValue*,LSValue*>::ls_minKey, Method::NATIVE},
		{Type::POINTER, {Type::PTR_REAL_MAP}, (void*) &LSMap<LSValue*,double>::ls_minKey, Method::NATIVE},
		{Type::POINTER, {Type::PTR_INT_MAP}, (void*) &LSMap<LSValue*,int>::ls_minKey, Method::NATIVE},
		{Type::INTEGER, {Type::INT_PTR_MAP}, (void*) &LSMap<int,LSValue*>::ls_minKey, Method::NATIVE},
		{Type::INTEGER, {Type::INT_REAL_MAP}, (void*) &LSMap<int,double>::ls_minKey, Method::NATIVE},
		{Type::INTEGER, {Type::INT_INT_MAP}, (void*) &LSMap<int,int>::ls_minKey, Method::NATIVE},
	});

	method("max", {
		{Type::PTR_PTR_MAP, Type::POINTER, {}, (void*) &LSMap<LSValue*,LSValue*>::ls_max, Method::NATIVE},
		{Type::PTR_REAL_MAP, Type::REAL, {}, (void*) &LSMap<LSValue*,double>::ls_max, Method::NATIVE},
		{Type::PTR_INT_MAP, Type::INTEGER, {}, (void*) &LSMap<LSValue*,int>::ls_max, Method::NATIVE},
		{Type::INT_PTR_MAP, Type::POINTER, {}, (void*) &LSMap<int,LSValue*>::ls_max, Method::NATIVE},
		{Type::INT_REAL_MAP, Type::REAL, {}, (void*) &LSMap<int,double>::ls_max, Method::NATIVE},
		{Type::INT_INT_MAP, Type::INTEGER, {}, (void*) &LSMap<int,int>::ls_max, Method::NATIVE},
	});

	method("maxKey", {
		{Type::PTR_PTR_MAP, Type::POINTER, {}, (void*) &LSMap<LSValue*,LSValue*>::ls_maxKey, Method::NATIVE},
		{Type::PTR_REAL_MAP, Type::POINTER, {}, (void*) &LSMap<LSValue*,double>::ls_maxKey, Method::NATIVE},
		{Type::PTR_INT_MAP, Type::POINTER, {}, (void*) &LSMap<LSValue*,int>::ls_maxKey, Method::NATIVE},
		{Type::INT_PTR_MAP, Type::INTEGER, {}, (void*) &LSMap<int,LSValue*>::ls_maxKey, Method::NATIVE},
		{Type::INT_REAL_MAP, Type::INTEGER, {}, (void*) &LSMap<int,double>::ls_maxKey, Method::NATIVE},
		{Type::INT_INT_MAP, Type::INTEGER, {}, (void*) &LSMap<int,int>::ls_maxKey, Method::NATIVE},
	});

	Type iter_ptr_ptr = Type::FUNCTION_P;
	iter_ptr_ptr.setArgumentType(0, Type::POINTER);
	iter_ptr_ptr.setArgumentType(1, Type::POINTER);
	iter_ptr_ptr.setReturnType(Type::VOID);

	Type iter_ptr_real = Type::FUNCTION_P;
	iter_ptr_real.setArgumentType(0, Type::POINTER);
	iter_ptr_real.setArgumentType(1, Type::REAL);
	iter_ptr_real.setReturnType(Type::VOID);

	Type iter_ptr_int = Type::FUNCTION_P;
	iter_ptr_int.setArgumentType(0, Type::POINTER);
	iter_ptr_int.setArgumentType(1, Type::INTEGER);
	iter_ptr_int.setReturnType(Type::VOID);

	Type iter_int_ptr = Type::FUNCTION_P;
	iter_int_ptr.setArgumentType(0, Type::INTEGER);
	iter_int_ptr.setArgumentType(1, Type::POINTER);
	iter_int_ptr.setReturnType(Type::VOID);

	Type iter_int_real = Type::FUNCTION_P;
	iter_int_real.setArgumentType(0, Type::INTEGER);
	iter_int_real.setArgumentType(1, Type::REAL);
	iter_int_real.setReturnType(Type::VOID);

	Type iter_int_int = Type::FUNCTION_P;
	iter_int_int.setArgumentType(0, Type::INTEGER);
	iter_int_int.setArgumentType(1, Type::INTEGER);
	iter_int_int.setReturnType(Type::VOID);

	method("iter", {
		{Type::VOID, {Type::PTR_PTR_MAP, iter_ptr_ptr}, (void*) &LSMap<LSValue*, LSValue*>::ls_iter, Method::NATIVE},
		{Type::VOID, {Type::PTR_REAL_MAP, iter_ptr_real}, (void*) &LSMap<LSValue*, double>::ls_iter, Method::NATIVE},
		{Type::VOID, {Type::PTR_INT_MAP, iter_ptr_int}, (void*) &LSMap<LSValue*, int>::ls_iter, Method::NATIVE},
		{Type::VOID, {Type::INT_PTR_MAP, iter_int_ptr}, (void*) &LSMap<int, LSValue*>::ls_iter, Method::NATIVE},
		{Type::VOID, {Type::INT_REAL_MAP, iter_int_real}, (void*) &LSMap<int, double>::ls_iter, Method::NATIVE},
		{Type::VOID, {Type::INT_INT_MAP, iter_int_int}, (void*) &LSMap<int, int>::ls_iter, Method::NATIVE},
	});
}

}
