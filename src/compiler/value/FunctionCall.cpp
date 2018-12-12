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
#include "../../vm/VM.hpp"
#include "../Compiler.hpp"
#include "../lexical/Token.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticError.hpp"
#include "ObjectAccess.hpp"
#include "VariableValue.hpp"
#include "../../type/RawType.hpp"

using namespace std;

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

	function->print(os, indent, debug);
	os << "(";
	for (unsigned i = 0; i < arguments.size(); ++i) {
		arguments.at(i)->print(os, indent, debug);
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

	// The function call be called?
	if (not function->type.is_callable()) {
		analyser->add_error({SemanticError::Type::CANNOT_CALL_VALUE, location(), function->location(), {function->to_string()}});
	}

	// Analyse arguments
	for (size_t a = 0; a < arguments.size(); ++a) {
		auto arg = arguments.at(a);
		arg->analyse(analyser);
		arguments.at(a)->type = arg->type;
	}

	// Standard library constructors TODO better
	auto vv = dynamic_cast<VariableValue*>(function);
	if (vv != nullptr) {
		if (vv->name == "Number") {
			if (arguments.size()) {
				type = arguments.at(0)->type;
			} else {
				type = Type::INTEGER;
			}
			return;
		}
		else if (vv->name == "Boolean") {
			type = Type::BOOLEAN;
			return;
		} else if (vv->name == "String") {
			type = Type::STRING;
			return;
		} else if (vv->name == "Array") {
			type = Type::PTR_ARRAY;
			return;
		} else if (vv->name == "Object") {
			type = Type::OBJECT;
			return;
		} else if (vv->name == "Set") {
			type = Type::PTR_SET;
			return;
		} else {
			type = Type::ANY; // Class constructor
		}
	} else if (function->type == Type::CLASS) {
		type = Type::ANY; // Class constructor
	}

	// Detect standard library functions
	auto oa = dynamic_cast<ObjectAccess*>(function);
	if (oa != nullptr) {

		auto field_name = oa->field->content;
		auto object_type = oa->object->type;

		vector<Type> arg_types;
		for (auto arg : arguments) {
			arg_types.push_back(arg->type);
		}
		std::string args_string = "";
		for (unsigned i = 0; i < arg_types.size(); ++i) {
			std::ostringstream oss;
			oss << arg_types[i];
			if (i > 0) args_string += ", ";
			args_string += oss.str();
		}

		if (object_type.is_class()) { // String.size("salut")

			string clazz = ((VariableValue*) oa->object)->name;
			auto object_class = (LSClass*) analyser->vm->system_vars[clazz];
			auto sm = object_class->getStaticMethod(analyser, oa->field->content, arg_types);

			if (sm != nullptr) {
				std_func = sm->addr;
				function->type = sm->type;
				is_native_method = sm->native;
				function_name = object_class->name + std::string("::") + oa->field->content;
				// Apply mutators
				for (const auto& mutator : sm->mutators) {
					mutator->apply(analyser, arguments);
				}
			} else {
				auto value_class = (LSClass*) analyser->vm->system_vars["Value"];
				auto m = value_class->getMethod(analyser, oa->field->content, object_type, arg_types);

				if (m != nullptr) {
					this_ptr = oa->object;
					this_ptr->analyse(analyser);
					this_ptr_type = m->obj_type;
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
			if (!object_type.is_any() && !object_type.is_placeholder()) {
				clazz = (LSClass*) analyser->vm->system_vars[object_type.clazz()];
				m = clazz->getMethod(analyser, oa->field->content, object_type, arg_types);
				// std::cout << "Method " << oa->field->content << " found : " << m->obj_type << "." << m->type << std::endl;
			}
			if (m == nullptr) {
				clazz = (LSClass*) analyser->vm->system_vars["Value"];
				m = clazz->getMethod(analyser, oa->field->content, object_type, arg_types);
			}
			if (m != nullptr) {
				this_ptr = oa->object;
				this_ptr->analyse(analyser);
				this_ptr_type = m->obj_type;
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
				if (!object_type.is_any() && !has_unknown_argument) {
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
	bool arguments_valid = arguments.size() <= function->type.arguments().size();
	auto total_arguments_passed = std::max(arguments.size(), function->type.arguments().size());
	size_t a = 0;
	for (auto& argument_type : function->type.arguments()) {
		if (a < arguments.size()) {
			// OK, the argument is present in the call
			arguments.at(a)->type.reference = argument_type.reference;
			if (argument_type.is_function()) {
				arguments.at(a)->will_take(analyser, argument_type.arguments(), 1);
				arguments.at(a)->set_version(argument_type.arguments(), 1);
				arguments.at(a)->must_return(analyser, argument_type.return_type());
			}
			arg_types.push_back(arguments.at(a)->type);
		} else if (function_object && function_object->defaultValues.at(a) != nullptr) {
			// OK, there's no argument in the call but a default value is set.
			arg_types.push_back(function_object->defaultValues.at(a)->type);
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
			return c.new_array(Type::PTR_ARRAY, {});
		}
		if (vv->name == "Object") {
			return c.new_pointer(new LSObject(), type);
		}
		if (vv->name == "Set") {
			return c.new_pointer(new LSSet<LSValue*>(), type);
		}
	}
	if (function->type == Type::CLASS) {
		auto clazz = function->compile(c);
		return c.new_object_class(clazz);
	}

	/** Standard function call on object : "hello".size() */
	if (this_ptr != nullptr) {
		auto obj = this_ptr->compile(c);
		if (obj.t.reference) {
			obj = c.insn_load(obj);
		}
		vector<Compiler::value> args = { obj };
		this_ptr->compile_end(c);
		vector<LSValueType> lsvalue_types = { (LSValueType) this_ptr_type.id() };
		for (unsigned i = 0; i < arguments.size(); ++i) {
			args.push_back(arguments.at(i)->compile(c));
			arguments.at(i)->compile_end(c);
			lsvalue_types.push_back(function->type.argument(i).id());
		}
		c.insn_check_args(args, lsvalue_types);

		Compiler::value res;
		if (is_native_method) {
			auto fun = (void*) std_func;
			res = c.insn_call(return_type, args, fun);
		} else {
			auto fun = (Compiler::value (*)(Compiler&, vector<Compiler::value>)) std_func;
			res = fun(c, args);
		}
		return res;
	}

	/** Static standard function call : Number.char(65) */
	if (std_func != nullptr) {
		vector<Compiler::value> args;
		vector<LSValueType> lsvalue_types;
		for (unsigned i = 0; i < arguments.size(); ++i) {
			args.push_back(arguments.at(i)->compile(c));
			arguments.at(i)->compile_end(c);
			lsvalue_types.push_back((LSValueType) function->type.argument(i).id());
		}
		c.insn_check_args(args, lsvalue_types);
		Compiler::value res;
		if (is_native_method) {
			auto fun = (void*) std_func;
			res = c.insn_call(return_type, args, fun);
		} else {
			auto fun = (Compiler::value (*)(Compiler&, vector<Compiler::value>)) std_func;
			res = fun(c, args);
		}
		return res;
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
	auto ls_fun_addr = c.new_function(nullptr, function->type);
	auto jit_object = c.new_pointer(nullptr, Type::ANY);
	auto is_closure = function->type.is_closure();

	if (is_unknown_method) {
		auto oa = static_cast<ObjectAccess*>(function);
		jit_object = c.insn_load(((LeftValue*) object)->compile_l(c));
		auto k = c.new_pointer(&oa->field->content, Type::ANY);
		ls_fun_addr = c.insn_call(Type::FUNCTION, {jit_object, k}, (void*) +[](LSValue* object, std::string* key) {
			return object->attr(*key);
		});
	} else {
		ls_fun_addr = function->compile(c);
		auto fun_to_ptr = Compiler::builder.CreatePointerCast(ls_fun_addr.v, Function_type::get_function_type());
		auto f = Compiler::builder.CreateStructGEP(Function_type::get_function_type()->getPointerElementType(), fun_to_ptr, 5);
		fun = { Compiler::builder.CreateLoad(f), Type::FUNCTION };
	}

	/** Arguments */
	size_t offset = is_closure or is_unknown_method ? 1 : 0;
	size_t arg_count = std::max(arg_types.size(), arguments.size()) + offset;
	vector<Compiler::value> args;
	vector<LSValueType> lsvalue_types;

	if (is_unknown_method) {
		// add 'function' object as first argument
		args.push_back(ls_fun_addr);
		lsvalue_types.push_back(Type::FUNCTION.id());
		// add 'this' object as second argument
		args.push_back(jit_object);
		lsvalue_types.push_back(object->type.id());
	} else if (is_closure) {
		// Function pointer as first argument
		args.push_back(ls_fun_addr);
		lsvalue_types.push_back(Type::CLOSURE.id());
	}

	// Compile arguments
	// std::cout << this << " args " << args.size() << " " << arg_count << " " << offset << std::endl;
	for (size_t i = 0; i < arg_count - offset; ++i) {
		if (i < arguments.size()) {
			auto arg = arguments.at(i)->compile(c);
			if (arg.t.is_primitive()) arg = c.insn_convert(arg, function_type.argument(i));
			args.push_back(arg);
			arguments.at(i)->compile_end(c);
			if (function_type.argument(i) == Type::MPZ && arguments.at(i)->type != Type::MPZ_TMP) {
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
		result = c.insn_call(Type::ANY, args, (void*) &LSFunction::call);
	} else {
		result = c.insn_call_indirect(return_type, fun, args);
	}

	// Destroy temporary arguments
	for (size_t i = 0; i < arg_count - offset; ++i) {
		c.insn_delete(args.at(offset + i));
	}
	c.insn_delete_temporary(ls_fun_addr);

	if (is_unknown_method) {
		object->compile_end(c);
	} else {
		function->compile_end(c);
	}

	// Custom function call : 1 op
	c.inc_ops(1);
	
	if (type._types.size() == 0) {
		return {nullptr, {}};
	} else {
		return result;
	}
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
