#include "Array.hpp"
#include "../../vm/VM.hpp"

using namespace std;

Array::Array() {
	type = Type::ARRAY;
}

void Array::addValue(Value* value, Value* key) {

	expressions.push_back(value);
	keys.push_back(key);
	if (key != nullptr) {
		associative = true;
	}
}

Array::~Array() {}

void Array::print(ostream& os) const {
	os << "[";
	for (unsigned i = 0; i < expressions.size(); ++i) {
		if (associative && (Value*)keys.at(i) != nullptr) {
			keys.at(i)->print(os);
			os << " : ";
		}
		expressions.at(i)->print(os);
		if (i < expressions.size() - 1) {
			os << ", ";
		}
	}
	os << "]";
}

void Array::analyse(SemanticAnalyser* analyser, const Type) {

	constant = true;
	only_values = true;

	for (Expression key : keys) {
		key.analyse(analyser, Type::NEUTRAL);
	}

	for (unsigned i = 0; i < expressions.size(); ++i) {

		Value* ex = expressions[i];

		ex->analyse(analyser, Type::NEUTRAL);

		if (ex->constant == false) {
			constant = false;
		}
		if (ex->type.nature != Nature::VALUE) {
			only_values = false;
		}
		if (i > 0 && expressions[i - 1]->type != ex->type) {
			type.homogeneous = false;
		}
		if (ex->type.raw_type != RawType::ARRAY or ex->type.getElementType().raw_type != RawType::UNKNOWN) {
			type.setElementType(ex->type);
		}
	}

	if (!type.homogeneous) {
		Type pointer = Type::POINTER;
		pointer.setReturnType(Type::POINTER);
		for (unsigned i = 0; i < expressions.size(); ++i) {
			Value* ex = expressions[i];
			ex->analyse(analyser, pointer);
		}
	}

//	cout << "Array only values : " << only_values << endl;
//	cout << "Array type : " << type << endl;
}

void Array::elements_will_take(SemanticAnalyser* analyser, const unsigned pos, const Type& type, int level) {

	for (unsigned i = 0; i < expressions.size(); ++i) {

		Array* arr = dynamic_cast<Array*>(expressions[i]);
		if (arr != nullptr and level > 0) {
			arr->elements_will_take(analyser, pos, type, level - 1);
		} else {
			expressions[i]->will_take(analyser, pos, type);
		}
	}
}

LSArray* LSArray_create() {
	return new LSArray();
}
void LSArray_push(LSArray* array, LSValue* value) {
	array->pushClone(value);
}

jit_value_t Array::compile_jit(Compiler& c, jit_function_t& F, Type) const {

	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, {}, 0, 0);
	jit_value_t array = jit_insn_call_native(F, "new", (void*) LSArray_create, sig, {}, 0, JIT_CALL_NOTHROW);

	for (Value* val : expressions) {

		jit_value_t v = val->compile_jit(c, F, Type::POINTER);

		jit_type_t args[2] = {JIT_POINTER, JIT_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 2, 0);
		jit_value_t args_v[] = {array, v};
		jit_insn_call_native(F, "push", (void*) LSArray_push, sig, args_v, 2, JIT_CALL_NOTHROW);
	}

	return array;
}
