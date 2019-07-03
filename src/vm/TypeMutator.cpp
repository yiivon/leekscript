#include "TypeMutator.hpp"
#include "../compiler/value/LeftValue.hpp"
#include "../compiler/value/VariableValue.hpp"
#include "../compiler/semantic/Variable.hpp"
#include "../type/Type.hpp"

namespace ls {

WillStoreMutator::WillStoreMutator() {}

void WillStoreMutator::apply(SemanticAnalyzer* analyzer, std::vector<Value*> values, const Type* return_type) const {
	// std::cout << "will store mutator " << values[0]->type << " += " << values[1]->type << std::endl;
	auto type = values[1]->type;
	if (type->is_function()) type = Type::fun_object(type->return_type(), type->arguments());
	values[0]->will_store(analyzer, type);
	values[1]->must_return_any(analyzer);
}

void ConvertMutator::apply(SemanticAnalyzer* analyzer, std::vector<Value*> values, const Type* return_type) const {
	std::cout << "ConvertMutator " << values[0]->type << " " << return_type << std::endl;
	if (auto vv = dynamic_cast<VariableValue*>(values[0])) {
		vv->type = return_type->not_temporary();
		vv->var->type = vv->type;
	}
}
void ChangeValueMutator::apply(SemanticAnalyzer* analyzer, std::vector<Value*> values, const Type* return_type) const {
	// std::cout << "change type mutator " << values[0]->type << " += " << values[1]->type << std::endl;
}

void WillTakeMutator::apply(SemanticAnalyzer* analyzer, std::vector<Value*> values, const Type* return_type) const {
	values[1]->will_take(analyzer, {Type::any}, 1);
	// std::vector<Type> args;
	// for (const auto& extractor : types) {
	// 	args.push_back(extractor->extract(analyzer, values));
	// }
}

const Type* TypeExtractor::extract(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	return Type::void_;
}
const Type* ElementExtractor::extract(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	return extractor->extract(analyzer, values)->element();
}
const Type* ArgumentExtractor::extract(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	return values.at(index)->type;
}

}
