#include "LSClass.hpp"
#include "LSString.hpp"
#include "LSNumber.hpp"
#include "LSFunction.hpp"
#include "../Module.hpp"
#include "../../type/Function_type.hpp"
#include "../../compiler/semantic/Callable.hpp"

namespace ls {

LSValue* LSClass::clazz;

LSClass::LSClass(std::string name) : LSValue(CLASS), name(name) {
	parent = nullptr;
	refs = 1;
	native = true;
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

void LSClass::addMethod(std::string& name, std::vector<Method> method) {
	methods.insert({name, method});

	// Add first implementation as default method
	auto fun = new LSFunction(method[0].addr);
	fun->refs = 1;
	fun->native = true;
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

Method* LSClass::getMethod(SemanticAnalyser* analyser, std::string& name, std::vector<Type> arguments) {
	// std::cout << "getMethod " << name << " in class " << this->name << " with args " << arguments << std::endl;
	try {
		Method* best = nullptr;
		int best_score = std::numeric_limits<int>::max();
		for (auto& implementation : methods.at(name)) {
			if (implementation.type.arguments().size() != arguments.size()) continue;
			// std::cout << "templates : " << implementation.templates << std::endl;
			// implementation.templates[0].implement(Type::real());
			// implementation.templates[1].implement(Type::real());
			for (size_t i = 0; i < std::min(implementation.type.arguments().size(), arguments.size()); ++i) {
				const auto& a = arguments.at(i);
				const auto implem_arg = implementation.type.arguments().at(i);
				if (auto fun = dynamic_cast<const Function_type*>(a._types[0].get())) {
					if (fun->function() and implem_arg.is_function()) {
						auto version = implem_arg.arguments();
						((Value*) fun->function())->will_take(analyser, version, 1);
						arguments.at(i) = fun->function()->version_type(version);
					}
				}
			}
			int d = 0;
			for (size_t i = 0; i < arguments.size(); ++i) {
				auto di = arguments.at(i).distance(implementation.type.arguments().at(i));
				// std::cout << " - " << arguments.at(i) << ", " << implementation.type.arguments().at(i) << " " << di << std::endl;
				if (di < 0) { d = std::numeric_limits<int>::max(); break; };
				d += di;
			}
			// std::cout << implementation.type.arguments() << " distance " << d << std::endl;
			if (best == nullptr or d <= best_score) {
				best_score = d;
				best = &implementation;
			}
		}
		return best;
	} catch (std::exception&) {
		return nullptr;
	}
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
	std::vector<const Operator*> implementations;
	std::string op_name;
	if (operators.find(name) != operators.end()) {
		for (const auto& i : operators.at(name)) implementations.push_back(&i);
		op_name = this->name + ".operator" + name;
	}
	auto parent = name == "Value" ? nullptr : LSValue::ValueClass;
	if (parent && parent->operators.find(name) != parent->operators.end()) {
		for (const auto& i : parent->operators.at(name)) implementations.push_back(&i);
		op_name = "Value.operator" + name;
	}
	auto callable = new Callable(op_name);
	for (const auto& implementation : implementations) {
		auto type = Type::fun(implementation->return_type, {implementation->object_type, implementation->operand_type});
		if (implementation->native) {
			callable->add_version({ name, type, implementation->addr, implementation->mutators, {}, nullptr, false, implementation->v1_addr, implementation->v2_addr });
		} else {
			callable->add_version({ name, type, (Compiler::value (*)(Compiler&, std::vector<Compiler::value>)) implementation->addr, implementation->mutators, {}, nullptr, false,  implementation->v1_addr, implementation->v2_addr });
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
