#include "VariableValue.hpp"
#include "math.h"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"
#include "../instruction/VariableDeclaration.hpp"

using namespace std;

namespace ls {

VariableValue::VariableValue(std::shared_ptr<Token> token) : token(token) {
	this->name = token->content;
	this->var = nullptr;
	constant = false;
}

void VariableValue::print(ostream& os, int, bool debug, bool condensed) const {
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

bool VariableValue::will_take(SemanticAnalyser* analyser, const vector<Type>& args, int level) {
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

void VariableValue::set_version(const vector<Type>& args, int level) {
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

	if (scope == VarScope::CAPTURE) {
		return c.insn_get_capture(capture_index, type);
	}
	Compiler::value v;
	if (scope == VarScope::INTERNAL) {
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
	if (var->type().reference) {
		return c.insn_load(v);
	}
	// std::cout << "return var " << v.v->getType() << std::endl;
	return v;
}

Compiler::value VariableValue::compile_l(Compiler& c) const {
	Compiler::value v;
	// No internal values here
	if (scope == VarScope::LOCAL) {
		v = c.get_var(name);
	} else if (scope == VarScope::CAPTURE) {
		v = c.insn_get_capture(capture_index, type);
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
