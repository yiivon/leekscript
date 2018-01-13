#include "TypeMutator.hpp"

namespace ls {

WillStoreMutator::WillStoreMutator() {

}

void WillStoreMutator::apply(SemanticAnalyser* analyser, std::vector<Value*> values) const {
	// std::cout << "will store mutator " << values[0]->type << " += " << values[1]->type <<  std::endl;
	values[0]->will_store(analyser, values[1]->type);
}

}
