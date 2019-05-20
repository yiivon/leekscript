#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <vector>
#include "../lexical/Ident.hpp"
#include "Value.hpp"
#include "Block.hpp"

namespace ls {

class SemanticVar;
class SemanticAnalyzer;

class Function : public Value {
public:

	static int id_counter;

	struct Version {
		Function* parent;
		Block* body;
		Type type;
		llvm::BasicBlock* landing_pad = nullptr;
		llvm::BasicBlock* catch_block = nullptr;
		llvm::AllocaInst* exception_slot = nullptr;
		llvm::AllocaInst* exception_line_slot = nullptr;
		llvm::BasicBlock* block = nullptr;
		llvm::Function* f = nullptr;
		Compiler::value value;
		bool is_compiled() const;
		void create_function(Compiler& c);
		void compile(Compiler& c, bool create_value = false, bool compile_body = true);
	};

	std::string name;
	std::string internal_name;
	bool lambda = false;
	std::vector<std::shared_ptr<Token>> arguments;
	std::vector<Value*> defaultValues;
	Block* body;

	std::vector<std::shared_ptr<SemanticVar>> captures;
	std::map<std::string, std::shared_ptr<SemanticVar>> vars;
	bool function_added;
	Function* parent;
	std::shared_ptr<const Base_type> placeholder_type;
	bool is_main_function = false;
	std::string file;
	Version* default_version = nullptr;
	std::map<std::vector<Type>, Version*> versions;
	bool generate_default_version = false;
	Version* current_version = nullptr;
	bool analyzed = false;
	Type return_type;
	Compiler* compiler = nullptr;
	int default_values_count = 0;
	bool recursive = false;

	Function();
	virtual ~Function();

	void addArgument(Token* token, Value* defaultValue);
	int capture(std::shared_ptr<SemanticVar> var);
	Type getReturnType();

	void print_version(std::ostream& os, int indent, bool debug, bool condensed, const Version* version) const;
	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	bool will_take(SemanticAnalyzer*, const std::vector<Type>&, int level) override;
	void set_version(const std::vector<Type>& args, int level) override;
	void analyze_body(SemanticAnalyzer* analyzer, std::vector<Type> args, Version* version);
	virtual Type version_type(std::vector<Type>) const override;
	virtual void must_return_any(SemanticAnalyzer*) override;
	virtual Callable* get_callable(SemanticAnalyzer*) const override;
	virtual void analyze(SemanticAnalyzer*) override;
	void create_version(SemanticAnalyzer* analyzer, std::vector<Type> args);

	virtual Compiler::value compile(Compiler&) const override;
	virtual Compiler::value compile_version(Compiler&, std::vector<Type>) const override;
	Compiler::value compile_default_version(Compiler&) const;
	void compile_version_internal(Compiler& c, std::vector<Type> args, Version* version) const;
	llvm::BasicBlock* get_landing_pad(const Compiler& c);
	void compile_return(const Compiler& c, Compiler::value v, bool delete_variables = false) const;
	void export_context(const Compiler& c) const;

	virtual Value* clone() const override;
};

}

#endif
