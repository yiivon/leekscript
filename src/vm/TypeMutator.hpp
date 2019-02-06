#ifndef TYPE_MUTATOR_HPP
#define TYPE_MUTATOR_HPP

#include "../compiler/value/Value.hpp"

namespace ls {

class SemanticAnalyser;

class TypeMutator {
public:
	virtual void apply(SemanticAnalyser*, std::vector<Value*> values) const = 0;
};
class TypeExtractor {
public:
	virtual Type extract(SemanticAnalyser*, std::vector<Value*> values) const;
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

class WillTakeMutator : public TypeMutator {
public:
	int index;
	std::vector<Type> types;
	WillTakeMutator(int index, std::vector<Type> types) : index(index), types(types) {}
	virtual void apply(SemanticAnalyser*, std::vector<Value*> values) const override;
};

class ElementExtractor : public TypeExtractor {
public:
	TypeExtractor* extractor;
	ElementExtractor(TypeExtractor* extractor) : extractor(extractor) {}
	virtual Type extract(SemanticAnalyser*, std::vector<Value*> values) const override;
};
class ArgumentExtractor : public TypeExtractor {
public:
	int index;
	ArgumentExtractor(int index) : index(index) {}
	virtual Type extract(SemanticAnalyser*, std::vector<Value*> values) const override;
};

}

#endif
