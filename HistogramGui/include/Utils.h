/**
 @file    Utils.h
 @brief Conveniently wrapped sstream conversions and container for other utility stuff in future
 @author   kzawisto
 @created:   2015-10-29
 **************************************************************/
#ifndef UTILS_H
#define UTILS_H
#include <sstream>
#include <string>

using namespace std;
/**
 *
 * String operations, etc. Wrapper class, never created, only owns static methods
 */
class Utils {

public:

	/// convert "something" to string
	template<class T>
	static string itos(T t1, unsigned int i = 4) {
		stringstream ss;
		ss.precision(i);
		ss << fixed;
		ss << t1;
		return ss.str();
	}
	/// convert string to something
	template<class T>
	static T stoi(string t1) {
		stringstream ss;
		ss << t1;
		T t2;
		ss >> t2;
		return t2;
	}

	static std::string format(double f, int n) {
	    if (f == 0) {
	        return "0";
	    }
	    int d = (int)::ceil(::log10(f < 0 ? -f : f)); /*digits before decimal point*/
	    double order = ::pow(10., n - d);
	    std::stringstream ss;
	    ss << std::fixed << std::setprecision(std::max(n - d, 0)) << round(f * order) / order;
	    return ss.str();
	}

protected:
private:
};

#endif // UTILS_H
