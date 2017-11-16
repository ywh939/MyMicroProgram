#include "CorrectAsset.h"

CorrectAsset::CorrectAsset()
{
}

CorrectAsset::~CorrectAsset()
{
}

bool CorrectAsset::Init(const std::string& filePath)
{
	std::ifstream ifs;
	ifs.open(filePath);

	if (!ifs.good())
	{
		std::cout << "Failed to open file(" << filePath << ")!" << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(ifs, line))
	{
		std::vector<std::string> params;
		Split(line, params, "=");
		if (params.size() != 2)
		{
			std::cout << "Config is error!" << std::endl;
			return false;
		}

		std::string hostsStr = params[1];
		std::vector<std::string> params2;
		Split(hostsStr, params2, ",");

		for (auto elem : params2)
		{
			m_ConfigPropertys[params[0]].push_back(elem);
		}
	}

	UInt16 hostNum = GetPropertyNum("host");
	UInt16 userNum = GetPropertyNum("user");
	UInt16 passwdNum = GetPropertyNum("passwd");
	UInt16 databaseNum = GetPropertyNum("database");
	UInt16 columnNum = GetPropertyNum("column");

	if (hostNum != passwdNum || hostNum != passwdNum || passwdNum != databaseNum)
	{
		std::cout << "Config property is error!" << std::endl;
		return false;
	}

	if (userNum != 1 || columnNum != 1)
	{
		std::cout << "Config property is error!" << std::endl;
		return false;
	}

	return true;
}

bool CorrectAsset::LoadFile(const std::string& filePath, UInt8 fileType)
{
	std::ifstream ifs;
	ifs.open(filePath);

	if (!ifs.good())
	{
		std::cout << "Failed to open file(" << filePath << ")!" << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(ifs, line))
	{
		std::vector<std::string> params;
		Split(line, params, ",");

		if (fileType == FT_BAN_LIST && params.size() != 2)
		{
			std::cout << "Line(" << line << ")'s format is error!(size: " << params.size() << ")" << std::endl;
			return false;
		}

		if (fileType == FT_INVALID_ASSET && params.size() != 3)
		{
			std::cout << "Line(" << line << ")'s format is error!(size: " << params.size() << ")" << std::endl;
			return false;
		}

		//UInt16 zone = (UInt16)atoi(params[0].c_str());
		UInt64 roleId = atoll(params[1].c_str());

		if (fileType == FT_BAN_LIST)
		{
			m_CorrectRoleIDs.push_back(roleId);
		}
		else if (fileType == FT_INVALID_ASSET)
		{
			UInt64 num = atoll(params[2].c_str());
			m_RoleIdxInvalidAssetNum[roleId] += num;
		}
	}

	ifs.close();

	return true;
}

void CorrectAsset::OnCorrcetAsset()
{
	std::vector<std::string> hostVec = GetProperty("host");
	std::vector<std::string> userVec = GetProperty("user");
	std::vector<std::string> passwdVec = GetProperty("passwd");
	std::vector<std::string> databaseVec = GetProperty("database");
	std::vector<std::string> columnVec = GetProperty("column");

	std::string userStr = userVec[0];
	std::string columnStr = columnVec[0];

	const char * user = userStr.c_str();

	for (size_t idx = 0; idx != hostVec.size(); ++idx)
	{
		mysql_init(&m_MysqlConn);

		std::string hostStr = hostVec[idx];
		std::string passwordStr = passwdVec[idx];
		std::string databaseStr = databaseVec[idx];
		const char * host = hostStr.c_str();
		const char * password = passwordStr.c_str();
		const char * database = databaseStr.c_str();

		if (!mysql_real_connect(&m_MysqlConn, host, user, password, database, 0, NULL, 0))
		{
			std::cout << "Error connecting to database:" << mysql_error(&m_MysqlConn) << std::endl;
			continue;
		}

		ExecuteData(hostStr, databaseStr, columnStr);
		
		mysql_close(&m_MysqlConn);
	}
}

void CorrectAsset::ExecuteData(const std::string& hostStr, const std::string& databaseStr, const std::string& columnStr)
{
	for (auto elem : m_RoleIdxInvalidAssetNum)
	{
		UInt64 roleid = elem.first;
		std::string selectCommand = "select guid, " + columnStr + " from t_player_info where guid=" + ConvertToStr(roleid);
		ExecuteCommand(selectCommand, true);
	}

	if (m_RoleAsset.empty())
	{
		std::cout << "Select t_player_info failed!" << std::endl;
		return;
	}
	RecordPlayerAsset(hostStr, databaseStr, columnStr);

	for (auto& elem : m_RoleAsset)
	{
		UInt64 roleid = elem.first;
		UInt64& asset = elem.second;

		auto iter = m_RoleIdxInvalidAssetNum.find(roleid);
		if (iter == m_RoleIdxInvalidAssetNum.end())
		{
			continue;
		}

		UInt64 invalidAsset = iter->second;
		Int64 remainAsset = asset - invalidAsset;
		if (remainAsset > 0)
		{
			asset = remainAsset;
		}
		else
		{
			asset = 0;
		}

		std::string updateCommand = "update t_player_info set " + columnStr + "=" + ConvertToStr(asset) + " where guid=" + ConvertToStr(roleid);
		ExecuteCommand(updateCommand);
	}

	RecordPlayerAsset(hostStr, databaseStr, columnStr, true);
	m_RoleAsset.clear();
}

void CorrectAsset::RecordPlayerAsset(const std::string& hostStr, const std::string& databaseStr, const std::string& columnStr, bool newAsset)
{
	system("mkdir \"AssetLog\"");

	std::string preStr;
	if (newAsset)
	{
		preStr = "/NewAsset";
	}
	else
	{
		preStr = "/OldAsset";
	}
	std::string filePath = "./AssetLog" + preStr + "_" + hostStr + "_" + databaseStr + "_" + columnStr + ".log";

	std::ofstream ofs(filePath, std::ios::app | std::ios::out);
	if (!ofs)
	{
		std::cout << "Ofstream file(" << filePath << ") error!" << std::endl;
		return;
	}

	for (auto elem : m_RoleAsset)
	{
		std::ostringstream oss;
		oss << elem.first << " : " << elem.second << "\n";
		std::string line = oss.str();
		ofs.write(line.c_str(), line.size());
	}

	ofs.close();
}

void CorrectAsset::ExecuteCommand(const std::string& command, bool isQuery)
{
	const char * query = command.c_str();
	int ret = mysql_query(&m_MysqlConn, query);
	if (ret)
	{
		return;
	}

	if (!isQuery)
	{
		return;
	}

	MYSQL_RES * res;
	MYSQL_ROW row;
	res = mysql_use_result(&m_MysqlConn);
	if (res)
	{
		while ((row = mysql_fetch_row(res)) != NULL)
		{
			std::string roleidStr = row[0];
			std::string assetStr = row[1];
			UInt64 roleid = atoll(roleidStr.c_str());
			UInt64 asset = atoll(assetStr.c_str());

			m_RoleAsset[roleid] = asset;
		}
	}
	mysql_free_result(res);
}

std::vector<std::string> CorrectAsset::GetProperty(const std::string& key)
{
	auto iter = m_ConfigPropertys.find(key);
	if (iter != m_ConfigPropertys.end())
	{
		return iter->second;
	}

	std::vector<std::string> tmp;
	return tmp;
}

UInt16 CorrectAsset::GetPropertyNum(const std::string& key)
{
	auto iter = m_ConfigPropertys.find(key);
	if (iter == m_ConfigPropertys.end())
	{
		std::cout << "Load config error!" << std::endl;
		return 0;
	}
	return iter->second.size();
}