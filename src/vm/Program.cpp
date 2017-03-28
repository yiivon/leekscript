#include <chrono>
#include <jit/jit-dump.h>

#include "Program.hpp"
#include "Context.hpp"
#include "value/LSNull.hpp"
#include "value/LSArray.hpp"
#include "../compiler/lexical/LexicalAnalyser.hpp"
#include "../compiler/syntaxic/SyntaxicAnalyser.hpp"
#include "Context.hpp"
#include "../compiler/semantic/SemanticAnalyser.hpp"
#include "../compiler/semantic/SemanticError.hpp"


using namespace std;

struct jit_stack_trace {
	unsigned int size;
	void* items[1];
};

namespace ls {

Program::Program(const std::string& code) {
	this->code = code;
	main = nullptr;
	closure = nullptr;
}

Program::~Program() {
	if (main != nullptr) {
		delete main;
	}
}

VM::Result Program::compile(VM& vm, const std::string& ctx) {

	VM::Result result;

	// Lexical analysis
	LexicalAnalyser lex;
	vector<Token> tokens = lex.analyse(code);

	if (lex.errors.size()) {
		result.compilation_success = false;
		result.lexical_errors = lex.errors;
		return result;
	}

	// Syntaxical analysis
	SyntaxicAnalyser syn;
	this->main = syn.analyse(tokens);
	this->main->is_main_function = true;

	if (syn.getErrors().size() > 0) {
		result.compilation_success = false;
		result.syntaxical_errors = syn.getErrors();
		return result;
	}

	// Semantical analysis
	Context context { ctx };
	SemanticAnalyser sem;
	sem.vm = &vm;
	sem.analyse(this, &context);

	std::ostringstream oss;
	print(oss, true);
	result.program = oss.str();

	if (sem.errors.size()) {
		result.compilation_success = false;
		result.semantical_errors = sem.errors;
		return result;
	}

	// Compilation
	jit_init();
	vm.internals.clear();
	vm.compiler.program = this;
	main->compile(vm.compiler);
	closure = main->ls_fun->function;
	vm.compiler.leave_function();

	// Result
	result.compilation_success = true;
	return result;
}

void Program::analyse(SemanticAnalyser* analyser) {
	main->analyse(analyser, Type::UNKNOWN);
}

/*
 * Handle a native JIT exception, like division by zero etc.
 * The exception is transformed directly into a VM::ExceptionObj
 */
void* handler(int type) {

	void* frame = __builtin_frame_address(1);
	void* pc = jit_get_return_address(frame);
	auto trace = (jit_stack_trace_t) jit_malloc(sizeof(struct jit_stack_trace));
	trace->size = 1;
	trace->items[0] = pc;
	unsigned int line = jit_stack_trace_get_offset(VM::current()->jit_context, trace, 0);
	jit_free(trace);

	auto ex = new VM::ExceptionObj((VM::Exception) type);
	ex->lines.push_back(line);
	return ex;
}

std::string Program::execute(VM& vm) {

	jit_exception_set_handler(&handler);

	Type output_type = main->type.getReturnType();

	if (output_type == Type::VOID) {
		auto fun = (void (*)()) closure;
		fun();
		if (vm.last_exception) throw vm.last_exception;
		return "(void)";
	}

	if (output_type == Type::BOOLEAN) {
		auto fun = (bool (*)()) closure;
		bool res = fun();
		if (vm.last_exception) throw vm.last_exception;
		return res ? "true" : "false";
	}

	if (output_type == Type::INTEGER) {
		auto fun = (int (*)()) closure;
		int res = fun();
		if (vm.last_exception) throw vm.last_exception;
		return std::to_string(res);
	}

	if (output_type == Type::MPZ_TMP or output_type == Type::MPZ) {
		auto fun = (__mpz_struct (*)()) closure;
		__mpz_struct ret = fun();
		if (vm.last_exception) throw vm.last_exception;
		char buff[1000000];
		mpz_get_str(buff, 10, &ret);
		mpz_clear(&ret);
		vm.mpz_deleted++;
		return std::string(buff);
	}

	if (output_type == Type::REAL) {
		auto fun = (double (*)()) closure;
		double res = fun();
		if (vm.last_exception) throw vm.last_exception;
		return LSNumber::print(res);
	}

	if (output_type == Type::LONG) {
		auto fun = (long (*)()) closure;
		long res = fun();
		if (vm.last_exception) throw vm.last_exception;
		return std::to_string(res);
	}

	auto fun = (LSValue* (*)()) closure;
	auto ptr = fun();
	if (vm.last_exception) throw vm.last_exception;
	std::ostringstream oss;
	ptr->dump(oss);
	LSValue::delete_ref(ptr);
	return oss.str();
}

void Program::print(ostream& os, bool debug) const {
	main->body->print(os, 0, debug);
}

std::ostream& operator << (std::ostream& os, const Program* program) {
	program->print(os, false);
	return os;
}

// void Program::compile_jit(VM& vm, Compiler& c, Context&, bool) {

	// User context variables
	/*
	if (toplevel) {
		for (auto var : context.vars) {

			string name = var.first;
			LSValue* value = var.second;

			jit_value_t jit_var = jit_value_create(c.F, LS_POINTER);
			jit_value_t jit_val = LS_CREATE_POINTER(c.F, value);
			jit_insn_store(c.F, jit_var, jit_val);

			c.add_var(name, jit_var, Type(value->getRawType(), Nature::POINTER), false);

			value->refs++;
		}
	}
	*/

	// jit_value_t res = main->body->compile(c).v;
	// jit_insn_return(c.F, res);

	/*
	if (toplevel) {

		// Push program res
		jit_type_t array_sig = jit_type_create_signature(jit_abi_cdecl, JIT_POINTER, {}, 0, 0);
		jit_value_t array = jit_insn_call_native(F, "new", (void*) &Program_create_array, array_sig, {}, 0, JIT_CALL_NOTHROW);

		jit_type_t push_args_types[2] = {JIT_POINTER, JIT_POINTER};
		jit_type_t push_sig_pointer = jit_type_create_signature(jit_abi_cdecl, jit_type_void, push_args_types, 2, 0);

		jit_value_t push_args[2] = {array, res};
		jit_insn_call_native(F, "push", (void*) &Program_push_pointer, push_sig_pointer, push_args, 2, 0);

		VM::delete_obj(F, res);

//		cout << "GLOBALS : " << globals.size() << endl;

		for (auto g : c.get_vars()) {

			string name = g.first;
			Type type = globals_types[name];

			if (globals_ref[name] == true) {
//				cout << name << " is ref, continue" << endl;
				continue;
			}

//			cout << "save in context : " << name << ", type: " << type << endl;
//			cout << "jit_val: " << g.second << endl;

			jit_value_t var_args[2] = {array, g.second};

			if (type.nature == Nature::POINTER) {

//				cout << "save pointer" << endl;
				jit_insn_call_native(F, "push", (void*) &Program_push_pointer, push_sig_pointer, var_args, 2, 0);

//				cout << "delete global " << g.first << endl;
				if (type.must_manage_memory()) {
					VM::delete_obj(F, g.second);
				}

			} else {
//				cout << "save value" << endl;
				if (type.raw_type == RawType::NULLL) {
					jit_type_t push_args_types[2] = {JIT_POINTER, JIT_INTEGER};
					jit_type_t push_sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, push_args_types, 2, 0);
					jit_insn_call_native(F, "push", (void*) &Program_push_null, push_sig, var_args, 2, JIT_CALL_NOTHROW);
				} else if (type.raw_type == RawType::BOOLEAN) {
					jit_type_t push_args_types[2] = {JIT_POINTER, JIT_INTEGER};
					jit_type_t push_sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, push_args_types, 2, 0);
					jit_insn_call_native(F, "push", (void*) &Program_push_boolean, push_sig, var_args, 2, JIT_CALL_NOTHROW);
				} else if (type.raw_type == RawType::INTEGER) {
					jit_type_t push_args_types[2] = {JIT_POINTER, JIT_INTEGER};
					jit_type_t push_sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, push_args_types, 2, 0);
					jit_insn_call_native(F, "push", (void*) &Program_push_integer, push_sig, var_args, 2, JIT_CALL_NOTHROW);
				} else if (type.raw_type == RawType::REAL) {
					jit_type_t args_float[2] = {JIT_POINTER, JIT_REAL};
					jit_type_t sig_push_float = jit_type_create_signature(jit_abi_cdecl, jit_type_void, args_float, 2, 0);
					jit_insn_call_native(F, "push", (void*) &Program_push_float, sig_push_float, var_args, 2, JIT_CALL_NOTHROW);
				} else if (type.raw_type == RawType::FUNCTION) {
					jit_insn_call_native(F, "push", (void*) &Program_push_function, push_sig_pointer, var_args, 2, JIT_CALL_NOTHROW);
				}
			}
		}
		jit_insn_return(F, array);
	}
	*/
// }

}
