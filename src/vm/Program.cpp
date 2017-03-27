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

extern map<string, jit_value_t> internals;

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
	internals.clear();
	this->compile_main(vm, context);

	// Result
	result.compilation_success = true;
	return result;
}

void Program::analyse(SemanticAnalyser* analyser) {
	main->analyse(analyser, Type::UNKNOWN);
}

void Program::compile_main(VM& vm, Context& context) {

	vm.compiler.program = this;

	jit_init();
	VM::jit_context = jit_context_create();
	jit_context_build_start(VM::jit_context);

	jit_type_t params[0] = {};
	jit_type_t signature = jit_type_create_signature(jit_abi_cdecl, VM::get_jit_type(main->type.getReturnType()), params, 0, 1);
	jit_function_t F = jit_function_create(VM::jit_context, signature);
	jit_insn_uses_catcher(F);
	vm.compiler.enter_function(F);

	compile_jit(vm, vm.compiler, context, false);

	// catch (ex) {
	jit_value_t ex = jit_insn_start_catcher(F);
	auto catchers = vm.compiler.catchers.back();
	if (catchers.size() > 0) {
		for (size_t i = 0; i < catchers.size() - 1; ++i) {
			auto ca = catchers[i];
			jit_insn_branch_if_pc_not_in_range(F, ca.start, ca.end, &ca.next);
			jit_insn_branch(F, &ca.handler);
			jit_insn_label(F, &ca.next);
		}
		jit_insn_branch(F, &catchers.back().handler);
	} else {
		vm.compiler.delete_function_variables();
		VM::store_exception(F, ex);
	}

	//jit_dump_function(fopen("main_uncompiled", "w"), F, "main");

	jit_function_compile(F);
	jit_context_build_end(VM::jit_context);

	//jit_dump_function(fopen("main_compiled", "w"), F, "main");

	closure = jit_function_to_closure(F);

	jit_type_free(signature);
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
	unsigned int line = jit_stack_trace_get_offset(VM::jit_context, trace, 0);
	jit_free(trace);

	VM::ExceptionObj* ex = new VM::ExceptionObj((VM::Exception) type);
	ex->lines.push_back(line);
	return ex;
}

std::string Program::execute() {

	jit_exception_set_handler(&handler);

	Type output_type = main->type.getReturnType();

	if (output_type == Type::VOID) {
		auto fun = (void (*)()) closure;
		fun();
		if (VM::last_exception) throw VM::last_exception;
		return "(void)";
	}

	if (output_type == Type::BOOLEAN) {
		auto fun = (bool (*)()) closure;
		bool res = fun();
		if (VM::last_exception) throw VM::last_exception;
		return res ? "true" : "false";
	}

	if (output_type == Type::INTEGER) {
		auto fun = (int (*)()) closure;
		int res = fun();
		if (VM::last_exception) throw VM::last_exception;
		return std::to_string(res);
	}

	if (output_type == Type::MPZ_TMP or output_type == Type::MPZ) {
		auto fun = (__mpz_struct (*)()) closure;
		__mpz_struct ret = fun();
		if (VM::last_exception) throw VM::last_exception;
		char buff[1000000];
		mpz_get_str(buff, 10, &ret);
		mpz_clear(&ret);
		VM::mpz_deleted++;
		return std::string(buff);
	}

	if (output_type == Type::REAL) {
		auto fun = (double (*)()) closure;
		double res = fun();
		if (VM::last_exception) throw VM::last_exception;
		return LSNumber::print(res);
	}

	if (output_type == Type::LONG) {
		auto fun = (long (*)()) closure;
		long res = fun();
		if (VM::last_exception) throw VM::last_exception;
		return std::to_string(res);
	}

	auto fun = (LSValue* (*)()) closure;
	auto ptr = fun();
	if (VM::last_exception) throw VM::last_exception;
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

/*
extern map<string, jit_value_t> internals;

LSArray<LSValue*>* Program_create_array() {
	return new LSArray<LSValue*>();
}
void Program_push_null(LSArray<LSValue*>* array, int) {
	array->push_clone(LSNull::get());
}
void Program_push_boolean(LSArray<LSValue*>* array, int value) {
	array->push_clone(LSBoolean::get(value));
}
void Program_push_integer(LSArray<LSValue*>* array, int value) {
	array->push_clone(LSNumber::get(value));
}
void Program_push_function(LSArray<LSValue*>* array, void* value) {
	array->push_clone(new LSFunction(value));
}
void Program_push_pointer(LSArray<LSValue*>* array, LSValue* value) {
	array->push_clone(value);
}
*/

void Program::compile_jit(VM& vm, Compiler& c, Context&, bool) {

	// System internal variables
	for (auto var : vm.system_vars) {

		string name = var.first;
		LSValue* value = var.second;

		auto val = c.new_pointer(value);
		internals.insert(pair<string, jit_value_t>(name, val.v));
	}

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

	jit_value_t res = main->body->compile(c).v;
	jit_insn_return(c.F, res);

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
}

}
