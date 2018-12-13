#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "Module.hpp"
#include "LSValue.hpp"
#include "value/LSClass.hpp"
#include "value/LSNumber.hpp"

using namespace std;

namespace ls {

bool Method::NATIVE = true;

Module::Module(std::string name) : name(name) {
	clazz = new LSClass(name);
	if (name != "Value") {
		clazz->parent = LSValue::ValueClass;
	}
}

Module::~Module() {
	//delete clazz;
}

void Module::operator_(std::string name, std::initializer_list<LSClass::Operator> impl) {
	vector<LSClass::Operator> operators = impl;
	clazz->addOperator(name, operators);
}

void Module::field(std::string name, Type type) {
	clazz->addField(name, type, nullptr);
}

void Module::field(std::string name, Type type, std::function<Compiler::value(Compiler&, Compiler::value)> fun) {
	clazz->addField(name, type, fun);
}
void Module::field(std::string name, Type type, void* fun) {
	clazz->addField(name, type, fun);
}

void Module::static_field(std::string name, Type type, std::function<Compiler::value(Compiler&)> fun) {
	clazz->addStaticField(ModuleStaticField(name, type, fun));
}
void Module::static_field(std::string name, Type type, void* fun) {
	clazz->addStaticField(ModuleStaticField(name, type, fun));
}

void Module::method(std::string name, Method::Option opt, initializer_list<MethodConstructor> methodsConstr) {
	std::vector<Method> inst;
	std::vector<StaticMethod> st;
	for (auto constr : methodsConstr) {
		if (opt == Method::Static || opt == Method::Both) {
			st.emplace_back(constr.return_type, constr.args, constr.addr, constr.native, constr.mutators);
		}
		if (opt == Method::Instantiate || opt == Method::Both) {
			assert(constr.args.size() > 0); // must be at least one argument to be the object used in instance
			auto obj_type = constr.args[0];
			constr.args.erase(constr.args.begin());
			inst.emplace_back(obj_type, constr.return_type, constr.args, constr.addr, constr.native, constr.mutators);
		}
	}
	if (!inst.empty()) {
		clazz->addMethod(name, inst);
	}
	if (!st.empty()) {
		clazz->addStaticMethod(name, st);
	}
}

void Module::generate_doc(std::ostream& os, std::string translation_file) {

	ifstream f;
	f.open(translation_file);
	if (!f.good()) {
		return; // no file
	}
	stringstream j;
	j << f.rdbuf();
	std::string str = j.str();
	f.close();

	// Erase tabs
	str.erase(std::remove(str.begin(), str.end(), '	'), str.end());

	// Parse json
	Json translation;
	try {
		translation = Json::parse(str);
	} catch (std::exception& e) { // LCOV_EXCL_LINE
		assert(false); // LCOV_EXCL_LINE
	}

	map<std::string, Json> translation_map;

	for (Json::iterator it = translation.begin(); it != translation.end(); ++it) {
		translation_map.insert({it.key(), it.value()});
	}

	os << "\"" << name << "\":{";

	os << "\"attributes\":{";
	int e = 0;
	for (auto& f : clazz->static_fields) {
		auto& a = f.second;
		auto desc = (translation_map.find(f.first) != translation_map.end()) ?
				translation_map[f.first] : "";
		if (!desc.is_string()) continue;

		if (e > 0) os << ",";
		os << "\"" << f.first << "\":{\"type\":";
		a.type.toJson(os);
		//os << ",\"value\":\"" << a.value << "\"";
		os << ",\"desc\":\"" << desc << "\"";
		os << "}";
		e++;
	}

	os << "},\"methods\":{";
	e = 0;
	for (auto& m : clazz->methods) {
		std::vector<Method>& impl = m.second;
		if (e > 0) os << ",";
		os << "\"" << m.first << "\":{\"type\":";
		impl[0].type.toJson(os);

		if (translation_map.find(m.first) != translation_map.end()) {
			Json json = translation_map[m.first];
			std::string desc = json["desc"];
			std::string return_desc = json["return"];

			os << ",\"desc\":\"" << desc << "\"";
			os << ",\"return\":\"" << return_desc << "\"";
		}
		os << "}";
		e++;
	}

	os << "},\"static_methods\":{";
	e = 0;
	for (auto& m : clazz->static_methods) {
		auto& impl = m.second;
		if (e > 0) os << ",";
		os << "\"" << m.first << "\":{\"type\":";
		impl[0].type.toJson(os);

		if (translation_map.find(m.first) != translation_map.end()) {
			Json json = translation_map[m.first];
			std::string desc = (json.find("desc") != json.end()) ? json["desc"] : "?";
			std::string return_desc = (json.find("return") != json.end()) ? json["return"] : "?";
			os << ",\"desc\":\"" << desc << "\"";
			os << ",\"return\":\"" << return_desc << "\"";
		}
		os << "}";
		e++;
	}
	os << "}}";
}

}
