#ifndef _LOG_COUNTER_MGR_H_
#define _LOG_COUNTER_MGR_H_

#include <unordered_map>
#include <map>
#include <string>



class LogCounterMgr
{
public:
	bool GenCounterDict();
	bool OpenFile(const std::string& filepath);
	//void OpenDir(const std::string& filedir);

private:
	size_t _GetPos(const std::string& line, const std::string& delims, int hopePos, int& delimsCounter);

private:
	std::unordered_map<std::string, int> m_LogCounter;
};

#endif