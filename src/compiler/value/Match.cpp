#include "Match.hpp"

using namespace std;

namespace ls {

Match::Match()
{

}

Match::~Match()
{

}

void Match::print(std::ostream &os, bool debug) const
{
	os << "match ";
	value->print(os, debug);
	os << " { ";
	for (size_t i = 0; i < patterns.size(); ++i) {
		patterns[i]->print(os, debug);
		os << " => ";
		returns[i]->print(os, debug);
		os << ", ";
	}
	os << " }";
}

unsigned Match::line() const
{
	return 0;
}

void Match::analyse(ls::SemanticAnalyser *analyser, const Type &req_type)
{

}

jit_value_t Match::compile(Compiler &compiler) const
{

}

}
