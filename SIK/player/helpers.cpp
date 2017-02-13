#include <string>
#include <sstream>
#include <cstring>

/*
 * Checks if endig is a suffix of full_string
 */
bool endswith(std::string const &full_string, std::string const &ending) {
	if (full_string.length() >= ending.length()) {
		return (0 == full_string.compare(full_string.length() - ending.length(), ending.length(), ending));
	} else {
		return false;
	}
}

/*
 * Parses an integer value from a string, formatted
 * as follows:
 * 		...
 * 		valuename: value
 * 		...
 * Returns -1 on failure
 */
int parse_value(std::string const &str, std::string const &valuename) {
	std::istringstream istr(str);
	std::string line;
	while (getline(istr, line)) {
		std::string delimeter = ":";
		size_t split = line.find(delimeter);
		if (line.substr(0, split) == valuename)
			return stoi(line.substr(split + 1, line.length())); // @TODO checking if int
	}
	return -1;
}

/*
 * Returns the unsigned integer value based
 * on its decimal representation as
 * a C string. Assumes the argument
 * is a valid C string. Returns _1 on failure.
 */
short parse_port_num(char *port_num) {
	size_t i;
	for (i = 0; i < strlen(port_num); i++)
		if (!isdigit(port_num[i]))
			return -1;
	return (short) atoi(port_num);
}

/*
 * Trims (from the left) all appearances of characters
 * from t (default contains whitespaces)
 */
std::string &ltrim(std::string &s, const char* t = " \t\n\r\f\v") {
	s.erase(0, s.find_first_not_of(t));
	return s;
}
