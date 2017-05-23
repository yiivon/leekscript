#include "Function.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../instruction/ExpressionInstruction.hpp"
#include "../../vm/value/LSFunction.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/Program.hpp"
#include <jit/jit-dump.h>
using namespace std;

namespace ls {

Function::Function() {
	body = nullptr;
	parent = nullptr;
	pos = 0;
	constant = true;
	function_added = false;
}

Function::~Function() {
	delete body;
	for (auto value : defaultValues) {
		delete value;
	}
	if (ls_fun != nullptr) {
		delete ls_fun;
		ls_fun = nullptr;
	}
}

void Function::addArgument(Token* name, bool reference, Value* defaultValue) {
	arguments.push_back(std::unique_ptr<Token> { name });
	references.push_back(reference);
	defaultValues.push_back(defaultValue);
}

Type Function::getReturnType() {
	if (type.getReturnType() == Type::UNKNOWN) {
		if (placeholder_type == Type::UNKNOWN) {
			placeholder_type = Type::generate_new_placeholder_type();
		}
		return placeholder_type;
	} else {
		return type.getReturnType();
	}
}

void Function::print(std::ostream& os, int indent, bool debug) const {

	if (captures.size() > 0) {
		os << "[";
		for (unsigned c = 0; c < captures.size(); ++c) {
			if (c > 0) os << ", ";
			os << captures[c]->name << " " << captures[c]->type;
		}
		os << "] ";
	}

	os << "(";
	for (unsigned i = 0; i < arguments.size(); ++i) {
		if (i > 0) os << ", ";
		if (references.at(i)) {
			os << "@";
		}
		os << arguments.at(i)->content;
		if (debug)
			os << " " << type.getArgumentType(i);

		if (defaultValues.at(i) != nullptr) {
			os << " = ";
			defaultValues.at(i)->print(os);
		}
	}

	os << ") → ";
	body->print(os, indent, debug);

	if (debug) {
		//os << " " << type;
	}
}

Location Function::location() const {
	return body->location();
}

/*
 * When returing a function, compile a default version with all parameters
 * as pointers, when the function will be in the nature, there will be no
 * problem. Inside a block, it's possible to compile a more specific version
 * of a function in this block to speed up things

let f = x -> x + '!'
let g2 = f('hello')   // default version with pointer
let g1 = f(10)		   // version with [x = int]

let f = x -> y -> x + y
let g = f('hello ')	// default version of f
let h1 = g(10)			// here we act like we don't know where does 'g' come from, and take the default version with pointer
let h2 = g('world')	// same version, pointers

let f = function(a) {
	let g = x -> x + a
	g('hello')	// default version with pointer
	g(12)		// version with [x = int]
	return g	// here, g will go outside his parent, so we take the full-pointer version, regardless of the version of function f. So f can be recompile multiple times, it will not affect the arguments of g and its return type.
}
let r1 = f(' !')		// default version
let r2 = f(12)			// version with number, recompiler f with a [a = int] version, it will not modify the signature of g
r2(12)
r2('hello')
 */
void Function::analyse(SemanticAnalyser* analyser, const Type& req_type) {

//	cout << "Function::analyse req_type " << req_type << endl;

	parent = analyser->current_function();

	if (!function_added) {
		analyser->add_function(this);
		function_added = true;
	}

	type = Type::FUNCTION_P;

	for (unsigned int i = 0; i < arguments.size(); ++i) {
		auto argument_type = Type::UNKNOWN;
		if (defaultValues[i] != nullptr) {
			defaultValues[i]->analyse(analyser, Type::UNKNOWN);
			argument_type = defaultValues[i]->type;
		}
		type.setArgumentType(i, argument_type, defaultValues[i] != nullptr);
	}

	for (unsigned int i = 0; i < req_type.getArgumentTypes().size(); ++i) {
		type.setArgumentType(i, req_type.getArgumentType(i));
	}

	analyse_body(analyser, req_type.getReturnType());

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
	if (ls_fun == nullptr) {
		ls_fun = new LSFunction<LSValue*>(nullptr);
		ls_fun->refs = 1;
		ls_fun->native = true;

		ls_fun->return_type = LSNull::get();
	}
	update_function_args(analyser);
	type.nature = Nature::POINTER;

//	cout << "Function type: " << type << endl;
}

bool Function::will_take(SemanticAnalyser* analyser, const std::vector<Type>& args, int level) {

//	cout << "Function::will_take " << arg_type << " at " << pos << endl;

	if (level == 1) {
		bool changed = type.will_take(args);
		if (changed) {
			analyse_body(analyser, Type::UNKNOWN);
		}
		update_function_args(analyser);
		return changed;
	} else {
		if (auto ei = dynamic_cast<ExpressionInstruction*>(body->instructions[0])) {
			if (auto f = dynamic_cast<Function*>(ei->value)) {

				analyser->enter_function(this);
				for (unsigned i = 0; i < arguments.size(); ++i) {
					analyser->add_parameter(arguments[i].get(), type.getArgumentType(i));
				}

				f->will_take(analyser, args, level - 1);

				analyser->leave_function();

				analyse_body(analyser, Type::UNKNOWN);
			}
		}
	}
	return false;
}

void Function::must_return(SemanticAnalyser* analyser, const Type& ret_type) {

//	cout << "Function::must_return : " << ret_type << endl;

	type.setReturnType(ret_type);

	analyse_body(analyser, ret_type);
}

void Function::analyse_body(SemanticAnalyser* analyser, const Type& req_type) {

	captures.clear();

	analyser->enter_function(this);

	for (unsigned i = 0; i < arguments.size(); ++i) {
		analyser->add_parameter(arguments[i].get(), type.getArgumentType(i));
	}

	type.setReturnType(Type::UNKNOWN);
	body->analyse(analyser, req_type);

	TypeList return_types;
	// Ignore recursive types
	for (const auto& t : body->types) {
		if (placeholder_type != Type::UNKNOWN and t == placeholder_type) {
			continue;
		}
		return_types.add(t);
	}

	if (body->types.size() >= 2) {
		// The body had multiple types, compute a compatible type and re-analyse it
		Type return_type = return_types[0];
		for (const auto& t : return_types) {
			return_type = Type::get_compatible_type(return_type, t);
		}
		type.return_types.clear();
		type.setReturnType(return_type);
		body->analyse(analyser, return_type); // second pass
	} else {
		if (return_types.size() > 0) {
			type.setReturnType(return_types[0]);
		} else {
			type.setReturnType(body->type);
		}
	}

	if (type.getReturnType() == Type::MPZ) {
		type.setReturnType(Type::MPZ_TMP);
	}

	vars = analyser->get_local_vars();
	analyser->leave_function();

	types = type;
//	cout << "function analyse body : " << type << endl;
}

int Function::capture(std::shared_ptr<SemanticVar> var) {
	for (size_t i = 0; i < captures.size(); ++i) {
		if (captures[i]->name == var->name)
			return i;
	}
	var = std::make_shared<SemanticVar>(*var);
	captures.push_back(var);

	if (var->function != parent) {
		auto new_var = std::make_shared<SemanticVar>(*var);
		new_var->index = parent->capture(new_var);
		var->scope = VarScope::CAPTURE;
		var->parent_index = new_var->index;
	}
	type.nature = Nature::POINTER;
	return captures.size() - 1;
}

void Function::update_function_args(SemanticAnalyser* analyser) {
	ls_fun->args.clear();
	for (unsigned int i = 0; i < arguments.size(); ++i) {
		auto& clazz = type.getArgumentType(i).raw_type->getClass();
		LSClass* arg_class = (LSClass*) analyser->vm->system_vars[clazz];
		if (arg_class != nullptr) {
			ls_fun->args.push_back((LSValue*) arg_class);
		} else {
			ls_fun->args.push_back(analyser->vm->system_vars["Value"]);
		}
	}
	auto return_class_name = type.getReturnType().raw_type->getClass();
	LSClass* return_class = (LSClass*) analyser->vm->system_vars[return_class_name];
	if (return_class != nullptr) {
		ls_fun->return_type = (LSValue*) return_class;
	} else {
		ls_fun->return_type = analyser->vm->system_vars["Value"];
	}
}

Compiler::value Function::compile(Compiler& c) const {

	// Add captures (for sub functions only)
	Compiler::value jit_fun;
	if (!is_main_function) {
		jit_fun = c.new_pointer(ls_fun);
		for (const auto& cap : captures) {
			jit_value_t jit_cap;
			if (cap->scope == VarScope::LOCAL) {
				jit_cap = c.get_var(cap->name).v;
			} else if (cap->scope == VarScope::CAPTURE) {
				jit_cap = c.insn_get_capture(cap->parent_index, cap->initial_type).v;
			} else {
				jit_cap = jit_value_get_param(c.F, 1 + cap->index);
			}
			if (cap->initial_type.nature != Nature::POINTER) {
				jit_cap = VM::value_to_pointer(c.F, jit_cap, cap->initial_type);
			}
			c.function_add_capture(jit_fun, {jit_cap, cap->type});
		}
	}

	unsigned arg_count = arguments.size() + 1;
	vector<jit_type_t> params = {LS_POINTER}; // first arg is the function pointer
	for (unsigned i = 0; i < arg_count - 1; ++i) {
		Type t = type.getArgumentType(i);
		params.push_back(VM::get_jit_type(t));
	}
	jit_type_t return_type = VM::get_jit_type(type.getReturnType());
	jit_type_t signature = jit_type_create_signature(jit_abi_cdecl, return_type, params.data(), arg_count, 1);
	((Function*) this)->jit_function = jit_function_create(c.vm->jit_context, signature);
	jit_function_set_meta(jit_function, 12, new std::string(name), nullptr, 0);
	jit_function_set_meta(jit_function, 13, new std::string(file), nullptr, 0);
	jit_type_free(signature);
	jit_insn_uses_catcher(jit_function);

	c.enter_function(jit_function);

	// System internal variables (for main function only)
	if (is_main_function) {
		for (auto var : c.vm->system_vars) {
			auto name = var.first;
			auto value = var.second;
			auto val = c.new_pointer(value);
			c.vm->internals.insert(pair<string, jit_value_t>(name, val.v));
		}
	}

	auto res = body->compile(c);

	jit_insn_return(jit_function, res.v);

	// catch (ex)
	auto ex = jit_insn_start_catcher(jit_function);
	auto catchers = c.catchers.back();
	if (catchers.size() > 0) {
		for (size_t i = 0; i < catchers.size() - 1; ++i) {
			auto ca = catchers[i];
			jit_insn_branch_if_pc_not_in_range(jit_function, ca.start, ca.end, &ca.next);
			jit_insn_branch(jit_function, &ca.handler);
			jit_insn_label(jit_function, &ca.next);
		}
		jit_insn_branch(jit_function, &catchers.back().handler);
	} else {
		c.delete_function_variables();
		if (is_main_function) {
			c.vm->store_exception(jit_function, ex);
		} else {
			jit_insn_rethrow_unhandled(jit_function);
		}
	}

	jit_function_compile(jit_function);

	if (!is_main_function) {
		c.leave_function();
		// Create a function : 1 op
		c.inc_ops(1);
	}

	// Function are always pointers for now
	// functions as a simple pointer value can be like :
	// {c.new_pointer(f).v, type};
	void* f = jit_function_to_closure(jit_function);
	ls_fun->function = f;

	if (is_main_function) {
		return {nullptr, Type::VOID};
	} else {
		return {jit_fun.v, type};
	}
}

Value* Function::clone() const {
	return (Value*) this;
}

}
