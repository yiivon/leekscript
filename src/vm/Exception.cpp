#include "Exception.hpp"
#include "VM.hpp"

namespace ls {
namespace vm {

std::string ExceptionObj::to_string(bool colors) const {
	auto pad = [](std::string s, int l) {
		l -= s.size();
		while (l-- > 0) s = " " + s;
		return s;
	};
	size_t padding = 0;
	for (auto& f : frames) {
		padding = fmax(padding, f.function.size() + 2);
	}
	std::ostringstream oss;
	oss << "Exception " << (colors ? BOLD : "") << VM::exception_message(type) << (colors ? END_COLOR : "") << std::endl;
	for (const auto& f : frames) {
		oss << (colors ? BOLD : "") << "    > " << (colors ? END_COLOR : "") << pad(f.function + "()", padding) << " @ " << (colors ? BOLD : "") << f.file << ":" << f.line << (colors ? END_COLOR : "");
		#if STACKTRACE_DETAILS
			oss << " (frame: " << f.frame << ", pc: " << f.pc << ")";
		#endif
		oss << std::endl;
	}
	return oss.str();
}

}
}
