#include <KT01/Core/PerformanceCounter.h>
#include <iostream>




std::ostream& operator<< (std::ostream& os, const PerformanceCounter::Count& value)
{

	os << '[' << value.tv_sec << '.' << value.tv_nsec << ']';

	return os;
}