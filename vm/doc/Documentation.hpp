#ifndef VM_DOC_DOCUMENTATION_HPP_
#define VM_DOC_DOCUMENTATION_HPP_

#include <vector>
#include <ostream>
#include "../Module.hpp"

class Documentation {
public:

	Documentation();
	virtual ~Documentation();

	void generate(std::ostream& os);
};

#endif
