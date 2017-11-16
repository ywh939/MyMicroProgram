#ifndef _CORRECT_ASSET_H_
#define _CORRECT_ASSET_H_

#include "Define.h"

#include "mysql/mysql.h"


enum FileType
{
	FT_INVALID,
	FT_BAN_LIST,		// 封号名单
	FT_INVALID_ASSET,	// 非法资产
};

#define LOG_FILE_PATH "./"

class CorrectAsset
{
public:
	CorrectAsset();
	~CorrectAsset();

public:
	// 初始化
	bool Init(const std::string& filePath);

	// 读取文件
	bool LoadFile(const std::string& filePath, UInt8 fileType);

	// 进行更正资产
	void OnCorrcetAsset();

private:
	// 处理信息
	void ExecuteData(const std::string& hostStr, const std::string& databaseStr, const std::string& columnStr);
	
	// 处理mysql命令
	void ExecuteCommand(const std::string& command, bool isQuery = false);

	// 获取配置属性
	std::vector<std::string> GetProperty(const std::string& key);

	// 获取配置文件中一个属性的数量
	UInt16 GetPropertyNum(const std::string& key);

	// 记录玩家资产
	void RecordPlayerAsset(const std::string& hostStr, const std::string& databaseStr, const std::string& columnStr, bool newAsset = false);

private:
	// 数据库连接
	MYSQL						m_MysqlConn;
	// 更正的玩家id
	std::vector<UInt64>			m_CorrectRoleIDs;
	// 角色id-->非法资产数量
	std::map<UInt64, UInt64>	m_RoleIdxInvalidAssetNum;
	// 配置属性
	std::map<std::string, std::vector<std::string>>	m_ConfigPropertys;

	// 角色id-->资产
	std::map<UInt64, UInt64>	m_RoleAsset;
};

#endif