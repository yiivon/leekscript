#include "Callable.hpp"
#include "../../colors.h"
#include "../value/LeftValue.hpp"
#include "../../type/Function_type.hpp"
#include "../../vm/value/LSFunction.hpp"
#include "../value/ObjectAccess.hpp"
#include "../../type/Template_type.hpp"
#include "../value/Function.hpp"
#include "../semantic/SemanticAnalyser.hpp"

namespace ls {

void Callable::add_version(CallableVersion v) {
	versions.push_back(v);
}

Type build(const Type& type) {
	if (type.is_template()) return ((Template_type*) type._types[0].get())->_implementation;
	if (type.is_array()) return Type::array(build(type.element()));
	if (type.is_map()) return Type::map(build(type.key()), build(type.element()));
	if (type.is_function()) {
		std::vector<Type> args;
		for (const auto& t : type.arguments()) {
			args.push_back(build(t));
		}
		return Type::fun(build(type.return_type()), args);
	}
	return type;
}

CallableVersion* Callable::resolve(SemanticAnalyser* analyser, std::vector<Type> arguments) const {
	// std::cout << "Callable::resolve(" << arguments << ")" << std::endl;
	CallableVersion* best = nullptr;
	int best_score = std::numeric_limits<int>::max();
	for (auto& version : versions) {
		if (version.flags == Module::LEGACY and not analyser->vm->legacy) continue;
		std::vector<Type> version_arguments = arguments;
		if (version.object) {
			version_arguments.insert(version_arguments.begin(), version.object->type);
		}
		auto fun = dynamic_cast<const Function_type*>(version.type._types[0].get());
		auto f = fun ? dynamic_cast<const Function*>(fun->function()) : nullptr;

		// Template resolution
		auto version_type = version.type;
		if (version.templates.size()) {
			version.resolve_templates(analyser, version_arguments);
			version_type = build(version.type);
			// Reset template implementations
			for (size_t i = 0; i < version.templates.size(); ++i) {
				version.templates.at(i).implement({});
			}
		}
		CallableVersion* new_version = new CallableVersion(version);
		new_version->type = version_type;
		// std::cout << "Resolved version = " << version_type << std::endl;

		bool valid = true;
		for (size_t i = 0; i < version_type.arguments().size(); ++i) {
			if (i < version_arguments.size()) {
				const auto& a = version_arguments.at(i);
				const auto implem_arg = version_type.arguments().at(i);
				if (auto fun = dynamic_cast<const Function_type*>(a._types[0].get())) {
					if (fun->function() and implem_arg.is_function()) {
						auto version = implem_arg.arguments();
						((Value*) fun->function())->will_take(analyser, version, 1);
						version_arguments.at(i) = fun->function()->version_type(version);
					}
				}
			} else if (f and f->defaultValues.at(i)) {
				// Default argument
			} else {
				valid = false;
			}
		}
		if ((!valid or version_arguments.size() > version_type.arguments().size()) and not version.unknown) {
			continue;
		}
		int d = 0;
		bool ok = true;
		if (!version.unknown) {
			for (size_t i = 0; i < version_type.arguments().size(); ++i) {
				auto type = [&]() { if (i < version_arguments.size()) {
					return version_arguments.at(i);
				} else if (f and f->defaultValues.at(i)) {
					return f->defaultValues.at(i)->type;
				} else {
					assert(false);
				}}();
				auto di = type.distance(version_type.arguments().at(i));
				// std::cout << type << " distance " << version_type.arguments().at(i) << " " << di << std::endl;
				if (di < 0) { ok = false; break; };
				d += di;
			}
		}
		// std::cout << implementation.type.arguments() << " distance " << d << std::endl;
		if (ok and (best == nullptr or d <= best_score)) {
			best_score = d;
			best = new_version;
		}
	}
	return best;
}

void CallableVersion::apply_mutators(SemanticAnalyser* analyser, std::vector<Value*> arguments) {
	std::vector<Value*> values;
	if (object) values.push_back(object);
	values.insert(values.end(), arguments.begin(), arguments.end());
	// std::cout << "mutators : " << mutators.size() << std::endl;
	for (const auto& mutator : mutators) {
		mutator->apply(analyser, values);
	}
}

void solve(SemanticAnalyser* analyser, const Type& t1, const Type& t2) {
	// std::cout << "Solve " << t1 << " ||| " << t2 << std::endl;
	if (t1.is_template()) {
		t1.implement(t2);
	}
	else if (t1.is_array() and t2.is_array()) {
		solve(analyser, t1.element(), t2.element());
	}
	else if (t1.is_map() and t2.is_map()) {
		solve(analyser, t1.key(), t2.key());
		solve(analyser, t1.element(), t2.element());
	}
	else if (t1.is_function() and t2.is_function()) {
		auto fun = dynamic_cast<const Function_type*>(t2._types[0].get());
		if (fun) {
			auto f = (Value*) fun->function();
			if (f) {
				auto t1_args = build(t1).arguments();
				f->will_take(analyser, t1_args, 1);
				solve(analyser, t1.return_type(), fun->function()->version_type(t1_args).return_type());
			}
		}
	}
}

void CallableVersion::resolve_templates(SemanticAnalyser* analyser, std::vector<Type> arguments) const {
	// std::cout << "CallableVersion::resolve_templates(" << arguments << ")" << std::endl;
	// First passage to solve easy types
	for (size_t i = 0; i < arguments.size(); ++i) {
		const auto& t1 = type.argument(i);
		if (t1.is_template()) {
			const auto& t2 = arguments.at(i);
			// std::cout << t1 << " <=> " << t2 << std::endl;
			solve(analyser, t1, t2);
		}
	}
	for (size_t i = 0; i < arguments.size(); ++i) {
		const auto& t1 = type.argument(i);
		if (not t1.is_template()) {
			const auto& t2 = arguments.at(i);
			// std::cout << t1 << " <=> " << t2 << std::endl;
			solve(analyser, t1, t2);
		}
	}
}

Compiler::value CallableVersion::compile_call(Compiler& c, std::vector<Compiler::value> args, bool no_return) const {
	// std::cout << "CallableVersion::compile_call(" << args << ")" << std::endl;
	// Add the object if it's a method call
	Compiler::value compiled_object;
	if (object) {
		compiled_object = [&]() { if (object->isLeftValue()) {
			if (object->type.is_mpz_ptr()) {
				return ((LeftValue*) object)->compile_l(c);
			} else {
				return c.insn_load(((LeftValue*) object)->compile_l(c));
			}
		} else {
			return object->compile(c);
		}}();
		args.insert(args.begin(), compiled_object);
	}
	// Do the call
	auto r = [&]() { if (user_fun) {
		user_fun->compile(c);
		if (user_fun->type.is_closure() or unknown) {
			// std::cout << "closure add fun as first arg: " << user_fun->value.v << std::endl;
			if (user_fun->value.v) {
				args.insert(args.begin(), user_fun->value);
			} else {
				args.insert(args.begin(), {user_fun->f, Type::any()});
			}
		}
		if (flags & Module::THROWS) {
			return c.insn_invoke(type.return_type(), args, user_fun->f);
		} else {
			return c.insn_call(type.return_type(), args, user_fun->f);
		}
	} else if (addr) {
		if (flags & Module::THROWS) {
			return c.insn_invoke(type.return_type(), args, name);
		} else {
			return c.insn_call(type.return_type(), args, name);
		}
	} else if (func) {
		return func(c, args, no_return);
	} else if (value) {
		auto fun = [&]() { if (object) {
			auto oa = dynamic_cast<const ObjectAccess*>(value);
			auto k = c.new_const_string(oa->field->content, "field");
			return c.insn_invoke(type.pointer(), {compiled_object, k}, "Value.attr");
		} else {
			return value->compile(c);
		}}();
		if (fun.t.is_closure() or unknown) {
			args.insert(args.begin(), fun);
		}
		auto r = [&]() { if (unknown) {
			return c.insn_call(Type::any(), args, "Function.call");
		} else {
			return c.insn_invoke(type.return_type(), args, fun);
		}}();
		if (!object) {
			value->compile_end(c);
		}
		return r;
	} else {
		assert(false);
	}}();
	if (object) {
		object->compile_end(c);
	}
	return r;
}

}

namespace std {
	std::ostream& operator << (std::ostream& os, const ls::CallableVersion& v) {
		if (v.templates.size()) {
			os << "template<";
			for (size_t i = 0; i < v.templates.size(); ++i) {
				if (i != 0) std::cout << ", ";
				std::cout << v.templates.at(i);
			}
			os << "> ";
		}
		os << v.name << " ";
		if (v.object) os << "★ " << v.object << ":" << v.object->type << " ";
		os << v.type.arguments() << BLUE_BOLD << " => " << END_COLOR << v.type.return_type();
		if (v.user_fun) {
			os << " (user func " << v.user_fun << ")";
		} else if (v.addr) {
			os << " (native " << v.addr << ")";
		} else if (v.func) {
			os << " (compiler func)";
		} else {
			os << " (user function)";
		}
		if (v.unknown) os << " (unknown)";
		if (v.flags) os << " " << v.flags;
		return os;
	}
	std::ostream& operator << (std::ostream& os, const ls::CallableVersion* v) {
		os << *v;
		return os;
	}
	std::ostream& operator << (std::ostream& os, const ls::Callable* callable) {
		os << callable->name << " [" << std::endl;
		for (const auto& v : callable->versions) {
			os << "    " << v << std::endl;
		}
		os << "]";
		return os;
	}
}