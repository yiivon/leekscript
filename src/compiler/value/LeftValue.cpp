#include "LeftValue.hpp"

namespace ls {

bool LeftValue::isLeftValue() const {
	return true;
}
void LeftValue::change_type(SemanticAnalyser*, const Type&, const Ty&) {}

}
