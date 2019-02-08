#include "VariableValue.hpp"
#include "math.h"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"
#include "../instruction/VariableDeclaration.hpp"
#include "../semantic/Callable.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSSet.hpp"

namespace ls {

VariableValue::VariableValue(std::shared_ptr<Token> token) : token(token) {
	this->name = token->content;
	this->var = nullptr;
	constant = false;
}

bool VariableValue::isLeftValue() const {
	return scope != VarScope::INTERNAL; // Internal variables are not left-value
}

void VariableValue::print(std::ostream& os, int, bool debug, bool condensed) const {
	os << token->content;
	if (debug) {
		os << " ";
		if (has_version && var != nullptr && var->value != nullptr)
			os << var->value->version_type(version);
		else
			os << type;
	}
}

Location VariableValue::location() const {
	return token->location;
}

Callable* VariableValue::get_callable(SemanticAnalyser* analyser) const {
	if (name == "+" or name == "-" or name == "*" or name == "×" or name == "/" or name == "÷" or name == "**" or name == "%") {
		auto callable = new Callable(name);
		auto type = Type::fun(Type::any(), {Type::any(), Type::any()});
		auto fun =  [&](Compiler& c, std::vector<Compiler::value> args) {
			if (name == "+") return c.insn_add(args[0], args[1]);
			else if (name == "-") return c.insn_sub(args[0], args[1]);
			else if (name == "*" or name == "×") return c.insn_mul(args[0], args[1]);
			else if (name == "/" or name == "÷") return c.insn_div(args[0], args[1]);
			else if (name == "**") return c.insn_pow(args[0], args[1]);
			else if (name == "%") return c.insn_mod(args[0], args[1]);
			assert(false);
		};
		callable->add_version({ name, type, fun, {}, {}, nullptr });
		auto type2 = Type::fun(Type::integer(), {Type::integer(), Type::integer()});
		if (name == "/" or name == "÷") type2 = Type::fun(Type::real(), {Type::integer(), Type::integer()});
		callable->add_version({ name, type2, fun, {}, {}, nullptr });
		return callable;
	}
	if (name == "Number") {
		auto callable = new Callable(name);
		callable->add_version({ "Number", Type::fun(Type::integer(), {}), [&](Compiler& c, std::vector<Compiler::value>) {
			return c.new_integer(0);
		}, {}, {}, nullptr });
		callable->add_version({ "Number", Type::fun(Type::real(), {Type::real()}), [&](Compiler& c, std::vector<Compiler::value> args) {
			return c.to_real(args[0]);
		}, {}, {}, nullptr });
		callable->add_version({ "Number", Type::fun(Type::mpz(), {Type::mpz()}), [&](Compiler& c, std::vector<Compiler::value> args) {
			return args[0];
		}, {}, {}, nullptr });
		return callable;
	}
	if (name == "Boolean") {
		auto callable = new Callable(name);
		auto type = Type::fun(Type::boolean(), {});
		callable->add_version({ "Boolean", type, [&](Compiler& c, std::vector<Compiler::value>) {
			return c.new_bool(false);	
		}, {}, {}, nullptr });
		return callable;
	}
	if (name == "String") {
		auto callable = new Callable(name);
		callable->add_version({ "String", Type::fun(Type::string(), {}), [&](Compiler& c, std::vector<Compiler::value>) {
			return c.new_pointer(new LSString(""), Type::string());
		}, {}, {}, nullptr });
		callable->add_version({ "String", Type::fun(Type::string(), {Type::string()}), [&](Compiler& c, std::vector<Compiler::value> args) {
			return args[0];
		}, {}, {}, nullptr });
		return callable;
	}
	if (name == "Array") {
		auto callable = new Callable(name);
		callable->add_version({ "Array", Type::fun(Type::array(Type::any()), {}), [&](Compiler& c, std::vector<Compiler::value>) {
			return c.new_array({}, {});
		}, {}, {}, nullptr });
		return callable;
	}
	if (name == "Object") {
		auto callable = new Callable(name);
		callable->add_version({ "Object", Type::fun(Type::array(Type::any()), {}), [&](Compiler& c, std::vector<Compiler::value>) {
			return c.new_object();
		}, {}, {}, nullptr });
		return callable;
	}
	if (name == "Set") {
		auto callable = new Callable(name);
		auto type = Type::fun(Type::set(Type::any()), {});
		callable->add_version({ "Set", type, [&](Compiler& c, std::vector<Compiler::value>) {
			return c.new_pointer(new LSSet<LSValue*>(), Type::set(Type::any()));
		}, {}, {}, nullptr });
		return callable;
	}
	if (type == Type::clazz()) {
		auto callable = new Callable(name);
		auto type = Type::fun(Type::any(), {Type::clazz()});
		callable->add_version({ name, type, [&](Compiler& c, std::vector<Compiler::value> args) {
			return c.new_object_class(args[0]);
		}, {}, {}, (Value*) this });
		return callable;
	}
	}
	return nullptr;
}

void VariableValue::analyse(SemanticAnalyser* analyser) {

	var = analyser->get_var(token.get());

	if (var != nullptr) {
		auto function_object = dynamic_cast<Function*>(var->value);
		if (var->value && function_object) {
			// Analyse the real function (if the function is defined below its call for example)
			if (!function_object->analyzed) {
				function_object->analyse(analyser);
			}
		}
		type = var->type();
		var->initial_type = type;
		scope = var->scope;
		attr_types = var->attr_types;
		if (scope != VarScope::INTERNAL and var->function != analyser->current_function()) {
			capture_index = analyser->current_function()->capture(var);
			var->index = capture_index;
			scope = VarScope::CAPTURE;
		}
	} else {
		type = Type::any();
	}
	type.temporary = false;

	//	cout << "VV " << name << " : " << type << endl;
	//	cout << "var scope : " << (int)var->scope << endl;
	//	for (auto t : attr_types)
	//	cout << t.first << " : " << t.second << endl;
}

bool VariableValue::will_take(SemanticAnalyser* analyser, const std::vector<Type>& args, int level) {
	if (var != nullptr and var->value != nullptr) {
		var->value->will_take(analyser, args, level);
		if (auto f = dynamic_cast<Function*>(var->value)) {
			if (f->versions.find(args) != f->versions.end()) {
				var->version = args;
				var->has_version = true;
			}
		}
		type = var->type();
	}
	set_version(args, level);
	return false;
}

void VariableValue::set_version(const std::vector<Type>& args, int level) {
	// std::cout << "VariableValue::set_version " << args << " " << level << std::endl;
	if (var != nullptr and var->value != nullptr) {
		var->value->set_version(args, level);
	}
	if (level == 1) {
		version = args;
		has_version = true;
	}
}

bool VariableValue::will_store(SemanticAnalyser* analyser, const Type& type) {
	if (var != nullptr and var->value != nullptr) {
		var->value->will_store(analyser, type);
		this->type = var->type();
	}
	return false;
}

bool VariableValue::elements_will_store(SemanticAnalyser* analyser, const Type& type, int level) {
	if (var != nullptr and var->value != nullptr) {
		var->value->elements_will_store(analyser, type, level);
		this->type = var->value->type.not_temporary();
	}
	return false;
}

void VariableValue::change_value(SemanticAnalyser*, Value* value) {
	if (var != nullptr) {
		var->value = value;
		var->value->type.constant = false;
	}
}

Type VariableValue::version_type(std::vector<Type> version) const {
	if (var != nullptr && var->value != nullptr) {
		// std::cout << "VariableValue " << this << " version_type() " << version << std::endl;
		return var->value->version_type(version);
	}
	return type;
}

Compiler::value VariableValue::compile(Compiler& c) const {
	// std::cout << "Compile var " << name << " " << version << std::endl;
	// cout << "compile vv " << name << " : " << type << "(" << (int) scope << ")" << endl;
	Compiler::value v;
	if (scope == VarScope::CAPTURE) {
		v = c.insn_get_capture(capture_index, type);
	} else if (scope == VarScope::INTERNAL) {
		v = c.vm->internals.at(name);
	} else if (scope == VarScope::LOCAL) {
		auto f = dynamic_cast<Function*>(var->value);
		if (has_version && f) {
			return f->compile_version(c, version);
		}
		v = c.insn_load(c.get_var(name));
	} else { /* if (scope == VarScope::PARAMETER) */
		v = c.insn_load(c.insn_get_argument(name));
	}
	assert(v.t == type);
	c.assert_value_ok(v);
	// std::cout << "return var " << v.v->getType() << std::endl;
	return v;
}

Compiler::value VariableValue::compile_l(Compiler& c) const {
	Compiler::value v;
	// No internal values here
	if (scope == VarScope::LOCAL) {
		v = c.get_var(name);
	} else if (scope == VarScope::CAPTURE) {
		v = c.insn_get_capture_l(capture_index, type);
	} else { /* if (scope == VarScope::PARAMETER) */
		v = c.insn_get_argument(name);
	}
	assert(type.pointer() == v.t);
	assert(type.llvm_type()->getPointerTo() == v.v->getType());
	return v;
}

Value* VariableValue::clone() const {
	auto vv = new VariableValue(token);
	return vv;
}

}
