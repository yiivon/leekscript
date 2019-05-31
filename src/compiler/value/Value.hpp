#ifndef VALUE_HPP
#define VALUE_HPP

#include <map>
#include <vector>
#include "../Compiler.hpp"
#include "../lexical/Location.hpp"
#include "../semantic/Call.hpp"

namespace ls {

class SemanticAnalyzer;

class Value {
public:
	Type type;
	Type return_type;
	bool returning = false;
	bool may_return = false;
	std::vector<Type> version;
	bool has_version = false;
	std::map<std::string, Type> attr_types;
	bool constant;
	bool parenthesis = false;
	std::string default_version_fun;
	std::map<std::vector<Type>, std::string> versions;
	bool is_void = false;
	bool throws = false;

	Value();
	virtual ~Value();

	virtual bool isLeftValue() const;
	virtual bool is_zero() const;

	// TODO PrintOptions to merge parameters
	virtual void print(std::ostream&, int indent = 0, bool debug = false, bool condensed = false) const = 0;
	std::string to_string() const;

	virtual Location location() const = 0;

	virtual bool will_take(SemanticAnalyzer*, const std::vector<Type>& args_type, int level);
	virtual bool will_store(SemanticAnalyzer*, const Type&);
	virtual bool elements_will_store(SemanticAnalyzer*, const Type&, int level);
	virtual bool must_be_any(SemanticAnalyzer*);
	virtual void must_return_any(SemanticAnalyzer*);
	virtual void set_version(const std::vector<Type>&, int level);
	virtual Type version_type(std::vector<Type>) const;
	virtual Call get_callable(SemanticAnalyzer*, int argument_count) const;
	virtual void analyze(SemanticAnalyzer*);

	virtual Compiler::value compile(Compiler&) const = 0;
	virtual Compiler::value compile_version(Compiler&, std::vector<Type>) const;
	virtual void compile_end(Compiler&) const {}

	virtual Value* clone() const = 0;

	static std::string tabs(int indent);
};

std::ostream& operator << (std::ostream& os, const Value* v);

}

#endif
