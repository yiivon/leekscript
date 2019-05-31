#include "Callable.hpp"
#include "../../colors.h"
#include "../../type/Function_type.hpp"
#include "../../vm/value/LSFunction.hpp"
#include "../value/ObjectAccess.hpp"
#include "../../type/Template_type.hpp"
#include "../value/Function.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../../vm/Module.hpp"

namespace ls {

void Callable::add_version(const CallableVersion* v) {
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

const CallableVersion* Callable::resolve(SemanticAnalyzer* analyzer, std::vector<Type> arguments) const {
	// std::cout << "Callable::resolve(" << arguments << ")" << std::endl;
	const CallableVersion* best = nullptr;
	int best_score = std::numeric_limits<int>::max();
	for (auto& version : versions) {
		if (version->flags == Module::LEGACY and not analyzer->vm->legacy) continue;
		auto result = version->get_score(analyzer, arguments);
		// std::cout << implementation.type.arguments() << " distance " << d << std::endl;
		if (best == nullptr or result.first <= best_score) {
			best_score = result.first;
			best = result.second;
		}
	}
	return best;
}

std::pair<int, const CallableVersion*> CallableVersion::get_score(SemanticAnalyzer* analyzer, std::vector<Type> arguments) const {
	// std::cout << "CallableVersion::get_score(" << arguments << ") " << type << std::endl;
	
	// Template resolution
	const CallableVersion* new_version = this;
	if (templates.size()) {
		resolve_templates(analyzer, arguments);
		auto version_type = build(type);
		// Reset template implementations
		// TODO simplify loop
		for (size_t i = 0; i < templates.size(); ++i) {
			templates.at(i).implement({});
		}
		new_version = new CallableVersion(*this);
		((CallableVersion*) new_version)->type = version_type;
		// std::cout << "Resolved version = " << version_type << std::endl;
	}

	auto fun = dynamic_cast<const Function_type*>(type._types[0].get());
	auto f = fun ? dynamic_cast<const Function*>(fun->function()) : nullptr;
	bool valid = true;
	for (size_t i = 0; i < new_version->type.arguments().size(); ++i) {
		if (i < arguments.size()) {
			const auto& a = arguments.at(i);
			const auto implem_arg = new_version->type.arguments().at(i);
			if (auto fun = dynamic_cast<const Function_type*>(a._types[0].get())) {
				if (fun->function() and implem_arg.is_function()) {
					auto version = implem_arg.arguments();
					((Value*) fun->function())->will_take(analyzer, version, 1);
					arguments.at(i) = fun->function()->version_type(version);
				}
			}
		} else if (f and f->defaultValues.at(i)) {
			// Default argument
		} else {
			valid = false;
		}
	}
	if ((!valid or arguments.size() > new_version->type.arguments().size()) and not unknown) {
		return { std::numeric_limits<int>::max(), nullptr };
	}
	int d = 0;
	bool ok = true;
	if (!unknown) {
		for (size_t i = 0; i < new_version->type.arguments().size(); ++i) {
			auto type = [&]() { if (i < arguments.size()) {
				return arguments.at(i);
			} else if (f and f->defaultValues.at(i)) {
				return f->defaultValues.at(i)->type;
			} else {
				assert(false);
			}}();
			auto di = type.distance(new_version->type.arguments().at(i));
			// std::cout << type << " distance " << version_type.arguments().at(i) << " " << di << std::endl;
			if (di < 0) { ok = false; break; };
			d += di;
		}
	}
	if (!ok) return { std::numeric_limits<int>::max(), nullptr };
	return { d, new_version };
}

bool Callable::is_compatible(int argument_count) {
	for (const auto& version : versions) {
		if (version->type.arguments().size() == (size_t) argument_count) return true;
	}
	return false;
}

void CallableVersion::apply_mutators(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	// std::cout << "CallableVersion::apply_mutators() mutators : " << mutators.size() << std::endl;
	for (const auto& mutator : mutators) {
		mutator->apply(analyzer, values);
	}
}

void solve(SemanticAnalyzer* analyzer, const Type& t1, const Type& t2) {
	// std::cout << "Solve " << t1 << " ||| " << t2 << std::endl;
	if (t1.is_template()) {
		t1.implement(t2);
	}
	else if (t1.is_array() and t2.is_array()) {
		solve(analyzer, t1.element(), t2.element());
	}
	else if (t1.is_map() and t2.is_map()) {
		solve(analyzer, t1.key(), t2.key());
		solve(analyzer, t1.element(), t2.element());
	}
	else if (t1.is_function() and t2.is_function()) {
		auto fun = dynamic_cast<const Function_type*>(t2._types[0].get());
		if (fun) {
			auto f = (Value*) fun->function();
			if (f) {
				auto t1_args = build(t1).arguments();
				f->will_take(analyzer, t1_args, 1);
				solve(analyzer, t1.return_type(), fun->function()->version_type(t1_args).return_type());
			}
		}
	}
}

void CallableVersion::resolve_templates(SemanticAnalyzer* analyzer, std::vector<Type> arguments) const {
	// std::cout << "CallableVersion::resolve_templates(" << arguments << ")" << std::endl;
	// First passage to solve easy types
	for (size_t i = 0; i < arguments.size(); ++i) {
		const auto& t1 = type.argument(i);
		if (t1.is_template()) {
			const auto& t2 = arguments.at(i);
			// std::cout << t1 << " <=> " << t2 << std::endl;
			solve(analyzer, t1, t2);
		}
	}
	for (size_t i = 0; i < arguments.size(); ++i) {
		const auto& t1 = type.argument(i);
		if (not t1.is_template()) {
			const auto& t2 = arguments.at(i);
			// std::cout << t1 << " <=> " << t2 << std::endl;
			solve(analyzer, t1, t2);
		}
	}
}

Compiler::value CallableVersion::compile_call(Compiler& c, std::vector<Compiler::value> args, bool no_return) const {
	// std::cout << "CallableVersion::compile_call(" << args << ")" << std::endl;
	// Do the call
	auto r = [&]() { if (user_fun) {
		user_fun->compile(c);
		if (user_fun->type.is_closure() or unknown) {
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
	} else if (symbol) {
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
			auto k = c.new_const_string(oa->field->content);
			return c.insn_invoke(type.pointer(), {args[0], k}, "Value.attr");
		} else {
			return value->compile(c);
		}}();
		if (unknown) {
			args.insert(args.begin(), fun);
		}
		auto r = [&]() { if (unknown) {
			if (fun.t.is_closure()) {
				args.insert(args.begin(), fun);
			}
			return c.insn_call(Type::any(), args, "Function.call");
		} else {
			if (flags & Module::THROWS) {
				return c.insn_invoke(type.return_type(), args, fun);
			} else {
				return c.insn_call(type.return_type(), args, fun);
			}
		}}();
		if (!object) {
			value->compile_end(c);
		}
		return r;
	} else {
		assert(false);
	}}();
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
		// if (v.object) os << "★ " << v.object << ":" << v.object->type << " ";
		os << v.type.arguments() << BLUE_BOLD << " => " << END_COLOR << v.type.return_type();
		if (v.user_fun) {
			os << " (user func " << v.user_fun << ")";
		} else if (v.symbol) {
			os << " (symbol " << v.name << ")";
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
		os << "[" << std::endl;
		for (const auto& v : callable->versions) {
			os << "    " << *v << std::endl;
		}
		os << "]";
		return os;
	}
}