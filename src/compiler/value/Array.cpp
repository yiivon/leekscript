#include "Array.hpp"
#include "../../vm/VM.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSInterval.hpp"
#include <math.h>

using namespace std;

namespace ls {

Array::Array() {
	type = Type::PTR_ARRAY;
}

Array::~Array() {
	for (const auto& ex : expressions) {
		delete ex;
	}
}

void Array::print(std::ostream& os, int indent, bool debug) const {

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

void Array::analyse(SemanticAnalyser* analyser, const Type&) {

	constant = true;

	if (interval) {

		type = Type::INTERVAL;
		expressions[0]->analyse(analyser, Type::INTEGER);
		expressions[1]->analyse(analyser, Type::INTEGER);

	} else {

		if (expressions.size() > 0) {

			Type element_type = Type::UNKNOWN;
			Type supported_type = Type::UNKNOWN;

			// First analyse pass
			for (size_t i = 0; i < expressions.size(); ++i) {

				Value* ex = expressions[i];
				ex->will_be_in_array(analyser);
				ex->analyse(analyser, Type::UNKNOWN);

				if (ex->constant == false) {
					constant = false;
				}
				element_type = Type::get_compatible_type(element_type, ex->type);
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
			for (size_t i = 0; i < expressions.size(); ++i) {
				expressions[i]->analyse(analyser, supported_type);
				if (expressions[i]->type.raw_type == RawType::FUNCTION) {
					std::vector<Type> types;
					for (unsigned p = 0; p < expressions[i]->type.getArgumentTypes().size(); ++p) {
						types.push_back(Type::POINTER);
					}
					if (types.size() > 0) {
						expressions[i]->will_take(analyser, types, 1);
					}
					expressions[i]->must_return(analyser, Type::POINTER);
				}
			}

			// Second computation of the array type
			element_type = Type::UNKNOWN;
			for (unsigned i = 0; i < expressions.size(); ++i) {
				Value* ex = expressions[i];
				if (element_type == Type::UNKNOWN or !element_type.compatible(ex->type)) {
					element_type = Type::get_compatible_type(element_type, ex->type);
				}
			}
			element_type.temporary = false;
			type.setElementType(element_type);
		}
	}
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

Compiler::value Array::compile(Compiler& c) const {

	if (interval) {
		Compiler::value a = {expressions[0]->compile(c).v, Type::INTEGER};
		Compiler::value b = {expressions[1]->compile(c).v, Type::INTEGER};
		return c.insn_call(Type::INTERVAL, {a, b}, +[](int a, int b) {
			// TODO a better constructor?
			LSInterval* interval = new LSInterval();
			interval->a = a;
			interval->b = b;
			return interval;
		});
	}

	Compiler::value array = {VM::create_array(c.F, type.getElementType(), expressions.size()), type};

	for (Value* val : expressions) {
		auto v = val->compile(c);
		c.insn_push_move_array(array, {v.v, type.getElementType()});
	}

	// size of the array + 1 operations
	c.inc_ops(expressions.size() + 1);

	return array;
}

}
