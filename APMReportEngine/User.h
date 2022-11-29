#pragma once

#include <map>
#include <string>
#include "json/json.h"

/*����ͻ����û���Ϣ*/
namespace APMReport
{
	struct UserInfo
	{
		std::string m_sUserID;
		std::string m_sUserName;
		std::string m_sUserAccount;
	};

	//��ǰ�û���Ϣ
	static std::map<std::string, UserInfo> g_mapUserInfo;

	//��չ���û���Ϣ
	static std::map<std::string, Json::Value> g_jsonUserInfo;

	/*�û���Ϣ*/
	class User
	{
	public:
		/*�����û�������Ϣ*/
		static int SetUserInfo(const char* sAppID, const char* sUserID, const char* sUserName, const char* sUserAccount);
		/*��ȡ�û�������Ϣ*/
		static UserInfo GetUserInfo(std::string appID);
		/*������չ���û�������Ϣ*/
		static int SetUserInfoEx(std::string appID, std::string msg);
		static Json::Value GetUserInfoEx(std::string appID);
	};
}

