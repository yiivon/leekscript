#include "MapSTD.hpp"
#include "../value/LSNumber.hpp"
#include "../value/LSMap.hpp"

using namespace std;

namespace ls {


MapSTD::MapSTD() : Module("Map") {

	method("size", Type::MAP, Type::INTEGER, {}, (void*) map_size);

	method("insert", {
	   {Type::PTR_PTR_MAP, Type::PTR_PTR_MAP, {Type::POINTER}, (void*) &LSMap<LSValue*,LSValue*>::insert},
	   {Type::PTR_FLOAT_MAP, Type::PTR_FLOAT_MAP, {Type::FLOAT}, (void*) &LSMap<LSValue*,double>::insert},
	   {Type::PTR_INT_MAP, Type::PTR_INT_MAP, {Type::INTEGER}, (void*) &LSMap<LSValue*,int>::insert},
	   {Type::INT_PTR_MAP, Type::INT_PTR_MAP, {Type::POINTER}, (void*) &LSMap<int,LSValue*>::insert},
	   {Type::INT_FLOAT_MAP, Type::INT_FLOAT_MAP, {Type::FLOAT}, (void*) &LSMap<int,double>::insert},
	   {Type::INT_INT_MAP, Type::INT_INT_MAP, {Type::INTEGER}, (void*) &LSMap<int,int>::insert},
   });
}

int map_size(const LSMap<LSValue*,LSValue*>* map) {
	return (int) map->size();
}

}
