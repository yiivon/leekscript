#include "ObjectAccess.hpp"

#include <chrono>

#include "../../compiler/semantic/SemanticAnalyser.hpp"
#include "VariableValue.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSFunction.hpp"
#include "../../vm/value/LSClass.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Module.hpp"

using namespace std;

namespace ls {

ObjectAccess::ObjectAccess() {
	field = nullptr;
	object = nullptr;
	type = Type::POINTER;
	class_attr = false;
	attr_addr = nullptr;
}

ObjectAccess::~ObjectAccess() {
	delete object;
}

void ObjectAccess::print(ostream& os) const {
	object->print(os);
	os << "." << field->content;
}

void ObjectAccess::analyse(SemanticAnalyser* analyser, const Type) {

	object->analyse(analyser);

//	cout << "Analyse oa : " << field << " => ";
//	cout << object->type << endl;

	// Search direct attributes
	try {
		//type = object->attr_types.at(field);
		//cout << "Type of " << field << " : " << type << endl;
	} catch (exception&) {}

//	cout << type << endl;

	// Static attribute
	VariableValue* vv = dynamic_cast<VariableValue*>(object);
	if (object->type == Type::CLASS and vv != nullptr) {

		class_name = vv->name->content;

		LSClass* std_class = (LSClass*) analyser->program->system_vars[class_name];

		type = std_class->static_fields[field->content].type;

//		cout << "static field type : " << type << endl;
	}


	// Search class attributes
	object_class = object->type.clazz;

//	cout << "Classe : " << clazz << endl;

	if (analyser->program->system_vars.find(object_class) != analyser->program->system_vars.end()) {

		LSClass* std_class = (LSClass*) analyser->program->system_vars[object_class];

//		cout << "Classe ! ";
//		std_class->print(cout);
//		cout << endl;

		try {
			type = std_class->fields[field->content];
//			cout << "Field " << field << " in class " << clazz << " found." << endl;
			//cout << "(type " << type << ")" << endl;
		} catch (exception& e) {

		}

		auto types = analyser->internal_vars[object_class]->attr_types;

//		cout << "search type of " << field << endl;

		if (types.find(field->content) != types.end()) {

//			cout << " oa " << field << endl;

			//type = types[field];
			//class_attr = true;

			// TODO : the attr must be a function here, not working with other types
			//attr_addr = ((LSFunction*) std_class->static_fields[field])->function;

			//attr_addr = std_class->getDefaultMethod(field)->function;
		}
	}

//	cout << "object_access '" << field << "' type : " << type << endl;
}

LSValue* object_access(LSValue* o, LSString* k) {
	return o->attr(k);
}

LSValue** object_access_l(LSValue* o, LSString* k) {
	return o->attrL(k);
}

long get_sec_time() {
	return std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long get_milli_time() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long get_micro_time() {
	return std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

long get_nano_time() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

jit_value_t ObjectAccess::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

	// Special case for System attrs
	// TODO generalize those custom attributes
	if (class_name == "System") {

		jit_value_t res;

		if (field->content == "operations") {

			jit_value_t jit_ops_ptr = jit_value_create_long_constant(F, jit_type_void_ptr, (long int) &VM::operations);
			res = jit_insn_load_relative(F, jit_ops_ptr, 0, jit_type_uint);

		} else if (field->content == "time") {

			jit_type_t args_types[0] = {};
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_INTEGER_LONG, args_types, 0, 0);
			res = jit_insn_call_native(F, "sec_time", (void*) get_sec_time, sig, {}, 0, JIT_CALL_NOTHROW);

		} else if (field->content == "milliTime") {

			jit_type_t args_types[0] = {};
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_INTEGER_LONG, args_types, 0, 0);
			res = jit_insn_call_native(F, "milli_time", (void*) get_milli_time, sig, {}, 0, JIT_CALL_NOTHROW);

		} else if (field->content == "microTime") {

			jit_type_t args_types[0] = {};
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_INTEGER_LONG, args_types, 0, 0);
			res = jit_insn_call_native(F, "micro_time", (void*) get_micro_time, sig, {}, 0, JIT_CALL_NOTHROW);

		} else if (field->content == "nanoTime") {

			jit_type_t args_types[0] = {};
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_INTEGER_LONG, args_types, 0, 0);
			res = jit_insn_call_native(F, "nano_time", (void*) get_nano_time, sig, {}, 0, JIT_CALL_NOTHROW);
		}
		if (req_type.nature == Nature::POINTER) {
			return VM::value_to_pointer(F, res, Type::LONG);
		}
		return res;
	}

	if (class_attr) {

		// TODO : only functions!
		return JIT_CREATE_CONST_POINTER(F, new LSFunction(attr_addr));

	} else {

		jit_value_t o = object->compile_jit(c, F, Type::POINTER);

		jit_type_t args_types[2] = {JIT_POINTER, JIT_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 2, 0);

		jit_value_t k = JIT_CREATE_CONST_POINTER(F, new LSString(field->content));
		jit_value_t args[] = {o, k};

		jit_value_t res = jit_insn_call_native(F, "access", (void*) object_access, sig, args, 2, JIT_CALL_NOTHROW);

		VM::delete_temporary(F, o);
		VM::delete_temporary(F, k);
		return res;
	}
}

jit_value_t ObjectAccess::compile_jit_l(Compiler& c, jit_function_t& F, Type) const {

	jit_value_t o = object->compile_jit(c, F, Type::POINTER);

	jit_type_t args_types[2] = {JIT_POINTER, JIT_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types, 2, 0);

	jit_value_t k = JIT_CREATE_CONST_POINTER(F, new LSString(field->content));
	jit_value_t args[] = {o, k};
	return jit_insn_call_native(F, "access_l", (void*) object_access_l, sig, args, 2, JIT_CALL_NOTHROW);
}

}
