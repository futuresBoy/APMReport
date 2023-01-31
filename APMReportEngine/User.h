#pragma once

#include <map>
#include <string>
#include "json/json.h"

/*处理客户端用户信息*/
namespace APMReport
{
	struct UserInfo
	{
		std::string m_sUserID;
		std::string m_sUserName;
		std::string m_sUserAccount;
	};

	//当前用户信息
	static std::map<std::string, UserInfo> g_mapUserInfo;

	//扩展的用户信息
	static std::map<std::string, Json::Value> g_jsonUserInfo;

	/*用户信息*/
	class User
	{
	public:
		/*设置用户基础信息*/
		static int SetUserInfo(const char* sAppID, const char* sUserID, const char* sUserName, const char* sUserAccount);

		/*获取用户基础信息*/
		static UserInfo GetUserInfo(const std::string& appID);

		/*设置扩展的用户基础信息*/
		static int SetUserInfoEx(const std::string& appID, const std::string& msg);

		/*获取扩展的用户基础信息*/
		static Json::Value GetUserInfoEx(const std::string& appID);
	};
}

