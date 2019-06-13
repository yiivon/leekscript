#include "SemanticVar.hpp"
#include "../value/Value.hpp"

namespace ls {

void SemanticVar::must_be_any(SemanticAnalyzer* analyzer) {
	if (value != nullptr) {
		value->must_be_any(analyzer);
	}
}

}