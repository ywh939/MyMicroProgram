#include "LogCounterMgr.h"

#include <fstream>
#include <sstream>
#include <algorithm>

//#include <sys/types.h>
//#include <dirent.h>
//#include <unistd.h>


#define START_OF_KEY '['
#define END_OF_KEY ']'
#define COUNTER_DICT_DIR "./logCounter.txt"


struct CmpByValue
{
	bool operator()(const std::pair<std::string, int> lhs, const std::pair<std::string, int> rhs)
	{
		return lhs.second > rhs.second;
	}
};

bool LogCounterMgr::GenCounterDict()
{
	std::ofstream ofs;
	ofs.open(COUNTER_DICT_DIR);
	if (!ofs.good())
	{
		return false;
	}

	std::vector<std::pair<std::string, int>> tmp;
	for (auto elem : m_LogCounter)
	{
		tmp.push_back(std::make_pair(elem.first, elem.second));
	}

	std::sort(tmp.begin(), tmp.end(), CmpByValue());

	for (auto elem : tmp)
	{
		std::ostringstream oss;
		oss << elem.second << ": " << elem.first << "\n";
		std::string line = oss.str();
		//fprintf(stderr, "%s, %d, %s\n", elem.first, elem.second, line);
		//fprintf(stderr, "%s\n", line);

		ofs.write(line.c_str(), line.size());
	}

	ofs.close();
	return true;
}

//void LogCounterMgr::OpenDir(const std::string& filedir)
//{
//	chdir(filedir.c_str());
//	char trueDir[128] = { 0 };
//	getcwd(trueDir, sizeof(trueDir));
//
//	DIR* dir = opendir(trueDir);
//	if (!dir)
//	{
//		fprintf(stderr, "Failed to open dir %s\n", filedir);
//		return;
//	}
//
//	struct dirent* entry;
//	while (entry = readdir(dir))
//	{
//		std::ostringstream oss;
//		oss << trueDir << "/" << entry->d_name;
//
//		std::string filepath = oss.str();
//		_OpenFile(filepath);
//	}
//}

bool LogCounterMgr::OpenFile(const std::string& filepath)
{
	std::ifstream ifs;
	ifs.open(filepath);

	if (!ifs.good())
	{
		return false;
	}

	std::string line;
	while (getline(ifs, line))
	{
		std::string key;
		int delimsCounter;
		size_t startPos = _GetPos(line, "[", 3, delimsCounter);
		size_t endPos = _GetPos(line, "]", 5, delimsCounter);

		if(delimsCounter < 3)
		{
			key = line.substr(startPos - 1);
		}
		else
		{
			key = line.substr(startPos, endPos - startPos + 1);
		}

		m_LogCounter[key]++;
		//fprintf(stderr, "%s, %d\n", key, m_LogCounter[key]);
	}

	ifs.close();

	return true;
}

size_t LogCounterMgr::_GetPos(const std::string& line, const std::string& delims, int hopePos, int& delimsCounter)
{
	size_t start = 0, pos; 
	int cnt = 0;

	do
	{
		pos = line.find_first_of(delims, start);

		if (pos == std::string::npos)
		{
			return start;
		}

		start = pos + 1;
		cnt++;
		delimsCounter = cnt;

		if (cnt == hopePos && line[pos] == delims[0])
		{
			return pos;
		}

	} while (start != std::string::npos);
}
