#include "Array.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSInterval.hpp"
#include <math.h>

using namespace std;

namespace ls {

Array::Array() {
	type = Type::ARRAY;
	conversion_type = Type::VOID;
}

Array::~Array() {
	for (const auto& ex : expressions) {
		delete ex;
	}
}

void Array::print(std::ostream& os, int indent, bool debug, bool condensed) const {

	if (interval) {
		os << "[";
		expressions[0]->print(os, indent, debug);
		os << "..";
		expressions[1]->print(os, indent, debug);
		os << "]";
	} else {
		os << "[";
		for (size_t i = 0; i < expressions.size(); ++i) {
			expressions[i]->print(os, indent, debug);
			if (i < expressions.size() - 1) {
				os << ", ";
			}
		}
		os << "]";
	}

	if (debug) {
		os << " " << type;
	}
}

Location Array::location() const {
	return {opening_bracket->location.start, closing_bracket->location.end};
}

void Array::analyse(SemanticAnalyser* analyser, const Type& req_type) {

	// std::cout << "Array::analyse " << req_type << std::endl;

	constant = true;

	if (interval) {

		type = Type::INTERVAL;
		type.temporary = true;
		expressions[0]->analyse(analyser, Type::INTEGER);
		expressions[1]->analyse(analyser, Type::INTEGER);

	} else {

		if (expressions.size() > 0) {

			Type element_type = Type::ANY;
			Type supported_type = Type::ANY;
			auto homogeneous = true;

			// First analyse pass
			for (size_t i = 0; i < expressions.size(); ++i) {

				Value* ex = expressions[i];
				ex->will_be_in_array(analyser);
				ex->analyse(analyser, Type::ANY);

				if (ex->constant == false) {
					constant = false;
				}
				if (element_type != Type::ANY and element_type != ex->type) {
					homogeneous = false;
				}
				element_type = Type::get_compatible_type(element_type, ex->type);
			}

			if (req_type.raw_type == RawType::ARRAY) {
				element_type = req_type.getElementType();
			}

			// Native elements types supported : integer, double
			if (element_type == Type::INTEGER || element_type == Type::REAL) {
				supported_type = element_type;
			}
			// For function, we store them as pointers
			else if (element_type.raw_type == RawType::FUNCTION) {
				element_type.nature = Nature::POINTER;
				supported_type = element_type;
			} else {
				supported_type = Type::POINTER;
				// If there are some functions, they types will be lost, so tell them to return pointers
				supported_type.setReturnType(Type::POINTER);
			}

			// Re-analyze expressions with the supported type
			// and second computation of the array type
			element_type = Type::ANY;
			if (req_type == Type::ANY_OLD) {
				element_type = Type::POINTER;
				supported_type = Type::POINTER;
			}
			for (size_t i = 0; i < expressions.size(); ++i) {
				auto ex = expressions[i];
				ex->analyse(analyser, supported_type);
				if (!homogeneous and ex->type.raw_type == RawType::ARRAY) {
					// If the array stores other arrays of different types,
					// force those arrays to store pointers. (To avoid having
					// unknown array<int> inside arrays.
					ex->will_store(analyser, Type::POINTER);
				}
				if (ex->type.raw_type == RawType::FUNCTION) {
					std::vector<Type> types;
					for (unsigned p = 0; p < ex->type.getArgumentTypes().size(); ++p) {
						types.push_back(Type::POINTER);
					}
					if (types.size() > 0) {
						ex->will_take(analyser, types, 1);
					}
					// e.g. Should compile a generic version
					ex->must_return(analyser, Type::POINTER);
				}
				if (element_type == Type::ANY or !element_type.compatible(ex->type)) {
					element_type = Type::get_compatible_type(element_type, ex->type);
				}
			}
			if (element_type == Type::BOOLEAN) element_type = Type::POINTER;
			element_type.temporary = false;
			type.setElementType(element_type);
		} else {
			if (req_type != Type::ANY) {
				type = req_type;
			}
		}
	}
	if (req_type == Type::ANY_OLD) {
		conversion_type = Type::ANY_OLD;
	}
	type.temporary = true;
	types = type;

	// std::cout << "Array type : " << type << std::endl;
}

void Array::elements_will_take(SemanticAnalyser* analyser, const std::vector<Type>& arg_types, int level) {

//	cout << "Array::elements_will_take " << type << " at " << pos << endl;

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
			element_type = Type::get_compatible_type(element_type, ex->type);
		}
	}
	this->type.setElementType(element_type);

//	cout << "Array::elements_will_take type after " << this->type << endl;
}

bool Array::will_store(SemanticAnalyser* analyser, const Type& type) {

	// std::cout << "Array::will_store " << this->type << " " << type << std::endl;

	Type added_type = type;
	if (added_type.raw_type == RawType::ARRAY or added_type.raw_type == RawType::SET) {
		added_type = added_type.getElementType();
	}
	Type current_type = this->type.getElementType();
	if (expressions.size() == 0) {
		this->type.setElementType(added_type);
	} else {
		this->type.setElementType(Type::get_compatible_type(current_type, added_type));
	}
	// Re-analyze expressions with the new type
	for (size_t i = 0; i < expressions.size(); ++i) {
		expressions[i]->analyse(analyser, this->type.getElementType());
	}
	this->types = type;
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
			element_type = Type::get_compatible_type(element_type, ex->type);
		}
	}
	this->type.setElementType(element_type);
}

Compiler::value Array::compile(Compiler& c) const {
	if (interval) {
		auto a = c.to_int(expressions[0]->compile(c));
		auto b = c.to_int(expressions[1]->compile(c));
		return c.insn_call(Type::INTERVAL_TMP, {a, b}, +[](int a, int b) {
			// TODO a better constructor?
			LSInterval* interval = new LSInterval();
			interval->a = a;
			interval->b = b;
			return interval;
		});
	}
	std::vector<Compiler::value> elements;
	for (Value* val : expressions) {
		auto v = c.insn_convert(val->compile(c), type.getElementType());
		val->compile_end(c);
		v = c.insn_move(v);
		elements.push_back(v);
	}
	auto array = c.new_array(type.getElementType(), elements);
	if (conversion_type == Type::ANY_OLD) {
		return { c.builder.CreatePointerCast(array.v, Type::POINTER.llvm_type()), Type::POINTER };
	}
	if (type.not_temporary() == Type::POINTER) {
		return { c.builder.CreatePointerCast(array.v, Type::POINTER.llvm_type()), Type::POINTER };
	}
	return array;
}

Value* Array::clone() const {
	auto array = new Array();
	array->opening_bracket = opening_bracket;
	array->closing_bracket = closing_bracket;
	for (const auto& ex : expressions) {
		array->expressions.push_back(ex->clone());
	}
	array->interval = interval;
	return array;
}

}
