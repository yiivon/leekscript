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

//	cout << "Analyse oa : " << field->content << " => ";
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

		ModuleStaticField& mod_field = std_class->static_fields[field->content];
		type = mod_field.type;

		if (mod_field.fun != nullptr) {
			access_function = mod_field.fun;
		}
	}

	// Search class attributes
	object_class = object->type.clazz;

//	cout << "Classe : " << object_class << endl;

	if (analyser->program->system_vars.find(object_class) != analyser->program->system_vars.end()) {

//		LSClass* std_class = (LSClass*) analyser->program->system_vars[object_class];

//		cout << "Classe ! ";
//		std_class->print(cout);
//		cout << endl;

		try {
//			type = std_class->fields[field->content];
//			cout << "Field " << field->content << " in class " << std_class << " found." << endl;
//			cout << "(type " << type << ")" << endl;
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

//	cout << "object_access '" << field->content << "' type : " << type << endl;
}

LSValue* object_access(LSValue* o, LSString* k) {
	return o->attr(k);
}

LSValue** object_access_l(LSValue* o, LSString* k) {
	return o->attrL(k);
}


jit_value_t ObjectAccess::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

	// Special case for custom attributes, accessible via a function
	if (access_function != nullptr) {

		auto fun = (jit_value_t (*)(jit_function_t&)) access_function;
		jit_value_t res = fun(F);

		if (req_type.nature == Nature::POINTER) {
			return VM::value_to_pointer(F, res, type);
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
