#ifndef TYPE_MUTATOR_HPP
#define TYPE_MUTATOR_HPP

#include "../compiler/value/Value.hpp"

namespace ls {

class SemanticAnalyser;

class TypeMutator {
public:
	virtual void apply(SemanticAnalyser*, std::vector<Value*> values) const = 0;
};

class WillStoreMutator : public TypeMutator {
public:
	WillStoreMutator();
	virtual void apply(SemanticAnalyser*, std::vector<Value*> values) const override;
};

class ChangeTypeMutator : public TypeMutator {
public:
	ChangeTypeMutator() {}
	virtual void apply(SemanticAnalyser*, std::vector<Value*> values) const override;
};

}

#endif
