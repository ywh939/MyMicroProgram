#ifndef _DEFINE_H_
#define _DEFINE_H_

//#define AVALON_OS_WINDOWS


#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#ifndef AVALON_OS_WINDOWS
#include <sys/time.h>
#endif


typedef char			Int8;
typedef unsigned char	UInt8;
typedef short			Int16;
typedef unsigned short	UInt16;
typedef int				Int32;
typedef unsigned int	UInt32;
typedef long long		Int64;
typedef unsigned long long	UInt64;


inline void Split(const std::string& str, std::vector<std::string>& strs, const std::string& delims)
{
	if (str.empty()) return;
	strs.clear();
	size_t start = 0, pos;
	do
	{
		pos = str.find_first_of(delims, start);
		if (pos == std::string::npos)
		{
			// Copy the rest of the string
			strs.push_back(str.substr(start));
			break;
		}
		else
		{
			// Copy up to delimiter
			strs.push_back(str.substr(start, pos - start));
			start = pos + 1;
		}
	} while (start != std::string::npos);
}


template<typename T>
inline std::string ConvertToStr(T value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

#endif