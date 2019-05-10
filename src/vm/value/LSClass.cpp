#include "LSClass.hpp"
#include "LSString.hpp"
#include "LSNumber.hpp"
#include "LSFunction.hpp"
#include "../Module.hpp"
#include "../../type/Function_type.hpp"
#include "../../compiler/semantic/Callable.hpp"

namespace ls {

LSValue* LSClass::clazz;

LSClass::LSClass(std::string name) : LSValue(CLASS, 1, true), name(name) {
	parent = nullptr;
}

LSClass::~LSClass() {
	for (auto s : static_fields) {
		if (s.second.value != nullptr) {
			delete s.second.value;
		}
	}
	for (auto f : fields) {
		if (f.second.default_value != nullptr) {
			delete f.second.default_value;
		}
	}
}

void LSClass::addMethod(std::string name, std::vector<Method> method) {
	methods.insert({name, method});

	// Add first implementation as default method
	auto fun = new LSFunction(method[0].addr);
	Type type = method[0].type;
	static_fields.insert({name, ModuleStaticField(name, type, fun)});
}

void LSClass::addField(std::string name, Type type, std::function<Compiler::value(Compiler&, Compiler::value)> fun) {
	fields.insert({name, field(name, type, fun, nullptr)});
}
void LSClass::addField(std::string name, Type type, void* fun) {
	fields.insert({name, field(name, type, fun, nullptr)});
}

void LSClass::addStaticField(ModuleStaticField f) {
	static_fields.insert({f.name, f});
}

void LSClass::addOperator(std::string name, std::vector<Operator> impl) {
	operators.insert({name, impl});
}

LSFunction* LSClass::getDefaultMethod(const std::string& name) {
	try {
		auto f = static_fields.at(name);
		f.value->refs++;
		return (LSFunction*) f.value;
	} catch (...) {
		return nullptr;
	}
}

const Callable* LSClass::getOperator(SemanticAnalyser* analyser, std::string& name, Type& obj_type, Type& operand_type) {
	// std::cout << "getOperator(" << name << ", " << obj_type << ", " << operand_type << ")" << std::endl;
	if (name == "is not") name = "!=";
	if (name == "÷") name = "/";
	if (name == "×") name = "*";
	std::vector<const Operator*> implementations;
	std::vector<std::string> names;
	if (operators.find(name) != operators.end()) {
		int i = 0;
		for (const auto& impl : operators.at(name)) {
			implementations.push_back(&impl);
			names.push_back(this->name + ".operator" + name + "." + std::to_string(i));
			i++;
		}
	}
	auto parent = name == "Value" ? nullptr : LSValue::ValueClass;
	if (parent && parent->operators.find(name) != parent->operators.end()) {
		int i = 0;
		for (const auto& impl : parent->operators.at(name)) {
			implementations.push_back(&impl);
			names.push_back("Value.operator" + name + "." + std::to_string(i));
			i++;
		}
	}
	auto callable = new Callable(name);
	int i = 0;
	for (const auto& implementation : implementations) {
		auto type = Type::fun(implementation->return_type, {implementation->object_type, implementation->operand_type});
		auto version_name = names.at(i);
		if (implementation->addr) {
			callable->add_version({ version_name, type, implementation->addr, implementation->mutators, implementation->templates, nullptr, false, implementation->v1_addr, implementation->v2_addr, implementation->flags });
		} else {
			callable->add_version({ version_name, type, implementation->func, implementation->mutators, implementation->templates, nullptr, false,  implementation->v1_addr, implementation->v2_addr, implementation->flags });
		}
		i++;
	}
	// oppa oppa gangnam style tetetorettt tetetorett ! blank pink in the areaaahhh !! bombayah bomm bayah bom bayahh yah yahh yahhh yahh ! bom bom ba BOMBAYAH !!!ya ya ya ya ya ya OPPA !!
	return callable;
}

bool LSClass::to_bool() const {
	return true;
}

bool LSClass::ls_not() const {
	return false;
}

bool LSClass::eq(const LSValue* v) const {
	if (auto clazz = dynamic_cast<const LSClass*>(v)) {
		return clazz->name == this->name;
	}
	return false;
}

bool LSClass::lt(const LSValue* v) const {
	if (auto clazz = dynamic_cast<const LSClass*>(v)) {
		return this->name < clazz->name;
	}
	return LSValue::lt(v);
}

LSValue* LSClass::attr(const std::string& key) const {
	if (key == "name") {
		return new LSString(name);
	}
	try {
		return static_fields.at(key).value;
	} catch (std::exception&) {
		return LSValue::attr(key);
	}
}

std::ostream& LSClass::dump(std::ostream& os, int) const {
	os << "<class " << name << ">";
	return os;
}

std::string LSClass::json() const {
	return "\"<class " + name + ">\"";
}

LSValue* LSClass::getClass() const {
	return LSClass::clazz;
}

}
