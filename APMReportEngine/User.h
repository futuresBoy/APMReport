#pragma once

#include <map>
#include <string>
#include "json/json.h"

/*����ͻ����û���Ϣ*/
namespace APMReport
{
	struct UserInfo
	{
		std::string m_appID;
		std::string m_sUserID;
		std::string m_sUserName;
		std::string m_sUserAccount;
	};

	//��ǰ�û���Ϣ
	static UserInfo g_userInfo;

	//��չ���û���Ϣ
	static Json::Value g_jsonUserInfo;

	/*�û���Ϣ*/
	class User
	{
	public:
		/*�����û�������Ϣ*/
		static int SetUserInfo(const char* sUserID, const char* sUserName, const char* sUserAccount);
		/*��ȡ�û�������Ϣ*/
		static UserInfo GetUserInfo();
		/*������չ���û�������Ϣ*/
		static int SetUserInfoEx(std::string msg);
		static Json::Value GetUserInfoEx();
	};
}

