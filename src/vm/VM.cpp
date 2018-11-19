#include <sstream>
#include <chrono>

#include "VM.hpp"
#include "../constants.h"
#include "../colors.h"
#include "../compiler/lexical/LexicalAnalyser.hpp"
#include "../compiler/syntaxic/SyntaxicAnalyser.hpp"
#include "Context.hpp"
#include "../compiler/semantic/SemanticAnalyser.hpp"
#include "../compiler/semantic/SemanticError.hpp"
#include "value/LSNumber.hpp"
#include "value/LSArray.hpp"
#include "Program.hpp"
#include "value/LSObject.hpp"
#include "value/LSFunction.hpp"
#include "standard/ValueSTD.hpp"
#include "standard/NullSTD.hpp"
#include "standard/NumberSTD.hpp"
#include "standard/BooleanSTD.hpp"
#include "standard/StringSTD.hpp"
#include "standard/ArraySTD.hpp"
#include "standard/MapSTD.hpp"
#include "standard/SetSTD.hpp"
#include "standard/ObjectSTD.hpp"
#include "standard/SystemSTD.hpp"
#include "standard/FunctionSTD.hpp"
#include "standard/ClassSTD.hpp"
#include "standard/IntervalSTD.hpp"
#include "standard/JsonSTD.hpp"
#include "legacy/Functions.hpp"

using namespace std;

namespace ls {

const unsigned long int VM::DEFAULT_OPERATION_LIMIT = 20000000;
jit_type_t VM::mpz_type;
VM* VM::current_vm = nullptr;

LSValue* op_add(LSValue* x, LSValue* y) {
	return x->add(y);
}
LSValue* op_sub(LSValue* x, LSValue* y) {
	return x->sub(y);
}
LSValue* op_mul(LSValue* x, LSValue* y) {
	return x->mul(y);
}
LSValue* op_div(LSValue* x, LSValue* y) {
	return x->div(y);
}
LSValue* op_int_div(LSValue* x, LSValue* y) {
	return x->int_div(y);
}
LSValue* op_pow(LSValue* x, LSValue* y) {
	return x->pow(y);
}
LSValue* op_mod(LSValue* x, LSValue* y) {
	return x->mod(y);
}

LSValue* ptr_fun(LSValue* v) {
	return v->move();
}

VM::VM(bool v1) : compiler(this) {

	jit_context = jit_context_create();

	operation_limit = VM::DEFAULT_OPERATION_LIMIT;

	null_value = LSNull::create();
	true_value = LSBoolean::create(true);
	false_value = LSBoolean::create(false);

	// Include STD modules
	add_module(new ValueSTD());
	add_module(new NullSTD());
	add_module(new BooleanSTD());
	add_module(new NumberSTD());
	add_module(new StringSTD());
	add_module(new ArraySTD());
	add_module(new MapSTD());
	add_module(new SetSTD());
	add_module(new ObjectSTD());
	add_module(new FunctionSTD());
	add_module(new ClassSTD());
	add_module(new SystemSTD());
	add_module(new IntervalSTD());
	add_module(new JsonSTD());

	// Add function operators
	std::vector<std::string> ops = {"+", "-", "*", "×", "/", "÷", "**", "%", "\\"};
	std::vector<void*> ops_funs = {(void*) &op_add, (void*) &op_sub, (void*) &op_mul, (void*) &op_mul, (void*) &op_div, (void*) &op_div, (void*) &op_pow, (void*) &op_mod, (void*) &op_int_div};

	Type op_type = Type(RawType::FUNCTION, Nature::POINTER);
	op_type.setArgumentType(0, Type::POINTER);
	op_type.setArgumentType(1, Type::POINTER);
	op_type.setReturnType(Type::POINTER);
	auto value_class = system_vars["Value"];

	for (unsigned o = 0; o < ops.size(); ++o) {
		auto fun = new LSFunction(ops_funs[o]);
		fun->refs = 1;
		fun->native = true;
		fun->args = {value_class, value_class};
		fun->return_type = value_class;
		system_vars.insert({ops[o], fun});
		add_internal_var(ops[o], op_type);
	}

	Type ptr_type = Type(RawType::FUNCTION, Nature::POINTER);
	ptr_type.setArgumentType(0, Type::POINTER);
	ptr_type.setReturnType(Type::POINTER);
	auto fun = new LSFunction((void*) ptr_fun);
	fun->refs = 1;
	fun->native = true;
	fun->args = {value_class};
	fun->return_type = value_class;
	system_vars.insert({"ptr", fun});
	add_internal_var("ptr", ptr_type);

	// Add v1 functions
	if (v1) {
		legacy::Functions::create(this);
	}
}

VM::~VM() {
	for (auto& module : modules) {
		delete module;
	}
	for (auto& fun : system_vars) {
		delete fun.second;
	}
	delete null_value;
	delete true_value;
	delete false_value;
	jit_context_destroy(jit_context);
}

VM* VM::current() {
	return current_vm;
}

void VM::add_module(Module* m) {

	modules.push_back(m);
	system_vars.insert({m->name, m->clazz});

	Type const_class = Type::CLASS;
	const_class.constant = true;
	add_internal_var(m->name, const_class);
}

void VM::add_constant(std::string name, Type type, LSValue* value) {
	system_vars.insert({name, value});
	type.constant = true;
	add_internal_var(name, type);
}

VM::Result VM::execute(const std::string code, std::string ctx, std::string file_name, bool debug, bool ops, bool assembly, bool pseudo_code, bool log_instructions) {

	jit_type_t types[3] = {jit_type_int, jit_type_int, jit_type_void_ptr};
	VM::mpz_type = jit_type_create_struct(types, 3, 1);

	// Reset
	this->file_name = file_name;
	VM::current_vm = this;
	LSNull::set_null_value(this->null_value);
	LSBoolean::set_true_value(this->true_value);
	LSBoolean::set_false_value(this->false_value);
	LSValue::obj_count = 0;
	LSValue::obj_deleted = 0;
	VM::mpz_created = 0;
	VM::mpz_deleted = 0;
	VM::operations = 0;
	VM::enable_operations = ops;
	Type::placeholder_counter = 0;
	#if DEBUG_LEAKS
		LSValue::objs().clear();
	#endif

	auto program = new Program(code, file_name);

	// Compile
	auto compilation_start = chrono::high_resolution_clock::now();
	VM::Result result = program->compile(*this, ctx, assembly, pseudo_code, log_instructions);
	auto compilation_end = chrono::high_resolution_clock::now();

	if (log_instructions) {
		std::cout << result.instructions_log;
	}

	if (debug) {
		std::cout << "main() " << result.program << std::endl; // LCOV_EXCL_LINE
	}
	if (pseudo_code) {
		if (debug) std::cout << std::endl;
		std::cout << result.pseudo_code;
	}
	if (assembly) {
		if (debug) std::cout << std::endl;
		std::cout << result.assembly;
	}

	// Execute
	std::string value = "";
	if (result.compilation_success) {

		auto exe_start = chrono::high_resolution_clock::now();
		try {
			value = program->execute(*this);
			result.execution_success = true;
		} catch (vm::ExceptionObj& ex) {
			result.exception = ex;
		}
		auto exe_end = chrono::high_resolution_clock::now();

		result.execution_time = chrono::duration_cast<chrono::nanoseconds>(exe_end - exe_start).count();
		result.execution_time_ms = (((double) result.execution_time / 1000) / 1000);

		result.value = value;
	}

	// Set results
	result.context = ctx;
	result.compilation_time = chrono::duration_cast<chrono::nanoseconds>(compilation_end - compilation_start).count();
	result.compilation_time_ms = (((double) result.compilation_time / 1000) / 1000);
	result.operations = VM::operations;

	// Cleaning
	delete program;
	VM::enable_operations = true;
	jit_type_free(VM::mpz_type);
	RawType::clear_placeholder_types();

	// Results
	result.objects_created = LSValue::obj_count;
	result.objects_deleted = LSValue::obj_deleted;
	result.mpz_objects_created = VM::mpz_created;
	result.mpz_objects_deleted = VM::mpz_deleted;

	if (ls::LSValue::obj_deleted != ls::LSValue::obj_count) {
		// LCOV_EXCL_START
		cout << C_RED << "/!\\ " << LSValue::obj_deleted << " / " << LSValue::obj_count << " (" << (LSValue::obj_count - LSValue::obj_deleted) << " leaked)" << END_COLOR << endl;
		#if DEBUG_LEAKS
			for (auto o : LSValue::objs()) {
				std::cout << o.second << " (" << o.second->refs << " refs)" << endl;
			}
		#endif
		// LCOV_EXCL_STOP
	}
	if (VM::mpz_deleted != VM::mpz_created) {
		cout << C_RED << "/!\\ " << VM::mpz_deleted << " / " << VM::mpz_created << " (" << (VM::mpz_created - VM::mpz_deleted) << " mpz leaked)" << END_COLOR << endl; // LCOV_EXCL_LINE
	}

	return result;
}

void VM::add_internal_var(std::string name, Type type) {
	internal_vars.insert({name,
		std::make_shared<SemanticVar>(name, VarScope::INTERNAL, type, 0, nullptr, nullptr, nullptr)
	});
}

int VM_boolean_to_value(LSBoolean* b) {
	return b->value;
}
int VM_integer_to_value(LSNumber* n) {
	return n->value;
}

jit_value_t VM::pointer_to_value(jit_function_t F, jit_value_t v, Type type) {

	if (type == Type::BOOLEAN) {
		jit_type_t args_types[1] = {LS_POINTER};
		jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_INTEGER, args_types, 1, 1);
		auto r = jit_insn_call_native(F, "convert", (void*) VM_boolean_to_value, sig, &v, 1, JIT_CALL_NOTHROW);
		jit_type_free(sig);
		return r;
	}

	// Integer
	jit_type_t args_types[1] = {LS_POINTER};
	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, LS_INTEGER, args_types, 1, 1);
	auto r = jit_insn_call_native(F, "convert", (void*) VM_integer_to_value, sig, &v, 1, JIT_CALL_NOTHROW);
	jit_type_free(sig);
	return r;
}

jit_value_t VM::int_to_real(jit_function_t F, jit_value_t v) {
	jit_value_t real = jit_value_create(F, LS_REAL);
	jit_insn_store(F, real, v);
	return real;
}

void VM::inc_mpz_counter(jit_function_t F) {
	jit_value_t jit_counter_ptr = jit_value_create_long_constant(F, LS_POINTER, (long) &VM::current()->mpz_created);
	jit_value_t jit_counter = jit_insn_load_relative(F, jit_counter_ptr, 0, jit_type_long);
	jit_insn_store_relative(F, jit_counter_ptr, 0, jit_insn_add(F, jit_counter, LS_CREATE_INTEGER(F, 1)));
}

void fake_destru(void*) {}
void VM::throw_exception(vm::Exception type) {
	auto ex = (vm::ExceptionObj*) __cxa_allocate_exception(sizeof(vm::ExceptionObj));
	new (ex) vm::ExceptionObj(type);
	__cxa_throw(ex, (void*) &typeid(vm::ExceptionObj), &fake_destru);
}

}
