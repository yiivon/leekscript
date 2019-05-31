#ifndef OBJECTACCESS_HPP
#define OBJECTACCESS_HPP

#include <string>
#include "LeftValue.hpp"
#include "Value.hpp"
#include "../../vm/value/LSString.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class Method;

class ObjectAccess : public LeftValue {
public:

	Value* object;
	std::shared_ptr<Token> field;
	std::string object_class_name;
	std::string class_name;
	bool class_method = false;
	std::vector<CallableVersion>* callable = nullptr;
	bool class_field = false;
	void* attr_addr = nullptr;
	std::function<Compiler::value(Compiler&)> static_access_function = nullptr;
	std::function<Compiler::value(Compiler&, Compiler::value)> access_function = nullptr;
	std::string native_access_function = "";
	std::string native_static_access_function = "";
	Type field_type;
	LSFunction* ls_function = nullptr;

	ObjectAccess(std::shared_ptr<Token> token);
	virtual ~ObjectAccess();

	virtual bool isLeftValue() const override;
	
	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	void set_version(const std::vector<Type>& args, int level) override;
	bool will_take(SemanticAnalyzer*, const std::vector<Type>&, int level) override;
	virtual Call get_callable(SemanticAnalyzer*, int argument_count) const;
	virtual Type version_type(std::vector<Type>) const override;
	virtual void analyze(SemanticAnalyzer*) override;

	virtual Compiler::value compile(Compiler&) const override;
	virtual Compiler::value compile_version(Compiler& c, std::vector<Type> version) const;
	virtual Compiler::value compile_l(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
