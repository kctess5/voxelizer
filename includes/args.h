#include <iostream>
#include <string>
#include <sstream>
// #include <cstdlib>


class NullBuffer : public std::streambuf { public: int overflow(int c) { return c; } };
class NullStream : public std::ostream { public: NullStream() : std::ostream(&m_sb) {} private: NullBuffer m_sb; };

struct Args {
	// meta settings
	int verbosity;
	NullStream null_stream;

	std::ostream& debug(int level) {
		if (verbosity >= level) {
			return std::cout;
		} else {
			return null_stream;
		}
	}
};
