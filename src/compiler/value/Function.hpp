#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <vector>
#include "../lexical/Ident.hpp"
#include "Value.hpp"
#include "Block.hpp"

namespace ls {

class SemanticVar;
class SemanticAnalyser;

class Function : public Value {
public:

	static int id_counter;

	struct Version {
		Block* body;
		LSFunction* function = nullptr;
		Type type;
		llvm::BasicBlock* landing_pad = nullptr;
		llvm::BasicBlock* catch_block = nullptr;
		llvm::AllocaInst* exception_slot = nullptr;
		llvm::AllocaInst* exception_line_slot = nullptr;
		bool is_compiled() const;
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
	bool compiled = false;
	bool generate_default_version = false;
	Version* current_version = nullptr;
	bool analyzed = false;
	Type return_type;
	llvm::orc::VModuleKey module_handle;
	bool handle_created = false;
	llvm::BasicBlock* block = nullptr;
	llvm::Module* module;
	Compiler* compiler = nullptr;
	int default_values_count = 0;

	Function();
	virtual ~Function();

	void addArgument(Token* token, Value* defaultValue);
	int capture(std::shared_ptr<SemanticVar> var);
	Type getReturnType();

	void print_version(std::ostream& os, int indent, bool debug, bool condensed, const Version* version) const;
	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	bool will_take(SemanticAnalyser*, const std::vector<Type>&, int level) override;
	void set_version(const std::vector<Type>& args, int level) override;
	void analyse_body(SemanticAnalyser* analyser, std::vector<Type> args, Version* version);
	void update_function_args(SemanticAnalyser*);
	virtual Type version_type(std::vector<Type>) const override;
	virtual void must_return_any(SemanticAnalyser*) override;
	virtual Callable* get_callable(SemanticAnalyser*) const override;
	virtual void analyse(SemanticAnalyser*) override;
	void create_version(SemanticAnalyser* analyser, std::vector<Type> args);

	virtual Compiler::value compile(Compiler&) const override;
	virtual Compiler::value compile_version(Compiler&, std::vector<Type>) const override;
	Compiler::value compile_default_version(Compiler&) const;
	void compile_version_internal(Compiler& c, std::vector<Type> args, Version* version) const;
	llvm::BasicBlock* get_landing_pad(const Compiler& c);
	void compile_return(const Compiler& c, Compiler::value v) const;

	virtual Value* clone() const override;
};

}

#endif
