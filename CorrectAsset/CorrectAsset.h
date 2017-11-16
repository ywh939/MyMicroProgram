#ifndef _CORRECT_ASSET_H_
#define _CORRECT_ASSET_H_

#include "Define.h"

#include "mysql/mysql.h"


enum FileType
{
	FT_INVALID,
	FT_BAN_LIST,		// �������
	FT_INVALID_ASSET,	// �Ƿ��ʲ�
};

#define LOG_FILE_PATH "./"

class CorrectAsset
{
public:
	CorrectAsset();
	~CorrectAsset();

public:
	// ��ʼ��
	bool Init(const std::string& filePath);

	// ��ȡ�ļ�
	bool LoadFile(const std::string& filePath, UInt8 fileType);

	// ���и����ʲ�
	void OnCorrcetAsset();

private:
	// ������Ϣ
	void ExecuteData(const std::string& hostStr, const std::string& databaseStr, const std::string& columnStr);
	
	// ����mysql����
	void ExecuteCommand(const std::string& command, bool isQuery = false);

	// ��ȡ��������
	std::vector<std::string> GetProperty(const std::string& key);

	// ��ȡ�����ļ���һ�����Ե�����
	UInt16 GetPropertyNum(const std::string& key);

	// ��¼����ʲ�
	void RecordPlayerAsset(const std::string& hostStr, const std::string& databaseStr, const std::string& columnStr, bool newAsset = false);

private:
	// ���ݿ�����
	MYSQL						m_MysqlConn;
	// ���������id
	std::vector<UInt64>			m_CorrectRoleIDs;
	// ��ɫid-->�Ƿ��ʲ�����
	std::map<UInt64, UInt64>	m_RoleIdxInvalidAssetNum;
	// ��������
	std::map<std::string, std::vector<std::string>>	m_ConfigPropertys;

	// ��ɫid-->�ʲ�
	std::map<UInt64, UInt64>	m_RoleAsset;
};

#endif