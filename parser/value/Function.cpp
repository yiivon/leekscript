#include "Function.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../../vm/VM.hpp"

using namespace std;

Function::Function() {
	body = nullptr;
	pos = 0;
	constant = true;
	type = Type::FUNCTION;

//	cout << "function:" << this << endl;
}

Function::~Function() {}

void Function::addArgument(Token* name, bool reference, Value* defaultValue) {
	arguments.push_back(name);
	references.push_back(reference);
	defaultValues.push_back(defaultValue);
}

void Function::print(ostream& os) const {

	if (lambda) {

		cout << "(";
		for (unsigned i = 0; i < arguments.size(); ++i) {
			os << arguments.at(i);
			if (i < arguments.size() - 1) {
				os << ", ";
			}
		}
		os << " -> ";
		body->instructions[0]->print(os);
		cout << ")";

	} else {

		os << "function(";
		for (unsigned i = 0; i < arguments.size(); ++i) {
			if (references.at(i)) {
				os << "@";
			}
			os << arguments.at(i);
			if ((Value*)defaultValues.at(i) != nullptr) {
				os << " = ";
				defaultValues.at(i)->print(os);
			}
			if (i < arguments.size() - 1) {
				os << ", ";
			}
		}

		os << ")" << endl;
		body->print(os);
		os << "end";
	}
}

void Function::analyse(SemanticAnalyser* analyser, const Type req_type) {

	if (!function_added) {
		analyser->add_function(this);
		function_added = true;
	}

	for (auto t : req_type.getArgumentTypes()) {
		type.addArgumentType(t);
	}
	type.setReturnType(req_type.getReturnType());

	analyse_body(analyser, req_type);

//	cout << "Function type: " << type << endl;
}

bool Function::will_take(SemanticAnalyser* analyser, const unsigned pos, const Type arg_type) {

//	cout << "function will_take " << type << endl;

	bool changed = type.will_take(pos, arg_type);

	//cout << "function after will_take " << type << endl;

//	if (changed) {
		analyse_body(analyser, type);
//	}

	return changed;
}

void Function::analyse_body(SemanticAnalyser* analyser, const Type& req_type) {

	analyser->enter_function(this);

	for (unsigned i = 0; i < arguments.size(); ++i) {
		analyser->add_parameter(arguments[i], type.getArgumentType(i));
	}

	body->analyse(analyser, req_type);

	//cout << "body type: " << body->type << endl;

	type.setReturnType(body->type);

	vars = analyser->get_local_vars();

	analyser->leave_function();

//	cout << "function return : " << type.getReturnType() << endl;
}

jit_value_t Function::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

//	cout << "compile fun: " << type << endl;

	jit_context_t context = jit_context_create();
	jit_context_build_start(context);

	unsigned arg_count = arguments.size();
	vector<jit_type_t> params;
	for (unsigned i = 0; i < arg_count; ++i) {
		Type t = Type::INTEGER;
		if (i < type.getArgumentTypes().size()) {
			t = type.getArgumentType(i);
		}
//		cout << "func arg: " << t << endl;
		params.push_back(t.nature == Nature::POINTER ? JIT_POINTER :
				(t.raw_type == RawType::FLOAT) ? JIT_FLOAT :
				(t.raw_type == RawType::LONG) ? JIT_INTEGER_LONG :
				JIT_INTEGER);
	}

	jit_type_t return_type = type.getReturnType().nature == Nature::POINTER ? JIT_POINTER : (
			(type.getReturnType().raw_type == RawType::FUNCTION) ? JIT_POINTER :
			(type.getReturnType().raw_type == RawType::LONG) ? JIT_INTEGER_LONG :
			(type.getReturnType().raw_type == RawType::FLOAT) ? JIT_FLOAT :
			JIT_INTEGER);

	jit_type_t signature = jit_type_create_signature(jit_abi_cdecl, return_type, params.data(), arg_count, 1);

	jit_function_t function = jit_function_create(context, signature);

	jit_value_t res = body->compile_jit(c, function, type.getReturnType());
	jit_insn_return(function, res);

	jit_function_compile(function);
	jit_context_build_end(context);

	void* f = jit_function_to_closure(function);

	if (req_type.nature == Nature::POINTER) {
//		cout << "create function pointer " << endl;
		LSFunction* fo = new LSFunction(f);
		return JIT_CREATE_CONST_POINTER(F, fo);
	} else {
//		cout << "create function value " << endl;
		return JIT_CREATE_CONST_POINTER(F, f);
	}
}
