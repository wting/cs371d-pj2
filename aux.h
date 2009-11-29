#ifndef AUX_H
#define AUX_H

#include <string>
#include <boost/lexical_cast.hpp>

namespace aux {
	template <typename T>
	std::string to_str(const T &input) {
		return boost::lexical_cast<std::string>(input);
	}
}

#endif // AUX_H
