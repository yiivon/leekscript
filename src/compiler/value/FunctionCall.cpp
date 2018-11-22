#include "FunctionCall.hpp"
#include <jit/jit-common.h>
#include <jit/jit-insn.h>
#include <jit/jit-type.h>
#include <jit/jit-value.h>
#include <sstream>
#include <string>
#include "../../vm/Module.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Type.hpp"
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

using namespace std;

namespace ls {

FunctionCall::FunctionCall(std::shared_ptr<Token> t) : token(t) {
	function = nullptr;
	type = Type::UNKNOWN;
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

void FunctionCall::print(std::ostream& os, int indent, bool debug) const {

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
		os << " " << types;
	}
}

Location FunctionCall::location() const {
	return {function->location().start, closing_parenthesis->location.end};
}

void FunctionCall::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	// std::cout << "FC " << this << " : " << req_type << std::endl;

	// Analyse the function (can be anything here)
	function->analyse(analyser, Type::UNKNOWN);

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
	// TODO add a is_callable() on Type
	if (function->type.raw_type != RawType::UNKNOWN and
		function->type.raw_type != RawType::FUNCTION and
		function->type.raw_type != RawType::CLOSURE and
		function->type.raw_type != RawType::CLASS) {
		analyser->add_error({SemanticError::Type::CANNOT_CALL_VALUE, location(), function->location(), {function->to_string()}});
	}

	// Analyse all arguments a first time
	for (size_t a = 0; a < arguments.size(); ++a) {
		auto arg = arguments.at(a);
		arg->analyse(analyser, Type::UNKNOWN);
		arguments.at(a)->type = arg->type;
		arguments.at(a)->type.reference = function->type.getArgumentType(a).reference;
	}

	// Standard library constructors TODO better
	auto vv = dynamic_cast<VariableValue*>(function);
	if (vv != nullptr) {
		if (vv->name == "Number") {
			if (arguments.size()) {
				function->type.setArgumentType(0, arguments.at(0)->type);
				type = arguments.at(0)->type;
			} else {
				type = Type::INTEGER;
			}
		}
		else if (vv->name == "Boolean") type = Type::BOOLEAN;
		else if (vv->name == "String") type = Type::STRING;
		else if (vv->name == "Array") type = Type::PTR_ARRAY;
		else if (vv->name == "Object") type = Type::OBJECT;
		else if (vv->name == "Set") type = Type::PTR_SET;
		else {
			type = Type::POINTER; // Class constructor
		}
	} else if (function->type == Type::CLASS) {
		type = Type::POINTER; // Class constructor
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

		if (object_type.raw_type == RawType::CLASS) { // String.size("salut")

			string clazz = ((VariableValue*) oa->object)->name;
			auto object_class = (LSClass*) analyser->vm->system_vars[clazz];
			auto sm = object_class->getStaticMethod(oa->field->content, arg_types);

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
				auto m = value_class->getMethod(oa->field->content, object_type, arg_types);

				if (m != nullptr) {
					this_ptr = oa->object;
					this_ptr->analyse(analyser, m->obj_type);
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
			if (object_type.raw_type != RawType::UNKNOWN) {
				clazz = (LSClass*) analyser->vm->system_vars[object_type.clazz];
				m = clazz->getMethod(oa->field->content, object_type, arg_types);
				// std::cout << "Method " << oa->field->content << " found : " << m->type << std::endl;
			}
			if (m == nullptr) {
				clazz = (LSClass*) analyser->vm->system_vars["Value"];
				m = clazz->getMethod(oa->field->content, object_type, arg_types);
			}
			if (m != nullptr) {
				this_ptr = oa->object;
				this_ptr->analyse(analyser, m->obj_type);
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
				for (const auto& a : arguments)
					if (a->type.nature == Nature::UNKNOWN) has_unknown_argument = true;
				if (object_type.raw_type != RawType::UNKNOWN && !has_unknown_argument) {
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

	// Operator function? TODO better, no special behavior
	vv = dynamic_cast<VariableValue*>(function);
	if (vv != nullptr) {
		auto name = vv->name;
		if (name == "+" or name == "-" or name == "*" or name == "/" or name == "**" or name == "%") {
			bool isByValue = true;
			Type effectiveType;
			for (auto& arg : arguments) {
				arg->analyse(analyser, Type::UNKNOWN);
				effectiveType = arg->type;
				if (arg->type.nature != Nature::VALUE) {
					isByValue = false;
				}
			}
			if (isByValue) {
				function->type.setArgumentType(0, effectiveType);
				function->type.setArgumentType(1, effectiveType);
				function->type.setReturnType(effectiveType);
			}
			type = function->type.getReturnType();
		}
	}

	// Check arguments count
	arg_types.clear();
	bool arguments_valid = arguments.size() <= function->type.getArgumentTypes().size();
	auto total_arguments_passed = std::max(arguments.size(), function->type.getArgumentTypes().size());
	size_t a = 0;
	for (auto& argument_type : function->type.getArgumentTypes()) {
		if (a < arguments.size()) {
			auto arg_type = function->type.getArgumentType(a);
			// OK, the argument is present in the call
			arguments.at(a)->analyse(analyser, argument_type);
			arguments.at(a)->type.reference = arg_type.reference;
			if (arg_type.raw_type == RawType::FUNCTION or arg_type.raw_type == RawType::CLOSURE) {
				arguments.at(a)->will_take(analyser, arg_type.arguments_types, 1);
				arguments.at(a)->set_version(arg_type.arguments_types, 1);
				arguments.at(a)->must_return(analyser, arg_type.getReturnType());
			}
			arg_types.push_back(arguments.at(a)->type);
		} else if (function->type.argumentHasDefault(a)) {
			// OK, there's no argument in the call but a default value is set.
			if (function_object) {
				arg_types.push_back(function_object->defaultValues.at(a)->type);
			}
		} else {
			// Missing argument
			arguments_valid = false;
			total_arguments_passed--;
		}
		a++;
	}

	if ((function->type.raw_type == RawType::FUNCTION or function->type.raw_type == RawType::CLOSURE) and !arguments_valid) {
		analyser->add_error({SemanticError::Type::WRONG_ARGUMENT_COUNT,	location(), location(), {
			function->to_string(),
			std::to_string(function->type.getArgumentTypes().size()),
			std::to_string(total_arguments_passed)
		}});
		type = Type::UNKNOWN;
		return;
	}

	if (arguments_valid) {
		function->will_take(analyser, arg_types, 1);
		function->set_version(arg_types, 1);
	}

	// Get the function type
	function_type = function->version_type(arg_types);

	// std::cout << "FC function type: " << function_type << std::endl;

	// The function is a variable
	if (vv and vv->var and vv->var->value) {
		// Recursive function
		if (vv->var->name == analyser->current_function()->name) {
			type = analyser->current_function()->getReturnType();
		} else {
			auto ret_type = function_type.getReturnType();
			if (ret_type != Type::UNKNOWN) {
				type = ret_type;
			}
		}
	} else {
		auto ret_type = function_type.getReturnType();
		if (is_unknown_method && ret_type == Type::UNKNOWN && function_type != Type::UNKNOWN) {
			type = Type::POINTER;
		}
		if (ret_type.nature != Nature::UNKNOWN) {
			type = ret_type;
		}
	}

	a = 0;
	for (auto& arg : arguments) {
		auto t = function_type.getArgumentType(a);
		if (t == Type::UNKNOWN) {
			t = Type::POINTER;
		}
		arg->analyse(analyser, t);
		arg->type.reference = function->type.getArgumentType(a).reference;
		a++;
	}

	return_type = function_type.getReturnType();

	if (req_type.nature == Nature::POINTER) {
		type.nature = req_type.nature;
	}
	types = type;

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
	return function->version_type(version).getReturnType();
}

Compiler::value FunctionCall::compile(Compiler& c) const {

	// std::cout << "FunctionCall::compile(" << function_type << ")" << std::endl;

	/** Standard library constructors : Array(), Number() */
	VariableValue* vv = dynamic_cast<VariableValue*>(function);
	if (vv != nullptr) {
		if (vv->name == "Boolean") {
			auto b = c.new_bool(false);
			if (type.nature == Nature::POINTER) {
				return c.insn_to_pointer(b);
			}
			return b;
		}
		if (vv->name == "Number") {
			auto n = [&]() {
				if (arguments.size() > 0) {
					return arguments.at(0)->compile(c);
				} else {
					return c.new_integer(0);
				}
			}();
			if (type.nature == Nature::POINTER) {
				return c.insn_to_pointer(n);
			}
			return n;
		}
		if (vv->name == "String") {
			if (arguments.size() > 0) {
				return arguments[0]->compile(c);
			}
			return {c.new_pointer(new LSString("")).v, type};
		}
		if (vv->name == "Array") {
			return {c.new_pointer(new LSArray<LSValue*>()).v, type};
		}
		if (vv->name == "Object") {
			return {c.new_pointer(new LSObject()).v, type};
		}
		if (vv->name == "Set") {
			return {c.new_pointer(new LSSet<LSValue*>()).v, type};
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
		vector<LSValueType> lsvalue_types = { (LSValueType) this_ptr->type.id() };
		for (unsigned i = 0; i < arguments.size(); ++i) {
			args.push_back(arguments.at(i)->compile(c));
			arguments.at(i)->compile_end(c);
			lsvalue_types.push_back(function->type.getArgumentType(i).id());
		}
		c.insn_check_args(args, lsvalue_types);

		Compiler::value res;
		if (is_native_method) {
			auto fun = (void*) std_func;
			res = c.insn_call(return_type, args, fun, function_name);
		} else {
			auto fun = (Compiler::value (*)(Compiler&, vector<Compiler::value>)) std_func;
			res = fun(c, args);
		}

		if (return_type.nature == Nature::VALUE and type.nature == Nature::POINTER) {
			return c.insn_to_pointer(res);
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
			lsvalue_types.push_back((LSValueType) function->type.getArgumentType(i).id());
		}
		c.insn_check_args(args, lsvalue_types);

		Compiler::value res;
		if (is_native_method) {
			auto fun = (void*) std_func;
			res = c.insn_call(return_type, args, fun, function_name);
		} else {
			auto fun = (Compiler::value (*)(Compiler&, vector<Compiler::value>)) std_func;
			res = fun(c, args);
		}

		if (return_type.nature == Nature::VALUE and type.nature == Nature::POINTER) {
			return c.insn_to_pointer(res);
		}
		return res;
	}

	/** Operator functions : +(1, 2) */
	VariableValue* f = dynamic_cast<VariableValue*>(function);

	if (f != nullptr) {
		if (function->type.getArgumentType(0).nature == Nature::VALUE
			and function->type.getArgumentType(1).nature == Nature::VALUE) {

			jit_value_t (*jit_func)(jit_function_t, jit_value_t, jit_value_t) = nullptr;
			if (f->name == "+") {
				jit_func = &jit_insn_add;
			} else if (f->name == "-") {
				jit_func = &jit_insn_sub;
			} else if (f->name == "*" or f->name == "×") {
				jit_func = &jit_insn_mul;
			} else if (f->name == "/" or f->name == "÷") {
				jit_func = &jit_insn_div;
			} else if (f->name == "**") {
				jit_func = &jit_insn_pow;
			} else if (f->name == "%") {
				jit_func = &jit_insn_rem;
			}
			if (jit_func != nullptr) {
				auto v0 = arguments[0]->compile(c);
				auto v1 = arguments[1]->compile(c);
				// jit_value_t ret = jit_func(c.F, v0.v, v1.v);
                //
				// if (type.nature == Nature::POINTER) {
				// 	return c.insn_to_pointer({ret, Type::INTEGER});
				// }
				// return {ret, type};
			}
		}
	}

	/** Default function : f(12) */
	Compiler::value fun;
	auto ls_fun_addr = c.new_pointer(nullptr);
	auto jit_object = c.new_pointer(nullptr);
	auto is_closure = function->type.raw_type == RawType::CLOSURE;
	// std::cout << "function call " << function << " " << function->type << " closure " << is_closure << std::endl;

	if (is_unknown_method) {
		auto oa = static_cast<ObjectAccess*>(function);
		jit_object = c.insn_load(((LeftValue*) object)->compile_l(c));
		auto k = c.new_pointer(&oa->field->content);
		ls_fun_addr = c.insn_call(Type::FUNCTION, {jit_object, k}, (void*) +[](LSValue* object, std::string* key) {
			return object->attr(*key);
		});
	} else {
		ls_fun_addr = function->compile(c);
		// c.insn_call(Type::VOID, {ls_fun_addr}, +[](LSFunction* fun) {
		// 	std::cout << "call fun: " << fun->function << std::endl;
		// });
		// std::cout << "ls_fun_addr " << this << " " << ls_fun_addr.t << " " << ls_fun_addr.v->getType() << std::endl;
		auto fun_to_ptr = LLVMCompiler::Builder.CreatePointerCast(ls_fun_addr.v, Type::LLVM_FUNCTION_TYPE_PTR);
		fun = {LLVMCompiler::Builder.CreateStructGEP(Type::LLVM_FUNCTION_TYPE, fun_to_ptr, 5), Type::POINTER};
		// std::cout << "load fun " << fun.t << std::endl;
		fun = c.insn_load(fun);
		// fun = c.insn_load(ls_fun_addr, 24, Type::POINTER);
		// c.insn_call(Type::VOID, {fun}, +[](void* fun) {
		// 	std::cout << "call fun loaded: " << fun << std::endl;
		// });
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
			args.push_back(arguments.at(i)->compile(c));
			arguments.at(i)->compile_end(c);
			if (function_type.getArgumentType(i) == Type::MPZ && arguments.at(i)->type != Type::MPZ_TMP) {
				args.at(offset + i) = c.insn_clone_mpz(args.at(offset + i));
			}
		} else {
			args.push_back(function_object->defaultValues.at(i)->compile(c));
		}
		lsvalue_types.push_back(function_type.getArgumentType(i).id());

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
		// std::cout << "is unknown method" << std::endl;
		result = c.insn_call(Type::POINTER, args, (void*) &LSFunction::call);
	} else {
		// std::cout << "known method" << std::endl;
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

	if (return_type.nature == Nature::VALUE and type.nature == Nature::POINTER) {
		return c.insn_to_pointer(result);
	}
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
