#include "Module.hpp"
#include "LSValue.hpp"

using namespace std;

Module::Module(string name) : name(name) {}

Module::~Module() {}

void Module::include(SemanticAnalyser* analyser, Program* program) {

	LSClass* clazz = new LSClass(name);
	program->system_vars.insert(pair<string, LSValue*>(name, clazz));
	SemanticVar* var = analyser->add_var(name, Type::CLASS, nullptr);

	for (auto m : methods) {
		var->attr_types.insert(pair<string, Type>(m.name, m.type));
		clazz->addStaticField(m.name, new LSFunction(m.addr));
	}
}

void Module::attr(std::string name, Type type, std::string value) {

	attributes.push_back(ModuleAttribute(name, type, value));
}

void Module::method(string name, Type return_type, initializer_list<Type> args, void* addr) {

	Type type(RawType::FUNCTION, Nature::POINTER);
	type.setReturnType(return_type);

	for (Type arg : args) {
		type.addArgumentType(arg);
	}
	methods.push_back(ModuleMethod(name, type, addr));
}

void Module::generate_doc(ostream& os, JsonValue translation) {

	map<string, JsonValue> translation_map;
	if (translation.getTag() == JSON_OBJECT) {
		for (auto i : translation) {
			translation_map.insert(pair<string, JsonValue>(i->key, i->value));
		}
	}

	os << "\"" << name << "\":{";

	os << "\"attributes\":{";
	for (unsigned e = 0; e < attributes.size(); ++e) {
		ModuleAttribute& a = attributes[e];

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
