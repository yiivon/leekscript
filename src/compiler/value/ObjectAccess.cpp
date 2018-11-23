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

void ObjectAccess::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	object->analyse(analyser, Type::UNKNOWN);

	if (object->type.nature == Nature::UNKNOWN) {
		type = Type::UNKNOWN;
	} else {
		type = Type::POINTER;
	}

	// Get the object class : 12 => Number
	object_class_name = object->type.clazz;
	LSClass* object_class = nullptr;
	if (analyser->vm->system_vars.find(object_class_name) != analyser->vm->system_vars.end()) {
		object_class = (LSClass*) analyser->vm->system_vars[object_class_name];
	}

	// Static attribute? (Number.PI <= static attr)
	auto vv = dynamic_cast<VariableValue*>(object);

	bool found = false;
	if (object->type.raw_type == RawType::CLASS and vv != nullptr) {

		auto std_class = (LSClass*) analyser->vm->system_vars.at(vv->name);

		if (std_class->methods.find(field->content) != std_class->methods.end()) {

			auto method = std_class->methods.at(field->content);
			for (const auto& m : method) {
				if (!m.native) continue;
				auto args = m.type.getArgumentTypes();
				args.insert(args.begin(), m.obj_type);
				versions.insert({args, m.addr});
			}
			type = method[0].type;
			default_version_fun = method[0].addr;
			class_method = true;
			found = true;
		}
	}
	if (!found and object->type.raw_type == RawType::CLASS and vv != nullptr) {

		auto std_class = (LSClass*) analyser->vm->system_vars.at(vv->name);

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

	auto value_class = (LSClass*) analyser->vm->system_vars.at("Value");

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
						versions.insert({m.type.getArgumentTypes(), m.addr});
					}
					type = object_class->methods.at(field->content)[0].type;
					default_version_fun = object_class->methods.at(field->content)[0].addr;
					class_method = true;
				} catch (...) {
					try {
						for (const auto& m : value_class->methods.at(field->content)) {
							if (!m.native) continue;
							versions.insert({m.type.getArgumentTypes(), m.addr});
						}
						type = value_class->methods.at(field->content)[0].type;
						default_version_fun = value_class->methods.at(field->content)[0].addr;
						class_field = true;
					} catch (...) {
						if (object_class->name != "Object") {
							if (object->type.raw_type == RawType::CLASS and vv != nullptr) {
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

	if (!access_function and !native_access_function and !static_access_function and !native_static_access_function and !class_method and object->type.nature != Nature::UNKNOWN) {
		object->analyse(analyser, Type::POINTER);
	}

	if (req_type.nature != Nature::UNKNOWN) {
		type.nature = req_type.nature;
	}
}

void ObjectAccess::change_type(SemanticAnalyser*, const Type&) {}

Compiler::value ObjectAccess::compile(Compiler& c) const {

	// Special case for custom attributes, accessible via a function
	// Static attributes : Number.PI
	if (static_access_function != nullptr) {
		auto res = static_access_function(c);
		if (field_type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
			return c.insn_to_pointer(res);
		}
		return res;
	}
	if (native_static_access_function != nullptr) {

		auto res = c.insn_call(field_type, {}, native_static_access_function);
		if (field_type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
			return c.insn_to_pointer(res);
		}
		return res;
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
		return c.insn_call(type, {obj}, native_access_function);
	}

	// Class method : 12.abs
	if (class_method || class_field) {
		void* fun = has_version ? versions.at(version) : default_version_fun;
		auto function = new LSFunction(fun);
		function->native = true;
		function->refs = 1;
		((ObjectAccess*) this)->ls_function = function;
		return c.new_pointer(ls_function);
	}

	// Default : object.attr
	auto o = object->compile(c);
	object->compile_end(c);
	auto k = c.new_pointer(&field->content);
	auto r = c.insn_call(type, {o, k}, (void*) +[](LSValue* object, std::string* key) {
		return object->attr(*key);
	});
	return r;
}

Compiler::value ObjectAccess::compile_version(Compiler& c, std::vector<Type> version) const {
	if (class_method) {
		return c.new_pointer(new LSFunction(versions.at(version)));
	}
	assert(false && "ObjectAccess::compile_version must be on a class method.");
}

Compiler::value ObjectAccess::compile_l(Compiler& c) const {
	auto o = object->compile(c);
	object->compile_end(c);
	auto k = c.new_pointer(&field->content);
	auto r = c.insn_call(type.add_pointer(), {o, k}, (void*) +[](LSValue* object, std::string* key) {
		return object->attrL(*key);
	});
	return r;
}

Value* ObjectAccess::clone() const {
	auto oa = new ObjectAccess(field);
	oa->object = object->clone();
	return oa;
}

}
