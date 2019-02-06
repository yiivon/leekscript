#include "ClassDeclaration.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSClass.hpp"

namespace ls {

ClassDeclaration::ClassDeclaration(std::shared_ptr<Token> token) : token(token) {
	name = token->content;
	var = nullptr;
	ls_class = new LSClass(name);
}

ClassDeclaration::~ClassDeclaration() {
	for (const auto& vd : fields) {
		delete vd;
	}
	delete ls_class;
}

void ClassDeclaration::print(std::ostream& os, int indent, bool debug) const {
	os << "class " << name << " {" << std::endl;
	for (VariableDeclaration* vd : fields) {
		os << tabs(indent + 1);
		vd->print(os, indent + 1, debug);
		os << std::endl;
	}
	os << tabs(indent) << "}";
}

Location ClassDeclaration::location() const {
	return token->location;
}

void ClassDeclaration::analyse(SemanticAnalyser* analyser, const Type&) {

	var = analyser->add_var(token.get(), Type::clazz(), nullptr, nullptr);

	for (auto vd : fields) {
		vd->analyse(analyser, Type::any());
		for (size_t i = 0; i < vd->variables.size(); ++i) {
			// std::cout << "Add class field '" << vd->variables.at(i)->content << "' type " << vd->expressions.at(i)->type << std::endl;
			auto t = i < vd->expressions.size() ? vd->expressions.at(i)->type : Type::any();
			ls_class->addField(vd->variables.at(i)->content, t, nullptr);
		}
	}
}

Compiler::value ClassDeclaration::compile(Compiler& c) const {

	auto clazz = c.new_class(ls_class);

	for (auto vd : fields) {
		for (size_t i = 0; i < vd->variables.size(); ++i) {
			// std::cout << "Compile class field '" << vd->variables.at(i)->content << "' type " << vd->expressions.at(i)->type << std::endl;
			auto default_value = vd->expressions.at(i)->compile(c);
			default_value = c.insn_to_any(default_value);
			auto field_name = c.new_pointer(&vd->variables.at(i)->content, Type::any());
			c.insn_call({}, {clazz, field_name, default_value}, +[](LSClass* clazz, std::string* field_name, LSValue* default_value) {
				clazz->fields.at(*field_name).default_value = default_value;
			});
		}
	}

	c.add_var(name, clazz);

	return clazz;
}

Instruction* ClassDeclaration::clone() const {
	auto cd = new ClassDeclaration(token);
	cd->name = name;
	for (const auto& f : fields) {
		cd->fields.push_back((VariableDeclaration*) f->clone());
	}
	return cd;
}

}
