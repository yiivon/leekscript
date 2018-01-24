#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "LLVMCompiler.hpp"
#include "../vm/VM.hpp"
#include "../vm/value/LSNull.hpp"
#include "../vm/value/LSArray.hpp"
#include "../vm/value/LSMap.hpp"
#include "../vm/value/LSClosure.hpp"

namespace ls {

llvm::LLVMContext LLVMCompiler::context;
llvm::IRBuilder<> LLVMCompiler::Builder(context);
std::unique_ptr<llvm::Module> LLVMCompiler::TheModule;
std::map<std::string, llvm::Value*> LLVMCompiler::NamedValues;
std::unique_ptr<llvm::legacy::FunctionPassManager> LLVMCompiler::TheFPM;

void LLVMCompiler::init() {

	// Open a new module.
	TheModule = llvm::make_unique<llvm::Module>("my_jit", context);
	TheModule->setDataLayout(getTargetMachine().createDataLayout());

	// Create a new pass manager attached to it.
	TheFPM = llvm::make_unique<llvm::legacy::FunctionPassManager>(TheModule.get());
	// Do simple "peephole" optimizations and bit-twiddling optzns.
	TheFPM->add(llvm::createInstructionCombiningPass());
	// Reassociate expressions.
	TheFPM->add(llvm::createReassociatePass());
	// Eliminate Common SubExpressions.
	TheFPM->add(llvm::createGVNPass());
	// Simplify the control flow graph (deleting unreachable blocks, etc).
	TheFPM->add(llvm::createCFGSimplificationPass());
	TheFPM->doInitialization();
}

void LLVMCompiler::end() {
}

LLVMCompiler::value LLVMCompiler::clone(LLVMCompiler::value v) const {
	if (v.t.must_manage_memory()) {
		if (v.t.reference) {
			v = insn_load(v);
		}
		auto r = insn_call(v.t, {v}, +[](LSValue* value) {
			return value->clone();
		}, "clone");
		// log_insn(4) << "clone " << dump_val(v) << " " << dump_val(r) << std::endl;
		return r;
	}
	return v;
}
LLVMCompiler::value LLVMCompiler::new_null() const {
	return new_pointer(LSNull::get());
}
LLVMCompiler::value LLVMCompiler::new_bool(bool b) const {
	return {llvm::ConstantInt::get(context, llvm::APInt(1, b, false)), Type::BOOLEAN};
}

LLVMCompiler::value LLVMCompiler::new_integer(int x) const {
	return {llvm::ConstantInt::get(context, llvm::APInt(32, x, true)), Type::INTEGER};
}

LLVMCompiler::value LLVMCompiler::new_real(double r) const {
	return {llvm::ConstantFP::get(context, llvm::APFloat(r)), Type::REAL};
}
LLVMCompiler::value LLVMCompiler::new_long(long l) const {
	return {llvm::ConstantInt::get(context, llvm::APInt(64, l, true)), Type::LONG};
}
LLVMCompiler::value LLVMCompiler::new_pointer(const void* p) const {
	// llvm::Constant* address = llvm::ConstantInt::get(llvm::Type::getInt64Ty(LLVMCompiler::context), (int64_t) p);
	// return {llvm::ConstantExpr::getIntToPtr(address, llvm::PointerType::getUnqual(llvm::Type::getInt64Ty(LLVMCompiler::context))), Type::POINTER};
	return {new_long((long) p).v, Type::POINTER};
}
LLVMCompiler::value LLVMCompiler::new_object() const {
	return insn_call(Type::OBJECT_TMP, {}, +[]() {
		// FIXME coverage doesn't work for the one line version
		auto o = new LSObject();
		return o;
	}, "new_object");
}
LLVMCompiler::value LLVMCompiler::new_object_class(LLVMCompiler::value clazz) const {
	assert(false);
}
LLVMCompiler::value LLVMCompiler::new_mpz(long value) const {
	assert(false);
}
LLVMCompiler::value LLVMCompiler::new_array(Type element_type, std::vector<LLVMCompiler::value> elements) const {
	auto array = [&]() { if (element_type == Type::INTEGER) {
		return insn_call(Type::INT_ARRAY_TMP, {new_integer(elements.size())}, +[](int capacity) {
			auto array = new LSArray<int>();
			array->reserve(capacity);
			return array;
		}, "new_int_array");
	} else if (element_type == Type::REAL) {
		return insn_call(Type::REAL_ARRAY_TMP, {new_integer(elements.size())}, +[](int capacity) {
			auto array = new LSArray<double>();
			array->reserve(capacity);
			return array;
		}, "new_real_array");
	} else {
		return insn_call(Type::PTR_ARRAY_TMP, {new_integer(elements.size())}, +[](int capacity) {
			auto array = new LSArray<LSValue*>();
			array->reserve(capacity);
			return array;
		}, "new_ptr_array");
	}}();
	for (const auto& element : elements) {
		insn_push_array(array, element);
	}
	// size of the array + 1 operations
	inc_ops(elements.size() + 1);
	return array;
}

LLVMCompiler::value LLVMCompiler::to_int(LLVMCompiler::value) const {
	assert(false);
}
LLVMCompiler::value LLVMCompiler::to_real(LLVMCompiler::value) const {
	assert(false);
}
LLVMCompiler::value LLVMCompiler::to_long(LLVMCompiler::value) const {
	assert(false);
}
LLVMCompiler::value LLVMCompiler::insn_convert(LLVMCompiler::value, Type) const {
	assert(false);
}

// Operators wrapping
LLVMCompiler::value LLVMCompiler::insn_not(LLVMCompiler::value) const {
	assert(false);
}
LLVMCompiler::value LLVMCompiler::insn_not_bool(LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_neg(LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_and(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_or(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }

LLVMCompiler::value LLVMCompiler::insn_add(LLVMCompiler::value a, LLVMCompiler::value b) const {
	return {Builder.CreateAdd(a.v, b.v, "addtmp"), Type::INTEGER};
}
LLVMCompiler::value LLVMCompiler::insn_sub(LLVMCompiler::value a, LLVMCompiler::value b) const {
	return {Builder.CreateSub(a.v, b.v, "addtmp"), Type::INTEGER};
}

LLVMCompiler::value LLVMCompiler::insn_eq(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_ne(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_lt(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_le(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_gt(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_ge(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_mul(LLVMCompiler::value a, LLVMCompiler::value b) const {
	return {Builder.CreateFMul(a.v, b.v, "multmp"), Type::REAL};
}
LLVMCompiler::value LLVMCompiler::insn_div(LLVMCompiler::value a, LLVMCompiler::value b) const {
	return {Builder.CreateFDiv(a.v, b.v, "divtmp"), Type::REAL};
}
LLVMCompiler::value LLVMCompiler::insn_int_div(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_bit_and(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_bit_or(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_bit_xor(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_mod(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_pow(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_log10(LLVMCompiler::value) const { assert(false); }

// LLVMCompiler::value management
LLVMCompiler::value LLVMCompiler::insn_create_value(Type t) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_to_pointer(LLVMCompiler::value v) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_to_bool(LLVMCompiler::value v) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_address_of(LLVMCompiler::value v) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_load(LLVMCompiler::value v, int pos, Type t) const { assert(false); }
void  LLVMCompiler::insn_store(LLVMCompiler::value, LLVMCompiler::value) const { assert(false); }
void  LLVMCompiler::insn_store_relative(LLVMCompiler::value, int, LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_typeof(LLVMCompiler::value v) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_class_of(LLVMCompiler::value v) const { assert(false); }
void  LLVMCompiler::insn_delete(LLVMCompiler::value v) const { assert(false); }
void  LLVMCompiler::insn_delete_temporary(LLVMCompiler::value v) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_array_size(LLVMCompiler::value v) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_get_capture(int index, Type type) const { assert(false); }
void  LLVMCompiler::insn_push_array(LLVMCompiler::value array, LLVMCompiler::value element) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_move_inc(LLVMCompiler::value) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_clone_mpz(LLVMCompiler::value mpz) const { assert(false); }
void  LLVMCompiler::insn_delete_mpz(LLVMCompiler::value mpz) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_inc_refs(LLVMCompiler::value v) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_dec_refs(LLVMCompiler::value v, LLVMCompiler::value previous) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_move(LLVMCompiler::value v) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_refs(LLVMCompiler::value v) const { assert(false); }
LLVMCompiler::value LLVMCompiler::insn_native(LLVMCompiler::value v) const { assert(false); }

// Iterators
LLVMCompiler::value LLVMCompiler::iterator_begin(LLVMCompiler::value v) const { assert(false); }
LLVMCompiler::value LLVMCompiler::iterator_end(LLVMCompiler::value v, LLVMCompiler::value it) const { assert(false); }
LLVMCompiler::value LLVMCompiler::iterator_get(LLVMCompiler::value it, LLVMCompiler::value previous) const { assert(false); }
LLVMCompiler::value LLVMCompiler::iterator_key(LLVMCompiler::value v, LLVMCompiler::value it, LLVMCompiler::value previous) const { assert(false); }
void LLVMCompiler::iterator_increment(LLVMCompiler::value it) const { assert(false); }

// Controls
void LLVMCompiler::insn_if(LLVMCompiler::value v, std::function<void()> then) const { assert(false); }
void LLVMCompiler::insn_if_not(LLVMCompiler::value v, std::function<void()> then) const { assert(false); }
void LLVMCompiler::insn_throw(LLVMCompiler::value v) const { assert(false); }
void LLVMCompiler::insn_throw_object(vm::Exception type) const { assert(false); }
void LLVMCompiler::insn_label(label*) const { assert(false); }
void LLVMCompiler::insn_branch(label* l) const { assert(false); }
void LLVMCompiler::insn_branch_if(LLVMCompiler::value v, label* l) const { assert(false); }
void LLVMCompiler::insn_branch_if_not(LLVMCompiler::value v, label* l) const { assert(false); }
void LLVMCompiler::insn_branch_if_pc_not_in_range(label* a, label* b, label* n) const { assert(false); }
void LLVMCompiler::insn_return(LLVMCompiler::value v) const {
	LLVMCompiler::Builder.CreateRet(v.v);
}

// Call functions
LLVMCompiler::value LLVMCompiler::insn_call(Type return_type, std::vector<LLVMCompiler::value> args, void* func, std::string function_name) const {
	std::vector<llvm::Value*> llvm_args;
	std::vector<llvm::Type*> llvm_types;
	for (unsigned i = 0, e = args.size(); i != e; ++i) {
		llvm_args.push_back(args[i].v);
		llvm_types.push_back(args[i].t.llvm_type());
	}
	((LLVMCompiler*) this)->mappings.insert({function_name, (llvm::JITTargetAddress) func});
	auto fun_type = llvm::FunctionType::get(return_type.llvm_type(), llvm_types, false);
	auto lambdaFN = llvm::Function::Create(fun_type, llvm::Function::ExternalLinkage, function_name, LLVMCompiler::TheModule.get());
	return {Builder.CreateCall(lambdaFN, llvm_args, function_name), return_type};
}

LLVMCompiler::value LLVMCompiler::insn_call_indirect(Type return_type, LLVMCompiler::value fun, std::vector<LLVMCompiler::value> args) const {
	return new_integer(0);
}

void LLVMCompiler::function_add_capture(LLVMCompiler::value fun, LLVMCompiler::value capture) { assert(false); }
void LLVMCompiler::log(const std::string&& str) const { assert(false); }

// Blocks
void LLVMCompiler::enter_block() {
	// TODO
}
void LLVMCompiler::leave_block() {
	// TODO
}
void LLVMCompiler::delete_variables_block(int deepness) { assert(false); } // delete all variables in the #deepness current blocks
void LLVMCompiler::enter_function(jit_function_t F, bool is_closure, Function* fun) {
	// TODO
}
void LLVMCompiler::leave_function() {
	// TODO
}
int LLVMCompiler::get_current_function_blocks() const { assert(false); }
void LLVMCompiler::delete_function_variables() { assert(false); }
bool LLVMCompiler::is_current_function_closure() const { assert(false); }

// Variables
void LLVMCompiler::add_var(const std::string& name, LLVMCompiler::value) { assert(false); }
void LLVMCompiler::add_function_var(LLVMCompiler::value) { assert(false); }
LLVMCompiler::value& LLVMCompiler::get_var(const std::string& name) { assert(false); }
void LLVMCompiler::set_var_type(std::string& name, const Type& type) { assert(false); }
std::map<std::string, LLVMCompiler::value> LLVMCompiler::get_vars() { assert(false); }
void LLVMCompiler::update_var(std::string& name, LLVMCompiler::value) { assert(false); }

// Loops
void LLVMCompiler::enter_loop(LLVMCompiler::label*, LLVMCompiler::label*) { assert(false); }
void LLVMCompiler::leave_loop() { assert(false); }
LLVMCompiler::label* LLVMCompiler::get_current_loop_end_label(int deepness) const { assert(false); }
LLVMCompiler::label* LLVMCompiler::get_current_loop_cond_label(int deepness) const { assert(false); }
int LLVMCompiler::get_current_loop_blocks(int deepness) const { assert(false); }

/** Operations **/
void LLVMCompiler::inc_ops(int add) const {
	// TODO
}
void LLVMCompiler::inc_ops_jit(LLVMCompiler::value add) const { assert(false); }
void LLVMCompiler::get_operations() const { assert(false); }

/** Exceptions **/
void LLVMCompiler::mark_offset(int line) {
	// TODO
}
void LLVMCompiler::add_catcher(label start, label end, label handler) { assert(false); }
void LLVMCompiler::insn_check_args(std::vector<LLVMCompiler::value> args, std::vector<LSValueType> types) const { assert(false); }

// Utils
std::ostringstream& LLVMCompiler::_log_insn(int indent) const { assert(false); }
std::string LLVMCompiler::dump_val(LLVMCompiler::value v) const { assert(false); }
void LLVMCompiler::register_label(label* v) const { assert(false); }
void LLVMCompiler::log_insn_code(std::string instruction) const {
	// TODO
}
void LLVMCompiler::add_literal(void* ptr, std::string value) const {
	((LLVMCompiler*) this)->literals.insert({ptr, value});
}

}
