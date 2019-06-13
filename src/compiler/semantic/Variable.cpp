#include "Variable.hpp"
#include "../value/Value.hpp"

namespace ls {

void Variable::must_be_any(SemanticAnalyzer* analyzer) {
	if (value != nullptr) {
		value->must_be_any(analyzer);
	}
}

}