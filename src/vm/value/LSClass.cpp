#include "LSClass.hpp"
#include "LSString.hpp"
#include "LSNumber.hpp"
#include "LSFunction.hpp"
#include "../Module.hpp"
#include "../../type/Function_type.hpp"
#include "../../compiler/semantic/Callable.hpp"
#include "../../vm/VM.hpp"

namespace ls {

LSValue* LSClass::clazz;

LSClass* LSClass::constructor(char* name) {
	auto clazz = new LSClass(name);
	VM::current()->function_created.push_back(clazz);
	return clazz;
}
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

void LSClass::addMethod(std::string name, std::initializer_list<CallableVersion> impl, std::vector<Type> templates) {
	methods.insert({name, impl});
	methods.at(name).name = this->name + "." + name;
	int i = 0;
	for (auto& m : methods.at(name).versions) {
		m.name = this->name + "." + name + "." + std::to_string(i++);
		if (templates.size()) {
			m.templates = templates;
		}
	}
	// Add first implementation as default method
	auto fun = new LSFunction(impl.begin()->addr);
	Type type = impl.begin()->type;
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

void LSClass::addOperator(std::string name, std::initializer_list<CallableVersion> impl, std::vector<Type> templates) {
	operators.insert({name, impl});
	int i = 0;
	for (auto& m : operators.at(name).versions) {
		m.name = this->name + ".operator" + name + "." + std::to_string(i++);
		if (templates.size()) {
			m.templates = templates;
		}
	}
}

LSFunction* LSClass::getDefaultMethod(const std::string& name) {
	try {
		auto& f = static_fields.at(name);
		f.value->refs++;
		return (LSFunction*) f.value;
	} catch (...) {
		return nullptr;
	}
}

const Callable* LSClass::getOperator(SemanticAnalyzer* analyzer, std::string& name, Type& obj_type, Type& operand_type) {
	// std::cout << "getOperator(" << name << ", " << obj_type << ", " << operand_type << ")" << std::endl;
	if (name == "is not") name = "!=";
	if (name == "÷") name = "/";
	if (name == "×") name = "*";
	auto callable = new Callable(name);
	auto i = operators.find(name);
	if (i != operators.end()) {
		for (const auto& impl : i->second.versions) {
			callable->add_version(impl);
		}
	}
	if (name != "Value") {
		auto i = LSValue::ValueClass->operators.find(name);
		if (i != LSValue::ValueClass->operators.end()) {
			for (const auto& impl : i->second.versions) {
				callable->add_version(impl);
			}
		}
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
