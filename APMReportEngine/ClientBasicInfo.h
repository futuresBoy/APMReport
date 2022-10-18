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

	//当前用户信息
	static UserInfo g_userInfo;
	//当前设备uuid
	static std::string g_deviceUUID;
	//各appID对应的基础信息
	static std::map<std::string, std::string> g_mapAppBaseInfoMD5;

	/*用户信息*/
	class User
	{
	public:
		/*设置用户基础信息*/
		static int SetUserInfo(const char* sUserID, const char* sUserName, const char* sUserAccount);
		/*获取用户基础信息*/
		static UserInfo GetUserInfo();
	};

	/*设备信息*/
	class Client
	{
	public:
		/*获取客户端设备UUID*/
		static std::string GetDeviceUUID();
		static void SetDeviceUUID(std::string uuid);
		/*获取客户端基础信息字典*/
		static std::map<std::string, std::string> GetBaseInfoMap();
		/*获取指定appID的客户端基础信息*/
		static std::string GetBaseInfo(std::string appID);
		static void SetBaseInfo(std::string appID, std::string baseInfoMD5);
	};
}


