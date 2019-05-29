#include <chrono>
#include "Program.hpp"
#include "Context.hpp"
#include "value/LSNull.hpp"
#include "value/LSArray.hpp"
#include "../compiler/lexical/LexicalAnalyzer.hpp"
#include "../compiler/syntaxic/SyntaxicAnalyzer.hpp"
#include "Context.hpp"
#include "../compiler/semantic/SemanticAnalyzer.hpp"
#include "../compiler/error/Error.hpp"
#include "../util/Util.hpp"
#include "../constants.h"
#include "../colors.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "../compiler/value/Expression.hpp"
#include "../compiler/instruction/ExpressionInstruction.hpp"
#include "../compiler/value/VariableValue.hpp"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Bitcode/BitcodeReader.h"

namespace ls {

Program::Program(const std::string& code, const std::string& file_name) {
	this->code = code;
	this->file_name = file_name;
	main = nullptr;
	closure = nullptr;
}

Program::~Program() {
	if (main != nullptr) {
		delete main;
	}
	if (handle_created) {
		vm->compiler.removeModule(module_handle);
	}
}

VM::Result Program::compile_leekscript(VM& vm, Context* ctx, bool bitcode, bool pseudo_code) {

	auto resolver = new Resolver();

	VM::Result result;
	auto file = new File(file_name, code, new FileContext());
	SyntaxicAnalyzer syn { resolver };
	auto block = syn.analyze(file);

	if (file->errors.size() > 0) {
		result.compilation_success = false;
		result.errors = file->errors;
		return result;
	}

	this->main = new Function();
	this->main->body = block;
	this->main->is_main_function = true;

	// Semantical analysis
	SemanticAnalyzer sem;
	sem.vm = &vm;
	sem.analyze(this, ctx);

	std::ostringstream oss;
	print(oss, true);
	result.program = oss.str();

	if (sem.errors.size()) {
		result.compilation_success = false;
		result.errors = sem.errors;
		return result;
	}

	// Compilation
	vm.internals.clear();
	vm.compiler.program = this;
	vm.compiler.init();

	module = new llvm::Module(file_name, vm.compiler.getContext());
	module->setDataLayout(vm.compiler.DL);

	main->compile(vm.compiler);

	if (pseudo_code) {
		std::error_code EC2;
		llvm::raw_fd_ostream ir(file_name + ".ll", EC2, llvm::sys::fs::F_None);
		module->print(ir, nullptr);
		ir.flush();
	}

	module_handle = vm.compiler.addModule(std::unique_ptr<llvm::Module>(module), true, bitcode);
	handle_created = true;
	auto ExprSymbol = vm.compiler.findSymbol("main");
	assert(ExprSymbol && "Function not found");

	closure = (void*) cantFail(ExprSymbol.getAddress());
	type = main->type.return_type().fold();

	result.compilation_success = true;

	return result;
}

VM::Result Program::compile_ir_file(VM& vm) {
	VM::Result result;
	llvm::SMDiagnostic Err;
	auto Mod = llvm::parseIRFile(file_name, Err, vm.compiler.getContext());
	if (!Mod) {
		Err.print("main", llvm::errs());
		result.compilation_success = false;
		result.program = "<error>";
		return result;
	}
	auto llvm_type = Mod->getFunction("main")->getReturnType();
	vm.compiler.addModule(std::move(Mod), true);
	auto symbol = vm.compiler.findSymbol("main");
	closure = (void*) cantFail(symbol.getAddress());

	type = llvm_type->isPointerTy() ? Type::any() : (llvm_type->isStructTy() ? Type::mpz() : Type::integer());

	result.compilation_success = true;
	std::ostringstream oss;
	oss << llvm_type;
	result.program = type.to_string() + " " + oss.str();
	return result;
}

VM::Result Program::compile_bitcode_file(VM& vm) {
	VM::Result result;
	auto EMod = llvm::parseBitcodeFile(llvm::MemoryBufferRef { *llvm::MemoryBuffer::getFile(file_name).get() }, vm.compiler.getContext());
	if (!EMod) {
		llvm::errs() << EMod.takeError() << '\n';
		result.compilation_success = false;
		result.program = "<error>";
		return result;
	}
	auto Mod = std::move(EMod.get());
	auto llvm_type = Mod->getFunction("main")->getReturnType();
	vm.compiler.addModule(std::move(Mod), false); // Already optimized
	auto symbol = vm.compiler.findSymbol("main");
	closure = (void*) cantFail(symbol.getAddress());

	type = llvm_type->isPointerTy() ? Type::any() : (llvm_type->isStructTy() ? Type::mpz() : Type::integer());

	result.compilation_success = true;
	std::ostringstream oss;
	oss << llvm_type;
	result.program = type.to_string() + " " + oss.str();
	return result;
}

VM::Result Program::compile(VM& vm, Context* ctx, bool export_bitcode, bool pseudo_code, bool ir, bool bitcode) {
	this->vm = &vm;

	if (ir) {
		return compile_ir_file(vm);
	} else if (bitcode) {
		return compile_bitcode_file(vm);
	} else {
		return compile_leekscript(vm, ctx, export_bitcode, pseudo_code);
	}
}

std::shared_ptr<SemanticVar> Program::get_operator(std::string name) {
	// std::cout << "Program::get_operator(" << name << ")" << std::endl;

	auto op = operators.find(name);
	if (op != operators.end()) {
		return op->second;
	}

	std::vector<std::string> ops = {"+", "-", "*", "×", "/", "÷", "**", "%", "\\", "~", ">", "<", ">=", "<="};
	std::vector<TokenType> token_types = {TokenType::PLUS, TokenType::MINUS, TokenType::TIMES, TokenType::TIMES, TokenType::DIVIDE, TokenType::DIVIDE, TokenType::POWER, TokenType::MODULO, TokenType::INT_DIV, TokenType::TILDE, TokenType::GREATER, TokenType::LOWER, TokenType::GREATER_EQUALS, TokenType::LOWER_EQUALS};
	
	auto o = std::find(ops.begin(), ops.end(), name);
	if (o == ops.end()) return nullptr;

	auto f = new Function();
	f->addArgument(new Token(TokenType::IDENT, 0, 1, 0, "x"), nullptr);
	f->addArgument(new Token(TokenType::IDENT, 2, 1, 2, "y"), nullptr);
	f->body = new Block(true);
	auto ex = new Expression();
	ex->v1 = new VariableValue(std::make_shared<Token>(TokenType::IDENT, 0, 1, 0, "x"));
	ex->v2 = new VariableValue(std::make_shared<Token>(TokenType::IDENT, 2, 1, 2, "y"));
	ex->op = std::make_shared<Operator>(new Token(token_types.at(std::distance(ops.begin(), o)), 1, 1, 1, name));
	f->body->instructions.push_back( new ExpressionInstruction(ex));
	auto type = Type::fun(Type::any(), {Type::any(), Type::any()});
	type.native = true;

	auto var = std::make_shared<SemanticVar>(name, VarScope::INTERNAL, type, 0, f, nullptr, f, nullptr);
	operators.insert({name, var});
	return var;
}

void Program::analyze(SemanticAnalyzer* analyzer) {
	main->name = "main";
	main->file = file_name;
	main->body->analyze_global_functions(analyzer);

	main->analyze(analyzer);
}

std::string Program::execute(VM& vm) {

	assert(!type.reference && "Program return type shouldn't be a reference!");

	if (type._types.size() == 0) {
		auto fun = (void (*)()) closure;
		fun();
		return "(void)";
	}
	if (type.not_temporary() == Type::boolean()) {
		auto fun = (bool (*)()) closure;
		bool res = fun();
		return res ? "true" : "false";
	}
	if (type.not_temporary() == Type::integer()) {
		auto fun = (int (*)()) closure;
		int res = fun();
		return std::to_string(res);
	}
	if (type.not_temporary() == Type::mpz()) {
		auto fun = (__mpz_struct (*)()) closure;
		auto ret = fun();
		char buff[1000000];
		mpz_get_str(buff, 10, &ret);
		mpz_clear(&ret);
		vm.mpz_deleted++;
		return std::string(buff);
	}
	if (type.not_temporary() == Type::real()) {
		auto fun = (double (*)()) closure;
		double res = fun();
		return LSNumber::print(res);
	}
	if (type.not_temporary() == Type::long_()) {
		auto fun = (long (*)()) closure;
		long res = fun();
		return std::to_string(res);
	}
	auto fun = (LSValue* (*)()) closure;
	auto ptr = fun();
	std::ostringstream oss;
	ptr->dump(oss, 5);
	LSValue::delete_ref(ptr);
	return oss.str();
}

void Program::print(std::ostream& os, bool debug) const {
	if (main) {
		main->body->print(os, 0, debug, false);
	} else {
		os << "(ll file)";
	}
}

std::ostream& operator << (std::ostream& os, const Program* program) {
	program->print(os, false);
	return os;
}

std::string Program::underline_code(Location location, Location focus) const {
	// std::cout << "underline " << location.start.column << " " << location.end.column << " " << focus.start.column << " " << focus.end.column << std::endl;
	auto padding = 10ul;
	auto start = padding > location.start.raw ? 0ul : location.start.raw - padding;
	auto end = std::min(code.size(), location.end.raw + padding);
	auto padding_left = std::min(padding, location.start.raw - start);
	auto padding_right = std::min(padding, end - location.end.raw);
	auto ellipsis_left = start > 0;
	auto ellipsis_right = end < code.size();

	auto extract = code.substr(start, end - start);
	auto underlined = extract.substr(padding_left, end - start - padding_left - padding_right);
	auto before = extract.substr(0, padding_left);
	auto after = extract.substr(extract.size() - padding_right, padding_right);

	size_t p = before.rfind('\n');
	if (p != std::string::npos) {
		before = before.substr(p + 1, before.size() - p);
		ellipsis_left = false;
	}
	p = after.find('\n');
	if (p != std::string::npos) {
		after = after.substr(0, p);
		ellipsis_right = false;
	}

	auto focus_start = focus.start.raw - location.start.raw;
	auto focus_size = focus.end.raw - focus.start.raw;
	underlined = underlined.substr(0, focus_start)
		+ C_RED + underlined.substr(focus_start, focus_size) + END_COLOR
		+ UNDERLINE + underlined.substr(focus_size + focus_start);

	if (before.size() and before.front() != ' ')
		before = ' ' + before;
	if (after.size() and after.back() != ' ')
		after = after + ' ';

	return (ellipsis_left ? (C_GREY "[...]" END_COLOR) : "")
		+ before + UNDERLINE + underlined + END_STYLE + after
		+ (ellipsis_right ? (C_GREY "[...]" END_COLOR) : "");
}

}
