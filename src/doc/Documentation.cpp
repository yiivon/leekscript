#include "Documentation.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../vm/standard/ArraySTD.hpp"
#include "../vm/standard/MapSTD.hpp"
#include "../vm/standard/BooleanSTD.hpp"
#include "../vm/standard/NumberSTD.hpp"
#include "../vm/standard/ObjectSTD.hpp"
#include "../vm/standard/StringSTD.hpp"
#include "../vm/standard/IntervalSTD.hpp"
#include "../vm/standard/SetSTD.hpp"
#include "../vm/standard/SystemSTD.hpp"
#include "../vm/standard/ClassSTD.hpp"
#include "../vm/standard/FunctionSTD.hpp"
#include "../vm/standard/JsonSTD.hpp"
#include "../vm/standard/NullSTD.hpp"
#include "../vm/standard/ValueSTD.hpp"

namespace ls {

Documentation::Documentation() {}

Documentation::~Documentation() {}

void Documentation::generate(VM* vm, std::ostream& os, std::string lang) {

	std::vector<Module*> modules;

	modules.push_back(new NullSTD());
	modules.push_back(new BooleanSTD());
	modules.push_back(new NumberSTD());
	modules.push_back(new ArraySTD());
	modules.push_back(new MapSTD());
	modules.push_back(new ObjectSTD());
	modules.push_back(new StringSTD());
	modules.push_back(new IntervalSTD());
	modules.push_back(new SetSTD());
	modules.push_back(new SystemSTD(vm));
	modules.push_back(new ClassSTD());
	modules.push_back(new FunctionSTD());
	modules.push_back(new JsonSTD());
	modules.push_back(new ValueSTD());

	os << "{";

	for (unsigned m = 0; m < modules.size(); ++m) {
		if (m > 0) os << ",";

		Module* mod = modules[m];
		std::string file = "src/doc/" + mod->name + "_" + lang + ".json";

		mod->generate_doc(os, file);
	}

	os << "}\n";

	for (const auto& m : modules) {
		delete m->clazz;
		delete m;
	}
}

}
