#ifndef OUTPUT_STREAM_HPP
#define OUTPUT_STREAM_HPP

#include <iostream>
#include <ostream>

namespace ls {

class OutputStream {
public:
	virtual std::ostream& stream() {
		return std::cout;
	}
	virtual void end() {
		std::cout << std::endl;
	}
};

}

#endif