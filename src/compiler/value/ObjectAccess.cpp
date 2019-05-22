#include "ObjectAccess.hpp"
#include <chrono>
#include "../semantic/SemanticAnalyzer.hpp"
#include "VariableValue.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSFunction.hpp"
#include "../../vm/value/LSClass.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Module.hpp"
#include "../../compiler/semantic/Callable.hpp"

namespace ls {

ObjectAccess::ObjectAccess(std::shared_ptr<Token> token) : field(token) {
	object = nullptr;
	attr_addr = nullptr;
	throws = true; // TODO doesn't always throw
}

ObjectAccess::~ObjectAccess() {
	delete object;
	if (ls_function != nullptr) {
		delete ls_function;
	}
}

bool ObjectAccess::isLeftValue() const {
	if (native_access_function.size()) {
		return false;
	}
	if (auto v = dynamic_cast<VariableValue*>(object)) {
		if (not v->isLeftValue()) return false;
	}
	return true;
}

void ObjectAccess::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	object->print(os, indent, debug, condensed);
	os << "." << field->content;
	if (debug) {
		os << " " << type;
	}
	os << " " << version;
}

Location ObjectAccess::location() const {
	return {object->location().start, field->location.end};
}

void ObjectAccess::set_version(const std::vector<Type>& args, int level) {
	// std::cout << "ObjectAccess::set_version(" << args << ", " << level << ")" << std::endl;
	version = args;
	has_version = true;
	if (callable) {
		for (const auto& m : callable->versions) {
			auto version = m.type.arguments();
			if (version == args) {
				type = Type::fun(m.type.return_type(), args, (const Value*) this);
			}
		}
	}
}

Type ObjectAccess::version_type(std::vector<Type> args) const {
	// std::cout << "ObjectAccess::version_tyoe(" << args << ")" << std::endl;
	if (callable) {
		for (const auto& m : callable->versions) {
			auto version = m.type.arguments();
			if (version == args) {
				return Type::fun(m.type.return_type(), args, (const Value*) this);
			}
		}
	}
	return type;
}

bool ObjectAccess::will_take(SemanticAnalyzer* analyzer, const std::vector<Type>& args, int level) {
	// std::cout << "OA will take " << args << std::endl;
	set_version(args, 1);
	return false;
}

Callable* ObjectAccess::get_callable(SemanticAnalyzer* analyzer, int argument_count) const {
	// std::cout << "ObjectAccess::get_callable(" << argument_count << ")" << std::endl;

	auto vv = dynamic_cast<VariableValue*>(object);
	auto value_class = (LSClass*) analyzer->vm->internal_vars.at("Value")->lsvalue;

	std::string object_class_name = object->type.class_name();
	LSClass* object_class = nullptr;
	if (analyzer->vm->internal_vars.find(object_class_name) != analyzer->vm->internal_vars.end()) {
		object_class = (LSClass*) analyzer->vm->internal_vars[object_class_name]->lsvalue;
	}
	
	Callable* callable = nullptr;

	// <class>.<field>
	if (object->type.is_class() and vv != nullptr) {
		auto std_class = (LSClass*) analyzer->vm->internal_vars.at(vv->name)->lsvalue;
		// <class>.<method>
		auto i = std_class->methods.find(field->content);
		if (i != std_class->methods.end() and i->second.is_compatible(argument_count)) {
			callable = &i->second;
			for (auto& v : callable->versions) {
				v.object = nullptr;
			}
			return callable;
		}
		// Value.<method>
		i = value_class->methods.find(field->content);
		if (i != value_class->methods.end() and i->second.is_compatible(argument_count)) {
			callable = &i->second;
			for (auto& v : callable->versions) {
				v.object = nullptr;
			}
			return callable;
		}
	}
	// <object>.<method>
	if (object_class) {
		auto i = object_class->methods.find(field->content);
		if (i != object_class->methods.end()) {
			callable = &i->second;
			for (auto& v : callable->versions) {
				v.object = object;
			}
			return callable;
		}
	}
	auto i = value_class->methods.find(field->content);
	if (i != value_class->methods.end() and i->second.is_compatible(argument_count + 1)) {
		callable = &i->second;
		for (auto& v : callable->versions) {
			v.object = object;
		}
		return callable;
	}
	if (!callable and not object->type.is_class()) {
		callable = new Callable("?");
		std::ostringstream oss;
		oss << object << "." << field->content;
		auto type = Type::fun(Type::any(), {Type::any(), Type::any()});
		callable->add_version({ oss.str(), type, this, {}, {}, object, true });
	}
	return callable;
}

void ObjectAccess::analyze(SemanticAnalyzer* analyzer) {

	// std::cout << "ObjectAccess analyse " << this << std::endl;

	object->analyze(analyzer);
	type = Type::any();

	// Get the object class : 12 => Number
	object_class_name = object->type.class_name();
	LSClass* object_class = nullptr;
	if (object_class_name != "Value" and analyzer->vm->internal_vars.find(object_class_name) != analyzer->vm->internal_vars.end()) {
		object_class = (LSClass*) analyzer->vm->internal_vars[object_class_name]->lsvalue;
	}

	// Static attribute? (Number.PI <= static attr)
	auto vv = dynamic_cast<VariableValue*>(object);

	bool found = false;
	if (object->type.is_class() and vv != nullptr) {

		auto std_class = (LSClass*) analyzer->vm->internal_vars.at(vv->name)->lsvalue;
		
		if (std_class->methods.find(field->content) != std_class->methods.end()) {

			auto& method = std_class->methods.at(field->content);
			int i = 0;
			for (const auto& m : method.versions) {
				versions.insert({m.type.arguments(), std_class->name + "." + field->content + "." + std::to_string(i)});
				i++;
			}
			type = Type::fun(method.versions[0].type.return_type(), method.versions[0].type.arguments(), (ObjectAccess*) this);
			default_version_fun = std_class->name + "." + field->content;
			class_method = true;
			callable = &method;
			found = true;
		}
	}
	if (!found and object->type.is_class() and vv != nullptr) {

		auto std_class = (LSClass*) analyzer->vm->internal_vars.at(vv->name)->lsvalue;

		if (std_class->static_fields.find(field->content) != std_class->static_fields.end()) {

			const auto& mod_field = std_class->static_fields.at(field->content);
			type = mod_field.type;

			if (mod_field.fun != nullptr) {
				static_access_function = mod_field.fun;
			}
			if (mod_field.native_fun != nullptr) {
				native_static_access_function = std_class->name + std::string(".") + mod_field.name;
			}
			field_type = mod_field.type;
			found = true;
		}
	}

	auto value_class = (LSClass*) analyzer->vm->internal_vars.at("Value")->lsvalue;

	// Attribute? Fields and methods ([1, 2, 3].length, 12.abs)
	if (!found and object_class != nullptr) {
		// Attribute : (x -> x).return
		try {
			const auto& f = object_class->fields.at(field->content);
			type = f.type;
			if (f.fun != nullptr) {
				access_function = f.fun;
			}
			if (f.native_fun != nullptr) {
				native_access_function = object_class->name + "." + f.name;
			}
		} catch (...) {
			// Attribute in Value?
			try {
				auto f = value_class->fields.at(field->content);
				type = f.type;
				class_field = true;
				if (f.fun != nullptr) {
					access_function = f.fun;
				}
				if (f.native_fun != nullptr) {
					native_access_function = "Value." + f.name;
				}
			} catch (...) {
				// Method : 12.abs
				try {
					for (const auto& m : object_class->methods.at(field->content).versions) {
						if (!m.addr) continue;
						versions.insert({m.type.arguments(), object_class->name + "." + field->content});
					}
					type = object_class->methods.at(field->content).versions[0].type;
					default_version_fun = object_class->name + "." + field->content;
					class_method = true;
				} catch (...) {
					try {
						for (const auto& m : value_class->methods.at(field->content).versions) {
							if (!m.addr) continue;
							versions.insert({m.type.arguments(), "Value." + field->content});
						}
						type = value_class->methods.at(field->content).versions[0].type;
						default_version_fun = "Value." + field->content;
						class_field = true;
					} catch (...) {
						if (object_class->name != "Object") {
							if (object->type.is_class() and vv != nullptr) {
								analyzer->add_error({SemanticError::Type::NO_SUCH_ATTRIBUTE, location(), field->location, {field->content, vv->name}});
							} else {
								analyzer->add_error({SemanticError::Type::NO_SUCH_ATTRIBUTE, location(), field->location, {field->content, object_class->name}});
							}
							return;
						}
					}
				}
			}
		}
	}
}

Compiler::value ObjectAccess::compile(Compiler& c) const {

	// Special case for custom attributes, accessible via a function
	// Static attributes : Number.PI
	if (static_access_function != nullptr) {
		return static_access_function(c);
	}
	if (native_static_access_function.size()) {
		return c.insn_call(field_type, {}, native_static_access_function);
	}

	// Field with an access function : 12.class
	if (access_function != nullptr) {
		auto obj = object->compile(c);
		object->compile_end(c);
		return access_function(c, obj);
	}
	if (native_access_function.size()) {
		auto obj = object->compile(c);
		object->compile_end(c);
		return c.insn_call(type, {obj}, native_access_function);
	}

	// Class method : 12.abs
	if (class_method || class_field) {
		const auto& fun = has_version and versions.find(version) != versions.end() ? versions.at(version) : default_version_fun;
		return c.new_function(fun, type);
	}

	// Default : object.attr
	auto o = object->compile(c);
	object->compile_end(c);
	auto k = c.new_const_string(field->content, "field");
	return c.insn_invoke(type, {o, k}, "Value.attr");
}

Compiler::value ObjectAccess::compile_version(Compiler& c, std::vector<Type> version) const {
	if (class_method) {
		return c.new_function(versions.at(version), Type::fun());
	}
	assert(false && "ObjectAccess::compile_version must be on a class method.");
}

Compiler::value ObjectAccess::compile_l(Compiler& c) const {
	auto o = [&]() { if (object->isLeftValue()) {
		return c.insn_load(((LeftValue*) object)->compile_l(c));
	} else {
		return object->compile(c);
	}}();
	object->compile_end(c);
	auto k = c.new_const_string(field->content, "field");
	return c.insn_invoke(type.pointer(), {o, k}, "Value.attrL");
}

Value* ObjectAccess::clone() const {
	auto oa = new ObjectAccess(field);
	oa->object = object->clone();
	return oa;
}

}
