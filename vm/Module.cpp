#include "Module.hpp"
#include "LSValue.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

Module::Module(string name) : name(name) {
	clazz = new LSClass(name);
}

Module::~Module() {}

void Module::include(SemanticAnalyser* analyser, Program* program) {

	program->system_vars.insert(pair<string, LSValue*>(name, clazz));
	SemanticVar* var = analyser->add_var(new Token(name), Type::CLASS, nullptr);

	for (auto f : fields) {
		//var->attr_types.insert(pair<string, Type>(f.name, f.type));
		clazz->addField(f.name, f.type);
	}

	for (auto f : static_fields) {
		//var->attr_types.insert(pair<string, Type>(f.name, f.type));
		clazz->addStaticField(f.name, LSNumber::get(std::stoi(f.value)));
	}

	for (auto m : methods) {
		var->attr_types.insert(pair<string, Type>(m.name, m.type));
		clazz->addStaticField(m.name, new LSFunction(m.addr));
	}
}

void Module::field(std::string name, Type type) {

	fields.push_back(ModuleField(name, type));
}

void Module::static_field(std::string name, Type type, std::string value) {

	static_fields.push_back(ModuleStaticField(name, type, value));
}

void Module::method(string name, Type return_type, initializer_list<Type> args, void* addr) {

	Type type(RawType::FUNCTION, Nature::POINTER);
	type.setReturnType(return_type);

	for (Type arg : args) {
		type.addArgumentType(arg);
	}
	methods.push_back(ModuleMethod(name, type, addr));
}

void Module::generate_doc(ostream& os, string translation_file) {

	ifstream f;
	f.open(translation_file);
	stringstream j;
	j << f.rdbuf();
	string str = j.str();
	f.close();

	// Erase tabs
	str.erase(std::remove(str.begin(), str.end(), '	'), str.end());

	// Parse json
	char *endptr;
	JsonValue translation(JSON_NULL, nullptr);
	JsonAllocator allocator;
	jsonParse((char*) str.c_str(), &endptr, &translation, allocator);

	map<string, JsonValue> translation_map;
	if (translation.getTag() == JSON_OBJECT) {
		for (auto i : translation) {
			translation_map.insert(pair<string, JsonValue>(i->key, i->value));
		}
	}

	os << "\"" << name << "\":{";

	os << "\"attributes\":{";
	for (unsigned e = 0; e < static_fields.size(); ++e) {
		ModuleStaticField& a = static_fields[e];

		string desc = (translation_map.find(a.name) != translation_map.end()) ?
				translation_map[a.name].toNode()->value.toString() : "";

		if (e > 0) os << ",";
		os << "\"" << a.name << "\":{\"type\":";
		a.type.toJson(os);
		os << ",\"value\":\"" << a.value << "\"";
		os << ",\"desc\":\"" << desc << "\"";
		os << "}";
	}

	os << "},\"methods\":{";
	for (unsigned e = 0; e < methods.size(); ++e) {
		ModuleMethod& m = methods[e];

		if (e > 0) os << ",";
		os << "\"" << m.name << "\":{\"type\":";
		m.type.toJson(os);

		if (translation_map.find(m.name) != translation_map.end()) {
			JsonNode* json = translation_map[m.name].toNode();
			string desc = json->value.toString();
			//JsonNode* args = json->next->value.toNode();
			string return_desc = json->next->next->value.toString();

			os << ",\"desc\":\"" << desc << "\"";
			os << ",\"return\":\"" << return_desc << "\"";
		}
		os << "}";
	}
	os << "}}";
}
