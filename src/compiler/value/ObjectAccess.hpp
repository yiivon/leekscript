#ifndef OBJECTACCESS_HPP
#define OBJECTACCESS_HPP

#include <string>
#include "LeftValue.hpp"
#include "Value.hpp"
#include "../../vm/value/LSString.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class ObjectAccess : public LeftValue {
public:

	Value* object;
	std::shared_ptr<Token> field;
	std::string object_class_name;
	std::string class_name;
	bool class_method = false;
	bool class_field = false;
	void* attr_addr;
	std::function<Compiler::value(Compiler&)> static_access_function = nullptr;
	std::function<Compiler::value(Compiler&, Compiler::value)> access_function = nullptr;
	void* native_access_function = nullptr;
	void* native_static_access_function = nullptr;
	Type field_type;
	LSFunction* ls_function = nullptr;

	ObjectAccess(std::shared_ptr<Token> token);
	virtual ~ObjectAccess();

	virtual bool isLeftValue() const override;
	
	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void analyse(SemanticAnalyser*) override;

	virtual Compiler::value compile(Compiler&) const override;
	virtual Compiler::value compile_version(Compiler& c, std::vector<Type> version) const;
	virtual Compiler::value compile_l(Compiler&) const override;

	virtual Value* clone() const override;
};

}

#endif
