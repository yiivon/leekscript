#include "Map.hpp"

using namespace std;

namespace ls {

Map::Map() {

}

Map::~Map()
{
	for (auto ex : keys) {
		delete ex;
	}
	for (auto ex : expressions) {
		delete ex;
	}
}

void Map::print(std::ostream &, int indent, bool debug) const {

}

unsigned Map::line() const {

}

bool Map::will_take(SemanticAnalyser*, const unsigned, const Type) {

}

bool Map::will_take_element(SemanticAnalyser*, const Type) {

}

bool Map::must_be_pointer(SemanticAnalyser*) {

}

void Map::must_return(SemanticAnalyser*, const Type&) {

}

void Map::analyse(SemanticAnalyser*, const Type&) {

}

jit_value_t Map::compile(Compiler &c) const {

//	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, {}, 0, 0);

//	jit_value_t array = jit_insn_call_native(c.F, "new", (void*) LSArray_create_map, sig, {}, 0, JIT_CALL_NOTHROW);

//	for (unsigned i = 0; i < expressions.size(); ++i) {

//		jit_value_t v = expressions[i]->compile(c);
//		jit_value_t k = keys[i]->compile(c);

//		jit_type_t args[3] = {JIT_POINTER, JIT_POINTER, JIT_POINTER};
//		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 3, 0);
//		jit_value_t args_v[] = {array, k, v};
//		jit_insn_call_native(c.F, "push", (void*) Array_push_map, sig, args_v, 3, JIT_CALL_NOTHROW);
//	}

//	return array;
}

}
