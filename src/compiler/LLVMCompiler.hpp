#ifndef COMPILER_LLVM_HPP
#define COMPILER_LLVM_HPP

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

class LLVMCompiler {
public:
	// TODO private builder and context
	static llvm::IRBuilder<> builder;
	static llvm::LLVMContext context;
	struct value {
		llvm::Value* v;
		Type t;
		bool operator == (const value& o) const {
			return v == o.v and t == o.t;
		}
		bool operator != (const value& o) const {
			return v != o.v or t != o.t;
		}
		value() : v(nullptr), t({}) {}
		value(llvm::Value* v, Type t) : v(v), t(t) {}
	};
	struct label {
		llvm::BasicBlock* block;
	};
	struct catcher {
		label start;
		label end;
		label handler;
		label next;
	};
	struct function_entry {
		llvm::JITTargetAddress addr;
		llvm::Value* function;
	};

	llvm::Function* F;
	Function* fun;
	std::stack<llvm::Function*> functions;
	std::stack<Function*> functions2;
	std::stack<bool> function_is_closure;
	std::vector<int> functions_blocks;
	std::stack<std::map<std::string, value>> arguments;
	std::vector<int> loops_blocks; // how many blocks are open in the current loop
	std::vector<label*> loops_end_labels;
	std::vector<label*> loops_cond_labels;
	std::vector<std::vector<value>> function_variables;
	std::vector<std::map<std::string, value>> variables;
	std::vector<std::vector<catcher>> catchers;
	bool output_assembly = false;
	std::ostringstream assembly;
	bool output_pseudo_code = false;
	std::ostringstream pseudo_code;
	bool log_instructions = false;
	std::ostringstream instructions_debug;
	std::map<label*, std::string> label_map;
	std::map<void*, std::string> literals;
	std::map<std::string, function_entry> mappings;

	VM* vm;
	Program* program;
	std::unique_ptr<llvm::TargetMachine> TM;
	const llvm::DataLayout DL;
	llvm::orc::RTDyldObjectLinkingLayer ObjectLayer;
	llvm::orc::IRCompileLayer<decltype(ObjectLayer), llvm::orc::SimpleCompiler> CompileLayer;
	using ModuleHandle = decltype(CompileLayer)::ModuleHandleT;

	LLVMCompiler(VM* vm) : vm(vm), TM(llvm::EngineBuilder().selectTarget()), DL(TM->createDataLayout()), ObjectLayer([]() { return std::make_shared<llvm::SectionMemoryManager>(); }), CompileLayer(ObjectLayer, llvm::orc::SimpleCompiler(*TM)) {
		llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
	}

	llvm::TargetMachine& getTargetMachine() {
		return *TM;
	}

	ModuleHandle addModule(std::shared_ptr<llvm::Module> M) {
		// Build our symbol resolver:
		// Lambda 1: Look back into the JIT itself to find symbols that are part of the same "logical dylib".
		// Lambda 2: Search for external symbols in the host process.
		auto Resolver = llvm::orc::createLambdaResolver(
			[&](const std::string &Name) {
				// std::cout << "resolve symbol 1 " << Name << std::endl;
				if (auto Sym = CompileLayer.findSymbol(Name, false))
					return Sym;
				auto i = mappings.find(Name);
				if (i != mappings.end()) {
					return llvm::JITSymbol(i->second.addr, llvm::JITSymbolFlags(llvm::JITSymbolFlags::FlagNames::None));
				}
				return llvm::JITSymbol(nullptr);
			},
			[](const std::string &Name) {
				// std::cout << "resolve symbol 2 " << Name << std::endl;
				if (auto SymAddr = llvm::RTDyldMemoryManager::getSymbolAddressInProcess(Name))
					return llvm::JITSymbol(SymAddr, llvm::JITSymbolFlags::Exported);
				return llvm::JITSymbol(nullptr);
		});
		// Add the set to the JIT with the resolver we created above and a newly created SectionMemoryManager.
		return cantFail(CompileLayer.addModule(std::move(M), std::move(Resolver)));
	}

	llvm::JITSymbol findSymbol(const std::string Name) {
		std::string MangledName;
		llvm::raw_string_ostream MangledNameStream(MangledName);
		llvm::Mangler::getNameWithPrefix(MangledNameStream, Name, DL);
		return CompileLayer.findSymbol(MangledNameStream.str(), true);
	}

	llvm::JITTargetAddress getSymbolAddress(const std::string Name) {
		return cantFail(findSymbol(Name).getAddress());
	}

	/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of the function.  This is used for mutable variables etc.
	static llvm::AllocaInst* CreateEntryBlockAlloca(const std::string& VarName, llvm::Type* type) {
		auto function = builder.GetInsertBlock()->getParent();
		llvm::IRBuilder<> builder(&function->getEntryBlock(), function->getEntryBlock().begin());
		return builder.CreateAlloca(type, nullptr, VarName);
	}

	void removeModule(ModuleHandle H) {
		cantFail(CompileLayer.removeModule(H));
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
	value new_pointer(const void* p, Type type = Type::ANY) const;
	value new_function(const void* p) const;
	value new_class(const void* p) const;
	value new_object() const;
	value new_object_class(value clazz) const;
	value new_mpz(long value = 0) const;
	value new_mpz_init(const mpz_t mpz) const;
	value create_entry(const std::string& name, Type type) const;

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
	value insn_sqrt(value) const;
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
	value insn_move_inc(value) const;
	value insn_clone_mpz(value mpz) const;
	void  insn_delete_mpz(value mpz) const;
	value insn_inc_refs(value v) const;
	value insn_dec_refs(value v, value previous = {nullptr, Type::NULLL}) const;
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
	value iterator_end(value v, value it) const;
	value iterator_get(Type collectionType, value it, value previous) const;
	value iterator_key(value v, value it, value previous) const;
	void iterator_increment(Type collectionType, value it) const;

	// Controls
	label insn_init_label(std::string name) const;
	void insn_if(value v, std::function<void()> then) const;
	void insn_if_new(value cond, label* then, label* elze) const;
	void insn_if_not(value v, std::function<void()> then) const;
	void insn_throw(value v) const;
	void insn_throw_object(vm::Exception type) const;
	void insn_label(label*) const;
	void insn_branch(label* l) const;
	void insn_branch_if_pc_not_in_range(label* a, label* b, label* n) const;
	void insn_return(value v) const;
	void insn_return_void() const;

	// Call functions
	template <typename R, typename... A>
	value insn_call(Type return_type, std::vector<value> args, R(*func)(A...), bool exception = false) const {
		return insn_call(return_type, args, (void*) func, exception);
	}
	value insn_call(Type return_type, std::vector<value> args, void* func, bool exception = false) const;
	value insn_call_indirect(Type return_type, value fun, std::vector<value> args) const;
	void function_add_capture(value fun, value capture);
	void log(const std::string&& str) const;

	// Blocks
	void enter_block();
	void leave_block();
	void delete_variables_block(int deepness); // delete all variables in the #deepness current blocks
	void enter_function(llvm::Function* F, bool is_closure, Function* fun);
	void leave_function();
	int get_current_function_blocks() const;
	void delete_function_variables() const;
	bool is_current_function_closure() const;

	// Variables
	value add_var(const std::string& name, value value);
	value create_and_add_var(const std::string& name, Type type);
	void add_function_var(value value);
	value get_var(const std::string& name);
	void set_var_type(std::string& name, const Type& type);
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
	void add_catcher(label start, label end, label handler);
	void insn_check_args(std::vector<value> args, std::vector<LSValueType> types) const;

	// Utils
	std::ostringstream& _log_insn(int indent) const;
	std::string dump_val(value v) const;
	void register_label(label* v) const;
	void log_insn_code(std::string instruction) const;
	void add_literal(void* ptr, std::string value) const;
	static void print_mpz(__mpz_struct value);
};

}

namespace std {
	std::ostream& operator << (std::ostream&, const __mpz_struct);
}

#endif
