#include "ObjectAccess.hpp"
#include <chrono>
#include "../semantic/SemanticAnalyser.hpp"
#include "VariableValue.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSFunction.hpp"
#include "../../vm/value/LSClass.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Module.hpp"

using namespace std;

namespace ls {

ObjectAccess::ObjectAccess(std::shared_ptr<Token> token) : field(token) {
	object = nullptr;
	attr_addr = nullptr;
}

ObjectAccess::~ObjectAccess() {
	delete object;
	if (ls_function != nullptr) {
		delete ls_function;
	}
}

bool ObjectAccess::isLeftValue() const {
	if (native_access_function != nullptr) {
		return false;
	}
	if (auto v = dynamic_cast<VariableValue*>(object)) {
		if (not v->isLeftValue()) return false;
	}
	return true;
}

void ObjectAccess::print(ostream& os, int indent, bool debug, bool condensed) const {
	object->print(os, indent, debug);
	os << "." << field->content;
	if (debug) {
		os << " " << type;
	}
	os << " " << version;
}

Location ObjectAccess::location() const {
	return {object->location().start, field->location.end};
}

void ObjectAccess::set_version(const vector<Type>& args, int level) {
	version = args;
	has_version = true;
	if (class_method) {
		for (const auto& m : methods) {
			if (!m.native) continue;
			auto version = m.type.arguments();
			version.push_back(m.obj_type);
			if (version == args) {
				type = Type::fun(m.type.return_type(), args);
			}
		}
	}
}

void ObjectAccess::analyse(SemanticAnalyser* analyser) {

	object->analyse(analyser);
	type = Type::any();

	// Get the object class : 12 => Number
	object_class_name = object->type.class_name();
	LSClass* object_class = nullptr;
	if (analyser->vm->internal_vars.find(object_class_name) != analyser->vm->internal_vars.end()) {
		object_class = (LSClass*) analyser->vm->internal_vars[object_class_name]->lsvalue;
	}

	// Static attribute? (Number.PI <= static attr)
	auto vv = dynamic_cast<VariableValue*>(object);

	bool found = false;
	if (object->type.is_class() and vv != nullptr) {

		auto std_class = (LSClass*) analyser->vm->internal_vars.at(vv->name)->lsvalue;

		if (std_class->methods.find(field->content) != std_class->methods.end()) {

			auto method = std_class->methods.at(field->content);
			for (const auto& m : method) {
				if (!m.native) continue;
				auto args = m.type.arguments();
				args.insert(args.begin(), m.obj_type);
				versions.insert({args, m.addr});
			}
			type = method[0].type;
			auto args = type.arguments(); args.insert(args.begin(), method[0].obj_type);
			type = Type::fun(type.return_type(), args);
			default_version_fun = method[0].addr;
			class_method = true;
			methods = method;
			found = true;
		}
	}
	if (!found and object->type.is_class() and vv != nullptr) {

		auto std_class = (LSClass*) analyser->vm->internal_vars.at(vv->name)->lsvalue;

		if (std_class->static_fields.find(field->content) != std_class->static_fields.end()) {

			auto mod_field = std_class->static_fields.at(field->content);
			type = mod_field.type;

			if (mod_field.fun != nullptr) {
				static_access_function = mod_field.fun;
			}
			if (mod_field.native_fun != nullptr) {
				native_static_access_function = mod_field.native_fun;
			}
			field_type = mod_field.type;
			found = true;
		}
	}

	auto value_class = (LSClass*) analyser->vm->internal_vars.at("Value")->lsvalue;

	// Attribute? Fields and methods ([1, 2, 3].length, 12.abs)
	if (!found and object_class != nullptr) {
		// Attribute : (x -> x).return
		try {
			auto f = object_class->fields.at(field->content);
			type = f.type;
			if (f.fun != nullptr) {
				access_function = f.fun;
			}
			if (f.native_fun != nullptr) {
				native_access_function = f.native_fun;
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
					native_access_function = f.native_fun;
				}
			} catch (...) {
				// Method : 12.abs
				try {
					for (const auto& m : object_class->methods.at(field->content)) {
						if (!m.native) continue;
						versions.insert({m.type.arguments(), m.addr});
					}
					type = object_class->methods.at(field->content)[0].type;
					default_version_fun = object_class->methods.at(field->content)[0].addr;
					class_method = true;
				} catch (...) {
					try {
						for (const auto& m : value_class->methods.at(field->content)) {
							if (!m.native) continue;
							versions.insert({m.type.arguments(), m.addr});
						}
						type = value_class->methods.at(field->content)[0].type;
						default_version_fun = value_class->methods.at(field->content)[0].addr;
						class_field = true;
					} catch (...) {
						if (object_class->name != "Object") {
							if (object->type.is_class() and vv != nullptr) {
								analyser->add_error({SemanticError::Type::NO_SUCH_ATTRIBUTE, location(), field->location, {field->content, vv->name}});
							} else {
								analyser->add_error({SemanticError::Type::NO_SUCH_ATTRIBUTE, location(), field->location, {field->content, object_class->name}});
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
	if (native_static_access_function != nullptr) {
		return c.insn_invoke(field_type, {}, native_static_access_function);
	}

	// Field with an access function : 12.class
	if (access_function != nullptr) {
		auto obj = object->compile(c);
		object->compile_end(c);
		return access_function(c, obj);
	}
	if (native_access_function != nullptr) {
		auto obj = object->compile(c);
		object->compile_end(c);
		return c.insn_invoke(type, {obj}, native_access_function);
	}

	// Class method : 12.abs
	if (class_method || class_field) {
		void* fun = has_version ? versions.at(version) : default_version_fun;
		auto function = new LSFunction(fun);
		function->native = true;
		function->refs = 1;
		((ObjectAccess*) this)->ls_function = function;
		return c.new_pointer(ls_function, Type::any());
	}

	// Default : object.attr
	auto o = object->compile(c);
	object->compile_end(c);
	auto k = c.new_pointer(&field->content, Type::any());
	auto r = c.insn_invoke(type, {o, k}, (void*) +[](LSValue* object, std::string* key) {
		return object->attr(*key);
	});
	return r;
}

Compiler::value ObjectAccess::compile_version(Compiler& c, std::vector<Type> version) const {
	if (class_method) {
		return c.new_function(new LSFunction(versions.at(version)), Type::fun());
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
	auto k = c.new_pointer(&field->content, Type::any());
	return c.insn_invoke(type.pointer(), {o, k}, (void*) +[](LSValue* object, std::string* key) {
		return object->attrL(*key);
	});
}

Value* ObjectAccess::clone() const {
	auto oa = new ObjectAccess(field);
	oa->object = object->clone();
	return oa;
}

}
