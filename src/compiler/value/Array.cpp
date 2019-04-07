#include "Array.hpp"
#include "../../vm/value/LSArray.hpp"
#include <math.h>

namespace ls {

Array::Array() {
	type = Type::array();
}

Array::~Array() {
	for (const auto& ex : expressions) {
		delete ex;
	}
}

void Array::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "[";
	for (size_t i = 0; i < expressions.size(); ++i) {
		expressions[i]->print(os, indent, debug, condensed);
		if (i < expressions.size() - 1) {
			os << ", ";
		}
	}
	os << "]";
	if (debug) {
		os << " " << type;
	}
}

Location Array::location() const {
	return {opening_bracket->location.start, closing_bracket->location.end};
}

void Array::analyse(SemanticAnalyser* analyser) {

	// std::cout << "Array::analyse " << req_type << std::endl;
	constant = true;

	if (expressions.size() > 0) {

		Type element_type = {};
		auto homogeneous = true;

		// First analyse pass
		for (size_t i = 0; i < expressions.size(); ++i) {

			Value* ex = expressions[i];
			ex->will_be_in_array(analyser);
			ex->analyse(analyser);

			if (ex->constant == false) {
				constant = false;
			}
			if (element_type._types.size() and element_type != ex->type) {
				homogeneous = false;
			}
			element_type = element_type * ex->type;
		}

		Type supported_type = {};
		// Native elements types supported : integer, double
		if (element_type == Type::integer() || element_type == Type::real()) {
			supported_type = element_type;
		}
		// For function, we store them as pointers
		else if (element_type.is_function()) {
			supported_type = element_type;
		} else {
			supported_type = Type::any();
			// If there are some functions, they types will be lost, so tell them to return pointers
			// TODO
			// supported_type.setReturnType(Type::any());
		}

		// Re-analyze expressions with the supported type
		// and second computation of the array type
		element_type = {};
		for (size_t i = 0; i < expressions.size(); ++i) {
			auto ex = expressions[i];
			if (!homogeneous and ex->type.is_array()) {
				// If the array stores other arrays of different types,
				// force those arrays to store pointers. (To avoid having
				// unknown array<int> inside arrays.
				ex->will_store(analyser, Type::any());
			}
			if (ex->type.is_function()) {
				std::vector<Type> types;
				for (unsigned p = 0; p < ex->type.arguments().size(); ++p) {
					types.push_back(Type::any());
				}
				if (types.size() > 0) {
					ex->will_take(analyser, types, 1);
				}
				ex->must_return_any(analyser);
			}
			element_type += ex->type;
		}
		if (element_type == Type::boolean()) element_type = Type::any();
		element_type.temporary = false;
		type = Type::array(element_type);
	}
	if (type.element()._types.size() == 0) {
		type = Type::array(Type::any());
	}
	type.temporary = true;
	// std::cout << "Array type : " << type << std::endl;
}

void Array::elements_will_take(SemanticAnalyser* analyser, const std::vector<Type>& arg_types, int level) {

	// std::cout << "Array::elements_will_take " << arg_types << " at " << level << std::endl;

	for (size_t i = 0; i < expressions.size(); ++i) {
		Array* arr = dynamic_cast<Array*>(expressions[i]);
		if (arr != nullptr && level > 0) {
			arr->elements_will_take(analyser, arg_types, level - 1);
		} else {
			expressions[i]->will_take(analyser, arg_types, 1);
		}
	}
	// Computation of the new array type
	Type element_type;
	for (unsigned i = 0; i < expressions.size(); ++i) {
		Value* ex = expressions[i];
		if (i == 0) {
			element_type = ex->type;
		} else {
			element_type = element_type * ex->type;
		}
	}
	this->type = Type::array(element_type);
	// std::cout << "Array::elements_will_take type after " << this->type << std::endl;
}

bool Array::will_store(SemanticAnalyser* analyser, const Type& type) {

	// std::cout << "Array::will_store " << this->type << " " << type << std::endl;

	Type added_type = type;
	if (added_type.is_array() or added_type.is_set()) {
		added_type = added_type.element();
	}
	Type current_type = this->type.element();
	if (expressions.size() == 0) {
		current_type = added_type;
	} else {
		current_type += added_type;
	}
	this->type = Type::array(current_type);
	return false;
}

bool Array::elements_will_store(SemanticAnalyser* analyser, const Type& type, int level) {
	for (auto& element : expressions) {
		element->will_store(analyser, type);
	}
	// Computation of the new array type
	Type element_type;
	for (unsigned i = 0; i < expressions.size(); ++i) {
		Value* ex = expressions[i];
		if (i == 0) {
			element_type = ex->type;
		} else {
			element_type = element_type * ex->type;
		}
	}
	this->type = Type::array(element_type);
	return false;
}

Compiler::value Array::compile(Compiler& c) const {
	std::vector<Compiler::value> elements;
	for (Value* val : expressions) {
		auto v = val->compile(c);
		val->compile_end(c);
		elements.push_back(v);
	}
	return c.new_array(type.element(), elements);
}

Value* Array::clone() const {
	auto array = new Array();
	array->opening_bracket = opening_bracket;
	array->closing_bracket = closing_bracket;
	for (const auto& ex : expressions) {
		array->expressions.push_back(ex->clone());
	}
	return array;
}

}
