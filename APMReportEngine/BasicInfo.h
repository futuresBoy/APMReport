#pragma once

#include <map>
#include <string>

/*处理客户端或用户基础信息*/
namespace APMReport
{
	struct UserInfo
	{
		std::string m_sUserID;
		std::string m_sUserName;
		std::string m_sUserAccount;
	};

	//当前设备uuid
	static std::string g_deviceUUID;
	//各appID对应的基础信息
	static std::map<std::string, std::string> g_mapAppBaseInfoMD5;

	//当前用户信息
	static UserInfo g_userInfo;

	/*用户信息*/
	class User
	{
	public:
		/*设置用户基础信息*/
		static int SetUserInfo(const char* sUserID, const char* sUserName, const char* sUserAccount);
		/*获取用户基础信息*/
		static UserInfo GetUserInfo();
	};
}


