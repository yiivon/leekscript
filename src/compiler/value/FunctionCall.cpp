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

	auto parenthesis = condensed && dynamic_cast<const Function*>(function);
	if (parenthesis) os << "(";
	function->print(os, indent, debug, condensed);
	if (parenthesis) os << ")";

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

	// std::cout << "FC analyse " << std::endl;

	// Analyse the function (can be anything here)
	function->analyse(analyser);

	// Analyse arguments
	for (const auto& argument : arguments) {
		argument->analyse(analyser);
	}

	// Perform a will_take to prepare eventual versions
	std::vector<Type> arguments_types;
	for (const auto& argument : arguments) {
		arguments_types.push_back(argument->type);
	}
	function->will_take(analyser, arguments_types, 1);
	function->set_version(arguments_types, 1);

	// std::cout << "FC function " << function->version_type(arguments_types) << std::endl;

	// Retrieve the callable version
	callable = function->get_callable(analyser);
	if (/*callable == nullptr or */not function->type.can_be_callable()) {
		analyser->add_error({SemanticError::Type::CANNOT_CALL_VALUE, location(), function->location(), {function->to_string()}});
	}
	if (callable) {
		// std::cout << "Callable: " << callable << std::endl;
		callable_version = callable->resolve(analyser, arguments_types);
		if (callable_version) {
			// std::cout << "Version: " << callable_version << std::endl;
			type = callable_version->type.return_type();
			callable_version->apply_mutators(analyser, arguments);
			
			int offset = callable_version->object ? 1 : 0;
			for (size_t a = 0; a < arguments.size(); ++a) {
				auto argument_type = callable_version->type.argument(a + offset);
				if (argument_type.is_function()) {
					arguments.at(a)->will_take(analyser, argument_type.arguments(), 1);
					arguments.at(a)->set_version(argument_type.arguments(), 1);
					arguments.at(a)->must_return(analyser, argument_type.return_type());
				}
			}
			if (callable_version->value) {
				function->will_take(analyser, arguments_types, 1);
				function->set_version(arguments_types, 1);
				function_type = function->version_type(arguments_types);
				auto vv = dynamic_cast<VariableValue*>(function);
				if (vv and vv->var and vv->var->value and vv->var->name == analyser->current_function()->name) {
					type = analyser->current_function()->getReturnType();
				} else {
					type = function_type.return_type();
				}
			}
			if (callable_version->unknown) {
				for (const auto& arg : arguments) {
					if (arg->type.is_function()) {
						arg->must_return(analyser, Type::any());
					}
				}
			}
			return;
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
			analyser->add_error({SemanticError::Type::STATIC_METHOD_NOT_FOUND, location(), oa->field->location, {clazz + "::" + oa->field->content + "(" + args_string + ")"}});
		} else {  // "salut".size()
			bool has_unknown_argument = false;
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
