#include "TypeMutator.hpp"
#include "../compiler/value/LeftValue.hpp"

namespace ls {

WillStoreMutator::WillStoreMutator() {}

void WillStoreMutator::apply(SemanticAnalyser* analyser, std::vector<Value*> values) const {
	// std::cout << "will store mutator " << values[0]->type << " += " << values[1]->type << std::endl;
	values[0]->will_store(analyser, values[1]->type);
}

void ChangeTypeMutator::apply(SemanticAnalyser* analyser, std::vector<Value*> values) const {
	// std::cout << "change type mutator " << values[0]->type << " += " << values[1]->type << std::endl;
	if (!values[1]->type.strictCastable(values[0]->type)) {
		if (LeftValue* l = dynamic_cast<LeftValue*>(values[0])) {
			l->change_value(analyser, values[1]);
		}
	}
}

}
