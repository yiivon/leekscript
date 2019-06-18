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

	modules.push_back(new NullSTD(vm));
	modules.push_back(new BooleanSTD(vm));
	modules.push_back(new NumberSTD(vm));
	modules.push_back(new ArraySTD(vm));
	modules.push_back(new MapSTD(vm));
	modules.push_back(new ObjectSTD(vm));
	modules.push_back(new StringSTD(vm));
	modules.push_back(new IntervalSTD(vm));
	modules.push_back(new SetSTD(vm));
	modules.push_back(new SystemSTD(vm));
	modules.push_back(new ClassSTD(vm));
	modules.push_back(new FunctionSTD(vm));
	modules.push_back(new JsonSTD(vm));
	modules.push_back(new ValueSTD(vm));

	os << "{";

	for (unsigned m = 0; m < modules.size(); ++m) {
		if (m > 0) os << ",";

		Module* mod = modules[m];
		std::string file = "src/doc/" + mod->name + "_" + lang + ".json";

		mod->generate_doc(os, file);
	}

	os << "}\n";

	for (const auto& m : modules) {
		delete m;
	}
}

}
