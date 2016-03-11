#include "../../vm/VM.hpp"
#include "FunctionCall.hpp"
#include "VariableValue.hpp"
#include "Function.hpp"
#include "ObjectAccess.hpp"
#include <math.h>
#include "../semantic/SemanticAnalyser.hpp"
#include "../../vm/standard/ArraySTD.hpp"
#include "../../vm/standard/NumberSTD.hpp"

FunctionCall::FunctionCall() {
	function = nullptr;
	type = Type::VALUE;
	std_func = nullptr;
	this_ptr = nullptr;
}

FunctionCall::~FunctionCall() {}

void FunctionCall::print(ostream& os) const {
	function->print(os);
	os << "(";
	for (unsigned i = 0; i < arguments.size(); ++i) {
		arguments.at(i)->print(os);
		if (i < arguments.size() - 1) {
			os << ", ";
		}
	}
	os << ")";
}

void FunctionCall::analyse(SemanticAnalyser* analyser, const Type) {

	constant = false;

	function->analyse(analyser);

//	cout << "Function call function type : " << function->type << endl;

	// Detect standard library functions
	ObjectAccess* oa = dynamic_cast<ObjectAccess*>(function);
	if (oa != nullptr) {

		VariableValue* vv = dynamic_cast<VariableValue*>(oa->object);
		if (vv != nullptr and vv->name->content == "Number") {
			is_native = true;
			if (oa->field == "abs") {
				function->type.setArgumentType(0, Type::INTEGER);
				function->type.setReturnType(Type::INTEGER);
			} else if (oa->field == "floor") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setReturnType(Type::INTEGER);
			} else if (oa->field == "round") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setReturnType(Type::FLOAT);
			} else if (oa->field == "ceil") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setReturnType(Type::INTEGER);
			} else if (oa->field == "cos") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setReturnType(Type::FLOAT);
			} else if (oa->field == "sin") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setReturnType(Type::FLOAT);
			} else if (oa->field == "max") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setArgumentType(1, Type::FLOAT);
				function->type.setReturnType(Type::FLOAT);
			} else if (oa->field == "min") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setArgumentType(1, Type::FLOAT);
				function->type.setReturnType(Type::FLOAT);
			} else if (oa->field == "sqrt") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setReturnType(Type::FLOAT);
			} else if (oa->field == "pow") {
				function->type.setArgumentType(0, Type::FLOAT);
				function->type.setArgumentType(1, Type::FLOAT);
				function->type.setReturnType(Type::FLOAT);
			}
			native_func = oa->field;
		}

		string clazz = oa->object->type.clazz;

		LSClass* std_class = (LSClass*) analyser->program->system_vars[clazz];

		if (std_class) {

			this_ptr = oa->object;

			std_func = ((LSFunction*) std_class->static_fields[oa->field])->function;

			function->type = analyser->internal_vars[clazz]->attr_types[oa->field];
		}
	}

	int a = 0;
	if (this_ptr != nullptr) {
		a = 1; // Argument offset for standard functions
	}
	for (Value* arg : arguments) {
		arg->analyse(analyser, function->type.getArgumentType(a));
		a++;
	}

	a = 0;
	for (Value* arg : arguments) {
		function->will_take(analyser, a++, arg->type);
	}

	// The function is a variable
	VariableValue* vv = dynamic_cast<VariableValue*>(function);
	if (vv and vv->var->value != nullptr) {
//		cout << "function call, fun is vv: " << vv->var << endl;
		a = 0;
		for (Value* arg : arguments) {
//			cout << "arg " << a << " " << arg->type << endl;
			vv->var->will_take(analyser, a++, arg->type);
		}
		type = vv->var->value->type.getReturnType();
	} else {
		type = function->type.getReturnType();
	}

//	cout << "Function call function type : " << function->type << endl;
}

void func_print(LSValue* v) {
	cout << " >>> ";
	v->print(cout);
	cout << endl;
}
void func_print_int(int v) {
	cout << " >>> " << v << endl;
}

LSValue* create_float_object_3(double n) {
	return LSNumber::get(n);
}
LSValue* create_int_object_3(int n) {
	return LSNumber::get(n);
}

jit_value_t FunctionCall::compile_jit(Compiler& c, jit_function_t& F, Type req_type) const {

//	cout << "compile function call" << endl;
//	cout << type << endl;

	/*
	 * Standard library constructors
	 */
	VariableValue* vv = dynamic_cast<VariableValue*>(function);
	if (vv != nullptr) {
		if (vv->name->content == "Number") {
			jit_value_t n = jit_value_create_nint_constant(F, JIT_INTEGER, 0);
			if (req_type.nature == Nature::POINTER) {
				return VM::value_to_pointer(F, n, Type::INTEGER);
			}
			return n;
		}
		if (vv->name->content == "String") {
			if (arguments.size() > 0) {
				return arguments[0]->compile_jit(c, F, Type::POINTER);
			}
			return JIT_CREATE_CONST_POINTER(F, new LSString(""));
		}
		if (vv->name->content == "Array") {
			return JIT_CREATE_CONST_POINTER(F, new LSArray());
		}
	}

	/*
	 * Compile native standard functions
	 */
	if (is_native) {

		jit_value_t res;

		if (native_func == "abs") {
			jit_value_t v = arguments[0]->compile_jit(c, F, Type::VALUE);
			res = jit_insn_abs(F, v);
		} else if (native_func == "floor") {
			jit_value_t v = arguments[0]->compile_jit(c, F, Type::VALUE);
			res = jit_insn_floor(F, v);
		} else if (native_func == "round") {
			if (req_type.nature == Nature::VALUE) {
				jit_value_t v = arguments[0]->compile_jit(c, F, Type::VALUE);
				res = jit_insn_round(F, v);
			} else {
				vector<jit_value_t> args = { arguments[0]->compile_jit(c, F, Type::POINTER) };
				vector<jit_type_t> args_types = { JIT_POINTER };
				jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types.data(), 1, 0);
				res = jit_insn_call_native(F, "round", (void*) &number_round, sig, args.data(), 1, JIT_CALL_NOTHROW);
				return res;
			}
		} else if (native_func == "ceil") {
			jit_value_t v = arguments[0]->compile_jit(c, F, Type::VALUE);
			res = jit_insn_ceil(F, v);
		} else if (native_func == "cos") {
			if (req_type.nature == Nature::VALUE) {
				jit_value_t v = arguments[0]->compile_jit(c, F, Type::VALUE);
				res = jit_insn_cos(F, v);
			} else {
				vector<jit_value_t> args = { arguments[0]->compile_jit(c, F, Type::POINTER) };
				vector<jit_type_t> args_types = { JIT_POINTER };
				jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types.data(), 1, 0);
				res = jit_insn_call_native(F, "round", (void*) &number_cos, sig, args.data(), 1, JIT_CALL_NOTHROW);
				return res;
			}
		} else if (native_func == "sin") {
			jit_value_t v = arguments[0]->compile_jit(c, F, Type::VALUE);
			res = jit_insn_sin(F, v);
		} else if (native_func == "max") {
			jit_value_t v1 = arguments[0]->compile_jit(c, F, Type::VALUE);
			jit_value_t v2 = arguments[1]->compile_jit(c, F, Type::VALUE);
			res = jit_insn_max(F, v1, v2);
		} else if (native_func == "min") {
			jit_value_t v1 = arguments[0]->compile_jit(c, F, Type::VALUE);
			jit_value_t v2 = arguments[1]->compile_jit(c, F, Type::VALUE);
			res = jit_insn_min(F, v1, v2);
		} else if (native_func == "sqrt") {
			jit_value_t v1 = arguments[0]->compile_jit(c, F, Type::VALUE);
			res = jit_insn_sqrt(F, v1);
		} else if (native_func == "pow") {
			if (req_type.nature == Nature::VALUE) {
				jit_value_t v1 = arguments[0]->compile_jit(c, F, Type::VALUE);
				jit_value_t v2 = arguments[1]->compile_jit(c, F, Type::VALUE);
				res = jit_insn_pow(F, v1, v2);
			} else {
				vector<jit_value_t> args = { arguments[0]->compile_jit(c, F, Type::POINTER),arguments[1]->compile_jit(c, F, Type::POINTER) };
				vector<jit_type_t> args_types = { JIT_POINTER,JIT_POINTER };
				jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types.data(), 2, 0);
				res = jit_insn_call_native(F, "test", (void*) &number_pow, sig, args.data(), 1, JIT_CALL_NOTHROW);
				return res;
			}
		}

		if (req_type.nature == Nature::POINTER && type.nature == Nature::VALUE) {
			return VM::value_to_pointer(F, res, type);
		}
		return res;
	}

	/*
	 * Standard function call on object
	 */
	if (this_ptr != nullptr) {

		int arg_count = arguments.size() + 1;
		vector<jit_value_t> args = { this_ptr->compile_jit(c, F, Type::POINTER) };
		vector<jit_type_t> args_types = { JIT_POINTER };

		for (int i = 0; i < arg_count - 1; ++i) {
			args.push_back(arguments[i]->compile_jit(c, F, function->type.getArgumentType(i)));
			args_types.push_back(function->type.getArgumentType(i).nature == Nature::POINTER ? JIT_POINTER :
				(function->type.getArgumentType(i).raw_type == RawType::FUNCTION)	? JIT_POINTER :
				(function->type.getArgumentType(i).raw_type == RawType::FLOAT)	? JIT_FLOAT :
				JIT_INTEGER);
		}

		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, args_types.data(), arg_count, 0);

		return jit_insn_call_native(F, "std_func", (void*) std_func, sig, args.data(), arg_count, JIT_CALL_NOTHROW);
	}


	VariableValue* f = dynamic_cast<VariableValue*>(function);

	if (f != nullptr && f->name->content == "print") {

		jit_value_t v = arguments[0]->compile_jit(c, F, Type::NEUTRAL);

		if (arguments[0]->type.nature == Nature::VALUE) {
			jit_type_t args[1] = {JIT_INTEGER};
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 1, 0);
			jit_insn_call_native(F, "lol", (void*) func_print_int, sig, &v, 1, JIT_CALL_NOTHROW);
		} else {
			jit_type_t args[1] = {JIT_POINTER};
			jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args, 1, 0);
			jit_insn_call_native(F, "lol", (void*) func_print, sig, &v, 1, JIT_CALL_NOTHROW);
		}

		return JIT_CREATE_CONST_POINTER(F, LSNull::null_var);
	}

	vector<jit_value_t> fun;

	if (function->type.nature == Nature::POINTER) {
		jit_value_t fun_addr = function->compile_jit(c, F, Type::NEUTRAL);
		fun.push_back(jit_insn_load_relative(F, fun_addr, 8, jit_type_void_ptr));
	} else {
		fun.push_back(function->compile_jit(c, F, Type::NEUTRAL));
	}

	int arg_count = arguments.size();
	vector<jit_value_t> args;
	vector<jit_type_t> args_types;

	for (int i = 0; i < arg_count; ++i) {
		args.push_back(arguments[i]->compile_jit(c, F, function->type.getArgumentType(i)));
		args_types.push_back(function->type.getArgumentType(i).nature == Nature::POINTER ? JIT_POINTER :
				(function->type.getArgumentType(i).raw_type == RawType::FUNCTION)	? JIT_POINTER :
				(function->type.getArgumentType(i).raw_type == RawType::FLOAT)	? JIT_FLOAT :
				JIT_INTEGER);
	}

	//cout << "function call return type : " << info << endl;

	jit_type_t return_type =
		type.nature == Nature::VALUE ? (
			(type.raw_type == RawType::FUNCTION) ? JIT_POINTER :
			(type.raw_type == RawType::LONG) ? JIT_INTEGER_LONG :
			(type.raw_type == RawType::FLOAT) ? JIT_FLOAT :
			JIT_INTEGER)
		: JIT_POINTER;

	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, return_type, args_types.data(), arg_count, 0);

	jit_value_t ret = jit_insn_call_indirect(F, fun[0], sig, args.data(), arg_count, JIT_CALL_NOTHROW);

	//cout << "function call type " << type << endl;

	if (req_type.nature == Nature::POINTER && type.nature == Nature::VALUE) {
		return VM::value_to_pointer(F, ret, type);
	}

	return ret;
}

