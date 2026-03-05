#ifndef _SRC_UTILITIES_HPP_  
#define _SRC_UTILITIES_HPP_  
#pragma once  

#include <stdio.h>  
#include <string>  
#include <cstdlib>  
#include <algorithm>  
#include <sstream>  
#include <iomanip>  
#include <cmath>  

#ifndef MIN  
#define MIN(x,y) (((x) < (y)) ? (x) : (y))  
#endif  

#ifndef MAX  
#define MAX(x,y) (((x) > (y)) ? (x) : (y))  
#endif  

#ifndef CLAMP  
#define CLAMP(val,min,max) ((val) < (min) ? (min) : ((val > max) ? (max) : (val)))  
#endif  

#ifndef ABS  
#define ABS(x) (((x) < 0) ? -(x) : (x))  
#endif  

namespace AKL {  
namespace Utilities {  
	template<class T, class BoolFunction>  
	void Remove(std::vector<T>& values, BoolFunction shouldErase) {  
		values.erase(remove_if(values.begin(), values.end(), shouldErase), values.end());  
	}  

	template<class T, class BoolFunction>  
	void Sort(std::vector<T>& values, BoolFunction compare) {  
		sort(values.begin(), values.end(), compare);  
	}  
}  

namespace MathUtilities {  
	template<typename Type>  
	typename std::enable_if<std::is_floating_point<Type>::value, bool>::type IsFloatEqual(const Type& a, const Type& b)  
	{  
		return std::abs(a - b) <= std::numeric_limits<Type>::epsilon() * std::abs(a);  
	}  
}  

namespace StringUtilities {  
	template<typename T>  
	T FromString(const std::string& str)  
	{  
		std::istringstream iss(str);  
		T value;  
		iss >> value;  
		return value;  
	}  

	template<typename T>  
	std::string ToString(const T& value)  
	{  
		std::ostringstream oss;  
		oss << value;  
		return oss.str();  
	}  

	// Like sprintf "%4f" format, in this example precision=4  
	template <class T>  
	std::string ToString(const T& value, int precision) {  
		std::ostringstream out;  
		out << std::fixed << std::setprecision(precision) << value;  
		return out.str();  
	}  

	/// Like sprintf "% 4d" or "% 4f" format, in this example width=4, fill=' '  
	template <class T>  
	std::string ToString(const T& value, int width, char fill) {  
		std::ostringstream out;  
		out << std::fixed << std::setfill(fill) << std::setw(width) << value;  
		return out.str();  
	}  

	/// Like sprintf "%04.2d" or "%04.2f" format, in this example precision=2, width=4, fill='0'  
	template <class T>  
	std::string ToString(const T& value, int precision, int width, char fill) {  
		std::ostringstream out;  
		out << std::fixed << std::setfill(fill) << std::setw(width) << std::setprecision(precision) << value;  
		return out.str();  
	}  

	std::vector<std::string> SplitString(const std::string& source, const std::string& delimiter, bool ignoreEmpty = false, bool trim = false);  
	std::string JoinString(const std::vector<std::string>& stringElements, const std::string& delimiter);  
	void StringReplace(std::string& input, const std::string& searchStr, const std::string& replaceStr);  

	bool IsStringInString(const std::string& haystack, const std::string& needle);  
	bool IsStringEndsWith(std::string const& value, std::string const& ending);  
	bool IsStringStartsWith(std::string const& value, std::string const& starting);  

	// Check how many times a string contains another string.  
	std::size_t StringTimesInString(const std::string& haystack, const std::string& needle);  
	bool StringIsDigits(const std::string& str);  

	std::string TrimFront(const std::string& src);  
	std::string TrimBack(const std::string& src);  
	std::string Trim(const std::string& src);  

	std::string VAArgsToString(const char* format, ...);  
	std::string VAArgsToString(const char* format, va_list args);  

	int ToInt(const std::string& intString);  
	float ToFloat(const std::string& floatString);  
}  
}  

#endif // !_SRC_UTILITIES_HPP_
