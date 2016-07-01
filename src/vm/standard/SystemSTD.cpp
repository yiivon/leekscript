#include "SystemSTD.hpp"

namespace ls {

SystemSTD::SystemSTD() : Module("System") {

	static_field("operations", Type::INTEGER, "0");
}

}
