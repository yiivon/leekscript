#include "FunctionCall.hpp"
#include <sstream>
#include <string>
#include "../../vm/Module.hpp"
#include "../../vm/Program.hpp"
#include "../../type/Type.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSClass.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../../vm/value/LSObject.hpp"
#include "../../vm/value/LSString.hpp"
#include "../Compiler.hpp"
#include "../lexical/Token.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticError.hpp"
#include "ObjectAccess.hpp"
#include "VariableValue.hpp"
#include "../semantic/Callable.hpp"

namespace ls {

FunctionCall::FunctionCall(std::shared_ptr<Token> t) : token(t) {
	function = nullptr;
	std_func = nullptr;
	this_ptr = nullptr;
	constant = false;
}

FunctionCall::~FunctionCall() {
	delete function;
	for (const auto& arg : arguments) {
		delete arg;
	}
}

void FunctionCall::print(std::ostream& os, int indent, bool debug, bool condensed) const {

	function->print(os, indent, debug, condensed);
	os << "(";
	for (unsigned i = 0; i < arguments.size(); ++i) {
		arguments.at(i)->print(os, indent, debug, condensed);
		if (i < arguments.size() - 1) {
			os << ", ";
		}
	}
	os << ")";
	if (debug) {
		os << " " << type;
	}
}

Location FunctionCall::location() const {
	return {function->location().start, closing_parenthesis->location.end};
}

void FunctionCall::analyse(SemanticAnalyser* analyser) {

	// std::cout << "FC " << this << " : " << req_type << std::endl;

	// Analyse the function (can be anything here)
	function->analyse(analyser);

	// Analyse arguments
	for (const auto& argument : arguments) {
		argument->analyse(analyser);
	}

	// Retrieve the callable version
	callable = function->get_callable(analyser);
	if (/*callable == nullptr or */not function->type.can_be_callable()) {
		analyser->add_error({SemanticError::Type::CANNOT_CALL_VALUE, location(), function->location(), {function->to_string()}});
	}
	if (callable) {
		// std::cout << "Callable: " << callable << std::endl;
		std::vector<Type> arguments_types;
		for (const auto& argument : arguments) {
			arguments_types.push_back(argument->type);
		}
		callable_version = callable->resolve(analyser, arguments_types);
		if (callable_version) {
			// std::cout << "Version: " << callable_version << std::endl;
			type = callable_version->type.return_type();
			// Apply mutators
			// for (const auto& mutator : callable_version->mutators) {
			// 	mutator->apply(analyser, arguments);
			// }
			// int offset = callable_version->object ? 1 : 0;
			// for (size_t a = 0; a < arguments.size(); ++a) {
			// 	auto argument_type = callable_version->type.argument(a);
			// 	if (argument_type.is_function()) {
			// 		arguments.at(a - offset)->will_take(analyser, argument_type.arguments(), 1);
			// 		arguments.at(a - offset)->set_version(argument_type.arguments(), 1);
			// 		arguments.at(a - offset)->must_return(analyser, argument_type.return_type());
			// 	}
			// }
			// return;
		} else {
			// std::cout << "No version found!" << std::endl;
			// analyser->add_error({SemanticError::Type::WRONG_ARGUMENT_COUNT,	location(), location(), {
			// 	function->to_string(),
			// 	std::to_string(function->type.arguments().size()),
			// 	std::to_string(arguments.size())
			// }});
			// return;
		}
	} else {
		// std::cout << "No callable! " << function << std::endl;
	}

	// Find the function object
	function_object = dynamic_cast<Function*>(function);
	if (!function_object) {
		auto vv = dynamic_cast<VariableValue*>(function);
		if (vv && vv->var && vv->var->value) {
			if (auto f = dynamic_cast<Function*>(vv->var->value)) {
				function_object = f;
			}
		}
	}

	// Standard library constructors TODO better
	auto vv = dynamic_cast<VariableValue*>(function);
	if (vv != nullptr) {
		if (vv->name == "Number") {
			if (arguments.size()) {
				type = arguments.at(0)->type;
			} else {
				type = Type::integer();
			}
			return;
		}
		else if (vv->name == "Boolean") {
			type = Type::boolean();
			return;
		} else if (vv->name == "String") {
			type = Type::string();
			return;
		} else if (vv->name == "Array") {
			type = Type::array();
			return;
		} else if (vv->name == "Object") {
			type = Type::object();
			return;
		} else if (vv->name == "Set") {
			type = Type::set(Type::any());
			return;
		} else {
			type = Type::any(); // Class constructor
		}
	} else if (function->type == Type::clazz()) {
		type = Type::any(); // Class constructor
	}

	// Detect standard library functions
	auto oa = dynamic_cast<ObjectAccess*>(function);
	if (oa != nullptr) {

		auto field_name = oa->field->content;
		auto object_type = oa->object->type;

		std::vector<Type> arg_types;
		for (auto arg : arguments) {
			arg_types.push_back(arg->type.fold());
		}
		std::string args_string = "";
		for (unsigned i = 0; i < arg_types.size(); ++i) {
			std::ostringstream oss;
			oss << arg_types[i];
			if (i > 0) args_string += ", ";
			args_string += oss.str();
		}

		if (object_type.is_class()) { // String.size("salut")

			std::string clazz = ((VariableValue*) oa->object)->name;
			auto object_class = (LSClass*) analyser->vm->internal_vars[clazz]->lsvalue;
			auto sm = object_class->getMethod(analyser, oa->field->content, arg_types);

			if (sm != nullptr) {
				std_func = sm->addr;
				function->type = sm->type;
				is_native_method = sm->native;
				function_name = object_class->name + std::string("::") + oa->field->content;
				// Apply mutators
				for (const auto& mutator : sm->mutators) {
					mutator->apply(analyser, arguments);
				}
				// std::cout << "Static method " << oa->field->content << " found : " << sm->type << std::endl;
			} else {
				auto value_class = (LSClass*) analyser->vm->internal_vars["Value"]->lsvalue;
				std::vector<Type> args = arg_types;
				args.insert(args.begin(), object_type);
				auto m = value_class->getMethod(analyser, oa->field->content, args);

				if (m != nullptr) {
					this_ptr = oa->object;
					this_ptr->analyse(analyser);
					std_func = m->addr;
					function->type = m->type;
					is_native_method = m->native;
					function_name = std::string("Value::") + oa->field->content;
					// Apply mutators
					for (const auto& mutator : m->mutators) {
						mutator->apply(analyser, arguments);
					}
				} else {
					analyser->add_error({SemanticError::Type::STATIC_METHOD_NOT_FOUND, location(), oa->field->location, {clazz + "::" + oa->field->content + "(" + args_string + ")"}});
				}
			}
		} else {  // "salut".size()

			Method* m = nullptr;
			LSClass* clazz;
			if (!object_type.fold().is_any() && !object_type.fold().is_placeholder()) {
				clazz = (LSClass*) analyser->vm->internal_vars[object_type.class_name()]->lsvalue;
				std::vector<Type> args = arg_types;
				args.insert(args.begin(), object_type);
				m = clazz->getMethod(analyser, oa->field->content, args);
			}
			if (m == nullptr) {
				clazz = (LSClass*) analyser->vm->internal_vars["Value"]->lsvalue;
				std::vector<Type> args = arg_types;
				args.insert(args.begin(), object_type);
				m = clazz->getMethod(analyser, oa->field->content, args);
			}
			if (m != nullptr) {
				// std::cout << "Method " << oa->field->content << " found : " << m->type << std::endl;
				this_ptr = oa->object;
				this_ptr->analyse(analyser);
				std_func = m->addr;
				function->type = m->type;
				is_native_method = m->native;
				function_name = clazz->name + std::string("::") + oa->field->content;
				// Apply mutators
				std::vector<Value*> values = {oa->object};
				values.insert(values.end(), arguments.begin(), arguments.end());
				for (const auto& mutator : m->mutators) {
					mutator->apply(analyser, values);
				}
			} else {
				bool has_unknown_argument = false;
				// for (const auto& a : arguments)
					// if (a->type == ANY) has_unknown_argument = true;
				if (!object_type.fold().is_any() && !has_unknown_argument) {
					std::ostringstream obj_type_ss;
					obj_type_ss << object_type;
					analyser->add_error({SemanticError::Type::METHOD_NOT_FOUND, location(), oa->field->location, {obj_type_ss.str() + "." + oa->field->content + "(" + args_string + ")"}});
				} else {
					is_unknown_method = true;
					object = oa->object;
				}
			}
		}
	}

	// Check arguments count
	arg_types.clear();
	auto arguments_count = arguments.size();
	if (this_ptr != nullptr) arguments_count++;
	bool arguments_valid = arguments_count <= function->type.arguments().size();
	auto total_arguments_passed = std::max(arguments.size(), function->type.arguments().size());
	int offset = this_ptr != nullptr ? 1 : 0;
	size_t a = 0;
	for (auto& argument_type : function->type.arguments()) {
		if (a == 0 and this_ptr != nullptr) {
			// OK it's the object for method call
		} else if (a < arguments_count) {
			// OK, the argument is present in the call
			arguments.at(a - offset)->type.reference = argument_type.reference;
			if (argument_type.is_function()) {
				arguments.at(a - offset)->will_take(analyser, argument_type.arguments(), 1);
				arguments.at(a - offset)->set_version(argument_type.arguments(), 1);
				arguments.at(a - offset)->must_return(analyser, argument_type.return_type());
			}
			arg_types.push_back(arguments.at(a - offset)->type);
		} else if (function_object && function_object->defaultValues.at(a - offset) != nullptr) {
			// OK, there's no argument in the call but a default value is set.
			arg_types.push_back(function_object->defaultValues.at(a - offset)->type);
		} else {
			// Missing argument
			arguments_valid = false;
			total_arguments_passed--;
		}
		a++;
	}

	if (function->type.is_function() and !arguments_valid) {
		analyser->add_error({SemanticError::Type::WRONG_ARGUMENT_COUNT,	location(), location(), {
			function->to_string(),
			std::to_string(function->type.arguments().size()),
			std::to_string(total_arguments_passed)
		}});
		return;
	}
	if (arguments_valid) {
		function->will_take(analyser, arg_types, 1);
		function->set_version(arg_types, 1);
	}
	if (is_unknown_method) {
		for (const auto& arg : arguments) {
			if (arg->type.is_function()) {
				arg->must_return(analyser, Type::any());
			}
		}
	}

	// Get the function type
	function_type = function->version_type(arg_types);

	// Recursive function
	if (vv and vv->var and vv->var->value and vv->var->name == analyser->current_function()->name) {
		type = analyser->current_function()->getReturnType();
	} else {
		type = function_type.return_type();
	}
	return_type = function_type.return_type();
	// std::cout << "FC " << this << " type " << type << std::endl;
}

bool FunctionCall::will_take(SemanticAnalyser* analyser, const std::vector<Type>& args, int level) {
	// std::cout << "FC " << this << " will_take " << args << std::endl;
	function->will_take(analyser, args, level + 1);
	return false;
}

void FunctionCall::set_version(const std::vector<Type>& args, int level) {
	function->set_version(args, level + 1);
}

Type FunctionCall::version_type(std::vector<Type> version) const {
	return function->version_type(version).return_type();
}

Compiler::value FunctionCall::compile(Compiler& c) const {

	// std::cout << "FunctionCall::compile(" << function_type << ")" << std::endl;
	if (callable) {
		std::vector<Compiler::value> args;
		for (unsigned i = 0; i < arguments.size(); ++i) {
			args.push_back(arguments.at(i)->compile(c));
			arguments.at(i)->compile_end(c);
		}
		return callable_version->compile_call(c, args);
	}

	/** Standard library constructors : Array(), Number() */
	VariableValue* vv = dynamic_cast<VariableValue*>(function);
	if (vv != nullptr) {
		if (vv->name == "Boolean") {
			return c.new_bool(false);
		}
		if (vv->name == "Number") {
			if (arguments.size() > 0) {
				return arguments.at(0)->compile(c);
			} else {
				return c.new_integer(0);
			}
		}
		if (vv->name == "String") {
			if (arguments.size() > 0) {
				return arguments[0]->compile(c);
			}
			return c.new_pointer(new LSString(""), type);
		}
		if (vv->name == "Array") {
			return c.new_array({}, {});
		}
		if (vv->name == "Object") {
			return c.new_pointer(new LSObject(), type);
		}
		if (vv->name == "Set") {
			return c.new_pointer(new LSSet<LSValue*>(), type);
		}
	}
	if (function->type == Type::clazz()) {
		auto clazz = function->compile(c);
		return c.new_object_class(clazz);
	}

	/** Operator functions : +(1, 2) */
	VariableValue* f = dynamic_cast<VariableValue*>(function);
	if (f != nullptr) {
		if (function->type.argument(0).is_primitive() and function->type.argument(1).is_primitive()) {
			if (f->name == "+") {
				return c.insn_add(arguments[0]->compile(c), arguments[1]->compile(c));
			} else if (f->name == "-") {
				return c.insn_sub(arguments[0]->compile(c), arguments[1]->compile(c));
			} else if (f->name == "*" or f->name == "×") {
				return c.insn_mul(arguments[0]->compile(c), arguments[1]->compile(c));
			} else if (f->name == "/" or f->name == "÷") {
				return c.insn_div(arguments[0]->compile(c), arguments[1]->compile(c));
			} else if (f->name == "**") {
				return c.insn_pow(arguments[0]->compile(c), arguments[1]->compile(c));
			} else if (f->name == "%") {
				return c.insn_mod(arguments[0]->compile(c), arguments[1]->compile(c));
			}
		}
	}

	/** Default function : f(12) */
	Compiler::value fun;
	auto jit_object = c.new_pointer(nullptr, Type::any());
	auto is_closure = function->type.is_closure();

	if (is_unknown_method) {
		auto oa = static_cast<ObjectAccess*>(function);
		jit_object = c.insn_load(((LeftValue*) object)->compile_l(c));
		auto k = c.new_pointer(&oa->field->content, Type::any());
		fun = c.insn_call(Type::fun(), {jit_object, k}, (void*) +[](LSValue* object, std::string* key) {
			return object->attr(*key);
		});
	} else {
		fun = function->compile(c);
	}

	/** Arguments */
	size_t offset = is_closure or is_unknown_method ? 1 : 0;
	size_t arg_count = std::max(arg_types.size(), arguments.size()) + offset;
	std::vector<Compiler::value> args;
	std::vector<LSValueType> lsvalue_types;

	if (is_unknown_method) {
		// add 'function' object as first argument
		args.push_back(fun);
		lsvalue_types.push_back(Type::fun().id());
		// add 'this' object as second argument
		args.push_back(jit_object);
		lsvalue_types.push_back(object->type.id());
	} else if (is_closure) {
		// Function pointer as first argument
		args.push_back(fun);
		lsvalue_types.push_back(Type::closure().id());
	}

	// Compile arguments
	// std::cout << this << " args " << args.size() << " " << arg_count << " " << offset << std::endl;
	for (size_t i = 0; i < arg_count - offset; ++i) {
		if (i < arguments.size()) {
			auto arg = arguments.at(i)->compile(c);
			c.assert_value_ok(arg);
			if (arg.t.is_primitive()) arg = c.insn_convert(arg, function_type.argument(i));
			args.push_back(arg);
			arguments.at(i)->compile_end(c);
			if (function_type.argument(i) == Type::mpz() && arguments.at(i)->type != Type::tmp_mpz()) {
				args.at(offset + i) = c.insn_clone_mpz(args.at(offset + i));
			}
		} else {
			args.push_back(function_object->defaultValues.at(i)->compile(c));
		}
		lsvalue_types.push_back(function_type.argument(i).id());

		// Increment references of argument
		c.insn_inc_refs(args.at(offset + i));
	}

	c.mark_offset(location().start.line);

	// TODO : some tests are failing with this
	// c.insn_check_args(args, lsvalue_types);

	// Call
	// std::cout << "Function call args: " << function << " | ";
	// for (auto& a : args) { std::cout << a.t << " " << a.v->getType() << ", "; }
	// std::cout << std::endl;

	Compiler::value result;
	if (is_unknown_method) {
		result = c.insn_call(Type::any(), args, (void*) &LSFunction::call);
	} else {
		result = c.insn_invoke(return_type, args, fun);
	}

	// Destroy temporary arguments
	for (size_t i = 0; i < arg_count - offset; ++i) {
		c.insn_delete(args.at(offset + i));
	}
	c.insn_delete_temporary(fun);

	if (is_unknown_method) {
		object->compile_end(c);
	} else {
		function->compile_end(c);
	}

	// Custom function call : 1 op
	c.inc_ops(1);

	return result;
}

Value* FunctionCall::clone() const {
	auto fc = new FunctionCall(token);
	fc->function = function->clone();
	for (const auto& a : arguments) {
		fc->arguments.push_back(a->clone());
	}
	fc->closing_parenthesis = closing_parenthesis;
	return fc;
}

}
