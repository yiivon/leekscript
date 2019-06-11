#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <vector>
#include "../lexical/Ident.hpp"
#include "Value.hpp"
#include "Block.hpp"

namespace ls {

class SemanticVar;
class SemanticAnalyzer;
class FunctionVersion;

class Function : public Value {
public:

	static int id_counter;

	std::string name;
	std::string internal_name;
	bool lambda = false;
	std::shared_ptr<Token> token;
	std::vector<std::shared_ptr<Token>> arguments;
	std::vector<Value*> defaultValues;
	Block* body;

	std::vector<std::shared_ptr<SemanticVar>> captures;
	std::map<std::string, std::shared_ptr<SemanticVar>> vars;
	bool function_added;
	Function* parent;
	bool is_main_function = false;
	FunctionVersion* default_version = nullptr;
	std::map<std::vector<const Type*>, FunctionVersion*> versions;
	bool generate_default_version = false;
	FunctionVersion* current_version = nullptr;
	bool analyzed = false;
	Compiler* compiler = nullptr;
	int default_values_count = 0;

	Function(std::shared_ptr<Token> token);
	virtual ~Function();

	void addArgument(Token* token, Value* defaultValue);
	int capture(std::shared_ptr<SemanticVar> var);

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	bool will_take(SemanticAnalyzer*, const std::vector<const Type*>&, int level) override;
	void set_version(SemanticAnalyzer* analyser, const std::vector<const Type*>& args, int level) override;
	virtual const Type* version_type(std::vector<const Type*>) const override;
	virtual void must_return_any(SemanticAnalyzer*) override;
	virtual Call get_callable(SemanticAnalyzer*, int argument_count) const override;
	virtual void analyze(SemanticAnalyzer*) override;
	void create_version(SemanticAnalyzer* analyzer, std::vector<const Type*> args);

	virtual Compiler::value compile(Compiler&) const override;
	virtual Compiler::value compile_version(Compiler&, std::vector<const Type*>) const override;
	Compiler::value compile_default_version(Compiler&) const;
	llvm::BasicBlock* get_landing_pad(const Compiler& c);
	void compile_return(const Compiler& c, Compiler::value v, bool delete_variables = false) const;
	void export_context(const Compiler& c) const;

	virtual Value* clone() const override;
};

}

#endif
