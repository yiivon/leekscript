#include "TypeMutator.hpp"
#include "../compiler/value/LeftValue.hpp"

namespace ls {

WillStoreMutator::WillStoreMutator() {}

void WillStoreMutator::apply(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	// std::cout << "will store mutator " << values[0]->type << " += " << values[1]->type << std::endl;
	values[0]->will_store(analyzer, values[1]->type);
}

void ChangeTypeMutator::apply(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	// std::cout << "change type mutator " << values[0]->type << " += " << values[1]->type << std::endl;
	if (!values[1]->type.strictCastable(values[0]->type)) {
		if (LeftValue* l = dynamic_cast<LeftValue*>(values[0])) {
			l->change_value(analyzer, values[1]);
		}
	}
}

void WillTakeMutator::apply(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	values[1]->will_take(analyzer, {Type::any()}, 1);
	// std::vector<Type> args;
	// for (const auto& extractor : types) {
	// 	args.push_back(extractor->extract(analyzer, values));
	// }
}

Type TypeExtractor::extract(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	return {};
}
Type ElementExtractor::extract(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	return extractor->extract(analyzer, values).element();
}
Type ArgumentExtractor::extract(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	return values.at(index)->type;
}

}
