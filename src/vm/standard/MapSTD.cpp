#include "MapSTD.hpp"
#include "../value/LSNumber.hpp"
#include "../value/LSMap.hpp"

using namespace std;

namespace ls {


MapSTD::MapSTD() : Module("Map") {

	method("size", Type::MAP, Type::INTEGER, {}, (void*) map_size);

	method("insert", {
			   {Type::PTR_PTR_MAP, Type::PTR_PTR_MAP, {Type::POINTER, Type::POINTER}, (void*) map_insert_ptr_ptr},
			   {Type::PTR_FLOAT_MAP, Type::PTR_FLOAT_MAP, {Type::POINTER, Type::FLOAT}, (void*) map_insert_ptr_float},
			   {Type::PTR_INT_MAP, Type::PTR_INT_MAP, {Type::POINTER, Type::INTEGER}, (void*) map_insert_ptr_int},
			   {Type::INT_PTR_MAP, Type::INT_PTR_MAP, {Type::INTEGER, Type::POINTER}, (void*) map_insert_int_ptr},
			   {Type::INT_FLOAT_MAP, Type::INT_FLOAT_MAP, {Type::INTEGER, Type::FLOAT}, (void*) map_insert_int_float},
			   {Type::INT_INT_MAP, Type::INT_INT_MAP, {Type::INTEGER, Type::INTEGER}, (void*) map_insert_int_int},
		   });

	method("clear", {
			   {Type::PTR_PTR_MAP, Type::PTR_PTR_MAP, {}, (void*) &LSMap<LSValue*,LSValue*>::ls_clear},
			   {Type::PTR_FLOAT_MAP, Type::PTR_FLOAT_MAP, {}, (void*) &LSMap<LSValue*,double>::ls_clear},
			   {Type::PTR_INT_MAP, Type::PTR_INT_MAP, {}, (void*) &LSMap<LSValue*,int>::ls_clear},
			   {Type::INT_PTR_MAP, Type::INT_PTR_MAP, {}, (void*) &LSMap<int,LSValue*>::ls_clear},
			   {Type::INT_FLOAT_MAP, Type::INT_FLOAT_MAP, {}, (void*) &LSMap<int,double>::ls_clear},
			   {Type::INT_INT_MAP, Type::INT_INT_MAP, {}, (void*) &LSMap<int,int>::ls_clear},
		   });

	method("erase", {
			   {Type::PTR_PTR_MAP, Type::PTR_PTR_MAP, {Type::POINTER}, (void*) &LSMap<LSValue*,LSValue*>::ls_erase},
			   {Type::PTR_FLOAT_MAP, Type::PTR_FLOAT_MAP, {Type::POINTER}, (void*) &LSMap<LSValue*,double>::ls_erase},
			   {Type::PTR_INT_MAP, Type::PTR_INT_MAP, {Type::POINTER}, (void*) &LSMap<LSValue*,int>::ls_erase},
			   {Type::INT_PTR_MAP, Type::INT_PTR_MAP, {Type::INTEGER}, (void*) &LSMap<int,LSValue*>::ls_erase},
			   {Type::INT_FLOAT_MAP, Type::INT_FLOAT_MAP, {Type::INTEGER}, (void*) &LSMap<int,double>::ls_erase},
			   {Type::INT_INT_MAP, Type::INT_INT_MAP, {Type::INTEGER}, (void*) &LSMap<int,int>::ls_erase},
		   });
}

int map_size(const LSMap<LSValue*,LSValue*>* map) {
	return (int) map->size();
}

LSMap<LSValue*,LSValue*>* map_insert_ptr_ptr(LSMap<LSValue*,LSValue*>* map, LSValue* key, LSValue* value) {
	return map->ls_insert(make_pair(key, value));
}
LSMap<LSValue*,int>* map_insert_ptr_int(LSMap<LSValue*,int>* map, LSValue* key, int value) {
	return map->ls_insert(make_pair(key, value));
}
LSMap<LSValue*,double>* map_insert_ptr_float(LSMap<LSValue*,double>* map, LSValue* key, double value) {
	return map->ls_insert(make_pair(key, value));
}
LSMap<int,LSValue*>* map_insert_int_ptr(LSMap<int,LSValue*>* map, int key, LSValue* value) {
	return map->ls_insert(make_pair(key, value));
}
LSMap<int,int>* map_insert_int_int(LSMap<int,int>* map, int key, int value) {
	return map->ls_insert(make_pair(key, value));
}
LSMap<int,double>* map_insert_int_float(LSMap<int,double>* map, int key, double value) {
	return map->ls_insert(make_pair(key, value));
}


}
