#include "ClassDeclaration.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSClass.hpp"

namespace ls {

ClassDeclaration::ClassDeclaration(std::shared_ptr<Token> token) : token(token) {
	name = token->content;
	var = nullptr;
}

void ClassDeclaration::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "class " << name << " {" << std::endl;
	for (const auto& vd : fields) {
		os << tabs(indent + 1);
		vd->print(os, indent + 1, debug, condensed);
		os << std::endl;
	}
	os << tabs(indent) << "}";
	if (debug) os << " " << type;
}

Location ClassDeclaration::location() const {
	return token->location;
}

void ClassDeclaration::pre_analyze(SemanticAnalyzer* analyzer) {
	var = analyzer->add_var(token.get(), Type::clazz(), nullptr);
	for (const auto& vd : fields) {
		vd->pre_analyze(analyzer);
	}
}

void ClassDeclaration::analyze(SemanticAnalyzer* analyzer, const Type*) {
	for (const auto& vd : fields) {
		vd->analyze(analyzer, Type::any);
	}
}

Compiler::value ClassDeclaration::compile(Compiler& c) const {

	auto clazz = c.new_class(name);

	for (const auto& vd : fields) {
		for (size_t i = 0; i < vd->variables.size(); ++i) {
			// std::cout << "Compile class field '" << vd->variables.at(i)->content << "' type " << vd->expressions.at(i)->type << std::endl;
			auto default_value = vd->expressions.at(i)->compile(c);
			default_value = c.insn_to_any(default_value);
			auto field_name = c.new_const_string(vd->variables.at(i)->content);
			c.insn_call(Type::void_, {clazz, field_name, default_value}, "Class.add_field");
		}
	}

	c.add_var(name, clazz);

	return clazz;
}

std::unique_ptr<Instruction> ClassDeclaration::clone() const {
	auto cd = std::make_unique<ClassDeclaration>(token);
	cd->name = name;
	for (const auto& f : fields) {
		cd->fields.push_back(unique_static_cast<VariableDeclaration>(f->clone()));
	}
	return cd;
}

}
