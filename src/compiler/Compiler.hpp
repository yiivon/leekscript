#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/GlobalMappingLayer.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/ExecutionEngine/Orc/IRTransformLayer.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/ExecutionEngine/Orc/IndirectionUtils.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Mangler.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "../type/Type.hpp"
#include "../vm/Exception.hpp"
#include "../vm/LSValue.hpp"
#include <gmp.h>

namespace ls {

class Program;
class VM;
class Function;

class Compiler {
public:
	struct value {
		llvm::Value* v;
		Type t;
		bool operator == (const value& o) const {
			return v == o.v and t == o.t;
		}
		bool operator != (const value& o) const {
			return v != o.v or t != o.t;
		}
		value() : v(nullptr), t() {}
		value(llvm::Value* v, Type t) : v(v), t(t) {}
	};
	struct label {
		llvm::BasicBlock* block = nullptr;
	};
	struct catcher {
		llvm::BasicBlock* handler;
	};
	struct function_entry {
		llvm::JITTargetAddress addr;
		llvm::Function* function;
	};

	static llvm::orc::ThreadSafeContext Ctx;
	static llvm::IRBuilder<> builder;

	llvm::Function* F;
	Function* fun;
	std::stack<llvm::Function*> functions;
	std::stack<Function*> functions2;
	std::stack<bool> function_is_closure;
	std::vector<int> functions_blocks;
	std::stack<std::map<std::string, value>> arguments;
	std::stack<llvm::BasicBlock*> function_llvm_blocks;
	std::vector<int> loops_blocks; // how many blocks are open in the current loop
	std::vector<label*> loops_end_labels;
	std::vector<label*> loops_cond_labels;
	std::vector<std::vector<value>> function_variables;
	std::vector<std::map<std::string, value>> variables;
	std::vector<std::vector<std::vector<catcher>>> catchers;
	bool output_assembly = false;
	std::ostringstream assembly;
	bool output_pseudo_code = false;
	std::ostringstream pseudo_code;
	bool log_instructions = false;
	std::ostringstream instructions_debug;
	std::map<label*, std::string> label_map;
	std::map<void*, std::string> literals;
	std::map<std::string, function_entry> mappings;
	std::stack<int> exception_line;

	VM* vm;
	Program* program;

	std::unique_ptr<llvm::TargetMachine> TM;
	llvm::DataLayout DL;
	llvm::orc::ExecutionSession ES;
	llvm::orc::LegacyRTDyldObjectLinkingLayer ObjectLayer;
	llvm::orc::LegacyIRCompileLayer<decltype(ObjectLayer), llvm::orc::SimpleCompiler> CompileLayer;
	using OptimizeFunction = std::function<std::unique_ptr<llvm::Module>(std::unique_ptr<llvm::Module>)>;
	llvm::orc::LegacyIRTransformLayer<decltype(CompileLayer), OptimizeFunction> OptimizeLayer;
	
	Compiler(VM* vm);

	const llvm::DataLayout& getDataLayout() const { return DL; }

	llvm::LLVMContext& getContext() const { return *Ctx.getContext(); }

	std::unique_ptr<llvm::Module> optimizeModule(std::unique_ptr<llvm::Module> M) {
		// Create a function pass manager.
		auto FPM = llvm::make_unique<llvm::legacy::FunctionPassManager>(M.get());
		// Add some optimizations.
		FPM->add(llvm::createInstructionCombiningPass());
		FPM->add(llvm::createReassociatePass());
		FPM->add(llvm::createGVNPass());
		FPM->add(llvm::createCFGSimplificationPass());
		FPM->doInitialization();
		// Run the optimizations over all functions in the module being added to the JIT.
		for (auto &F : *M)
			FPM->run(F);
		return M;
	}
	llvm::orc::VModuleKey addModule(std::unique_ptr<llvm::Module> M) {
		llvm::orc::VModuleKey K = ES.allocateVModule();
		cantFail(CompileLayer.addModule(K, std::move(M)));
		return K;
	}
	llvm::JITSymbol findSymbol(const std::string Name) {
		return CompileLayer.findSymbol(Name, false);
	}
	void removeModule(llvm::orc::VModuleKey K) {
		cantFail(CompileLayer.removeModule(K));
	}

	/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of the function.  This is used for mutable variables etc.
	llvm::AllocaInst* CreateEntryBlockAlloca(const std::string& VarName, llvm::Type* type) const {
		auto function = builder.GetInsertBlock()->getParent();
		llvm::IRBuilder<> builder(&function->getEntryBlock(), function->getEntryBlock().begin());
		return builder.CreateAlloca(type, nullptr, VarName);
	}

	void init();
	void end();

	// Value creation
	value clone(value) const;
	value new_null() const;
	value new_bool(bool b) const;
	value new_integer(int i) const;
	value new_real(double r) const;
	value new_long(long l) const;
	value new_mpz() const;
	value new_const_string(std::string s, std::string name) const;
	value new_pointer(const void* p, Type type) const;
	value new_null_pointer() const;
	value new_function(Type type) const;
	value new_function(llvm::Function* f, Type type) const;
	value new_function(std::string name, Type type) const;
	value new_closure(llvm::Function* f, Type type, std::vector<value> captures) const;
	value new_class(std::string name) const;
	value new_object() const;
	value new_object_class(value clazz) const;
	value new_set() const;
	value create_entry(const std::string& name, Type type) const;
	value get_symbol(const std::string& name, Type type) const;

	// Conversions
	value to_int(value) const;
	value to_real(value) const;
	value to_long(value) const;
	value insn_convert(value, Type) const;

	// Operators wrapping
	value insn_not(value) const;
	value insn_not_bool(value) const;
	value insn_neg(value) const;
	value insn_and(value, value) const;
	value insn_or(value, value) const;
	value insn_add(value, value) const;
	value insn_sub(value, value) const;
	value insn_eq(value, value) const;
	value insn_pointer_eq(value, value) const;
	value insn_ne(value, value) const;
	value insn_lt(value, value) const;
	value insn_le(value, value) const;
	value insn_gt(value, value) const;
	value insn_ge(value, value) const;
	value insn_mul(value, value) const;
	value insn_div(value, value) const;
	value insn_int_div(value, value) const;
	value insn_bit_and(value, value) const;
	value insn_bit_or(value, value) const;
	value insn_bit_xor(value, value) const;
	value insn_shl(value, value) const;
	value insn_lshr(value, value) const;
	value insn_ashr(value, value) const;
	value insn_mod(value, value) const;
	value insn_double_mod(value, value) const;
	value insn_cmpl(value, value) const;

	// Math Functions
	value insn_log(value) const;
	value insn_log10(value) const;
	value insn_ceil(value) const;
	value insn_round(value) const;
	value insn_floor(value) const;
	value insn_cos(value) const;
	value insn_sin(value) const;
	value insn_tan(value) const;
	value insn_acos(value) const;
	value insn_asin(value) const;
	value insn_atan(value) const;
	value insn_pow(value, value) const;
	value insn_min(value, value) const;
	value insn_max(value, value) const;
	value insn_exp(value) const;
	value insn_atan2(value, value) const;
	value insn_abs(value) const;

	// Value management
	value insn_to_any(value v) const;
	value insn_to_bool(value v) const;
	value insn_load(value v) const;
	value insn_load_member(value v, int pos) const;
	void  insn_store(value, value) const;
	void  insn_store_member(value, int, value) const;
	value insn_typeof(value v) const;
	value insn_class_of(value v) const;
	void  insn_delete(value v) const;
	void  insn_delete_temporary(value v) const;
	value insn_get_capture(int index, Type type) const;
	value insn_get_capture_l(int index, Type type) const;
	value insn_move_inc(value) const;
	value insn_clone_mpz(value mpz) const;
	void  insn_delete_mpz(value mpz) const;
	value insn_inc_refs(value v) const;
	value insn_dec_refs(value v, value previous = {}) const;
	value insn_move(value v) const;
	value insn_refs(value v) const;
	value insn_native(value v) const;
	value insn_get_argument(const std::string& name) const;

	// Arrays
	value new_array(Type type, std::vector<value> elements) const;
	value insn_array_size(value v) const;
	void  insn_push_array(value array, value element) const;
	value insn_array_at(value array, value index) const;
	value insn_array_end(value array) const;

	// Iterators
	value iterator_begin(value v) const;
	value iterator_rbegin(value v) const;
	value iterator_end(value v, value it) const;
	value iterator_rend(value v, value it) const;
	value iterator_get(Type collectionType, value it, value previous) const;
	value iterator_rget(Type collectionType, value it, value previous) const;
	value iterator_key(value v, value it, value previous) const;
	value iterator_rkey(value v, value it, value previous) const;
	void iterator_increment(Type collectionType, value it) const;
	void iterator_rincrement(Type collectionType, value it) const;
	value insn_foreach(value v, Type output, const std::string var, const std::string key, std::function<value(value, value)>, bool reversed = false);

	// Controls
	label insn_init_label(std::string name) const;
	void insn_if(value v, std::function<void()> then, std::function<void()> elze = nullptr) const;
	void insn_if_new(value cond, label* then, label* elze) const;
	void insn_if_not(value v, std::function<void()> then) const;
	void insn_throw(value v) const;
	void insn_throw_object(vm::Exception type) const;
	void insn_label(label*) const;
	void insn_branch(label* l) const;
	void insn_return(value v) const;
	void insn_return_void() const;
	value insn_phi(Type type, value v1, label l1, value v2, label l2) const;

	// Call functions
	value insn_invoke(Type return_type, std::vector<value> args, std::string name) const;
	value insn_invoke(Type return_type, std::vector<value> args, value func) const;
	value insn_invoke(Type return_type, std::vector<value> args, llvm::Function* fun) const;
	value insn_call(Type return_type, std::vector<value> args, value fun) const;
	value insn_call(Type return_type, std::vector<value> args, llvm::Function* fun) const;
	value insn_call(Type return_type, std::vector<value> args, std::string name) const;
	void function_add_capture(value fun, value capture) const;
	void log(const std::string&& str) const;

	// Blocks
	void enter_block();
	void leave_block(bool delete_vars = true);
	void delete_variables_block(int deepness); // delete all variables in the #deepness current blocks
	void enter_function(llvm::Function* F, bool is_closure, Function* fun);
	void leave_function();
	int get_current_function_blocks() const;
	void delete_function_variables() const;
	bool is_current_function_closure() const;
	void insert_new_generation_block() const;

	// Variables
	value add_var(const std::string& name, value value);
	value create_and_add_var(const std::string& name, Type type);
	void add_function_var(value value);
	void remove_function_var(value value);

	value get_var(const std::string& name);
	void update_var(std::string& name, value value);

	// Loops
	void enter_loop(label*, label*);
	void leave_loop();
	label* get_current_loop_end_label(int deepness) const;
	label* get_current_loop_cond_label(int deepness) const;
	int get_current_loop_blocks(int deepness) const;

	/** Operations **/
	void inc_ops(int add) const;
	void inc_ops_jit(value add) const;

	/** Exceptions **/
	void mark_offset(int line);
	void insn_try_catch(std::function<void()> try_, std::function<void()> catch_);
	void insn_check_args(std::vector<value> args, std::vector<LSValueType> types) const;
	const catcher* find_catcher() const;

	// Utils
	std::ostringstream& _log_insn(int indent) const;
	std::string dump_val(value v) const;
	void register_label(label* v) const;
	void log_insn_code(std::string instruction) const;
	void add_literal(void* ptr, std::string value) const;
	static void print_mpz(__mpz_struct value);
	void assert_value_ok(value) const;
	void increment_mpz_created() const;
	void increment_mpz_deleted() const;
};

}

namespace std {
	std::ostream& operator << (std::ostream&, const __mpz_struct);
	std::ostream& operator << (std::ostream&, const std::vector<ls::Compiler::value>&);
}

#endif
