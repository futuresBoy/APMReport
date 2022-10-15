#pragma once

#include <map>
#include <string>

/*����ͻ��˻��û�������Ϣ*/
namespace APMReport
{
	//��ǰ�豸uuid
	static std::string g_deviceUUID;
	//��appID��Ӧ�Ļ�����Ϣ
	static std::map<std::string, std::string> g_mapAppBaseInfoMD5;

	struct UserInfo
	{
		std::string m_sUserID;
		std::string m_sUserName;
		std::string m_sUserAccount;
	};

	/*�û���Ϣ*/
	class User
	{
	public:
		/*�����û�������Ϣ*/
		static int SetUserInfo(const char* sUserID, const char* sUserName, const char* sUserAccount);
		static UserInfo GetUserInfo();
	private:
		//��ǰ�û���Ϣ
		static UserInfo g_userInfo;
	};
}


