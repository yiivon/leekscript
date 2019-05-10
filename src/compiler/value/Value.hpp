#ifndef VALUE_HPP
#define VALUE_HPP

#include <map>
#include <vector>
#include "../Compiler.hpp"
#include "../lexical/Location.hpp"

namespace ls {

class SemanticAnalyser;
class Callable;

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
	void* default_version_fun = nullptr;
	std::map<std::vector<Type>, void*> versions;
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

	virtual bool will_take(SemanticAnalyser*, const std::vector<Type>& args_type, int level);
	virtual bool will_store(SemanticAnalyser*, const Type&);
	virtual bool elements_will_store(SemanticAnalyser*, const Type&, int level);
	virtual bool must_be_any(SemanticAnalyser*);
	virtual void must_return_any(SemanticAnalyser*);
	virtual void will_be_in_array(SemanticAnalyser*);
	virtual void set_version(const std::vector<Type>&, int level);
	virtual Type version_type(std::vector<Type>) const;
	virtual Callable* get_callable(SemanticAnalyser*) const;
	virtual void analyse(SemanticAnalyser*);

	virtual Compiler::value compile(Compiler&) const = 0;
	virtual Compiler::value compile_version(Compiler&, std::vector<Type>) const;
	virtual void compile_end(Compiler&) const {}

	virtual Value* clone() const = 0;

	static std::string tabs(int indent);
};

std::ostream& operator << (std::ostream& os, const Value* v);

}

#endif
