#include "APMBasic.h"
#include "ClientBasicInfo.h"
#include <string>


namespace APMReport
{
	int User::SetUserInfo(const char* sUserID, const char* sUserName, const char* sUserAccount)
	{
		if (nullptr == sUserID || sUserID == "")
		{
			return ERROR_CODE_PARAMS;
		}
		std::string userID(sUserID);
		g_userInfo.m_sUserID = userID;

		if (nullptr != sUserName)
		{
			std::string userName(sUserName);
			if (userName.length() > 1)
			{
				g_userInfo.m_sUserName = userName;
			}
		}
		if (nullptr != sUserAccount)
		{
			std::string userAccount(sUserAccount);
			if (userAccount.length() > 1)
			{
				g_userInfo.m_sUserAccount = userAccount;
			}
		}
		return 0;
	}

	UserInfo User::GetUserInfo()
	{
		return g_userInfo;
	}

	std::string Client::GetDeviceUUID()
	{
		return g_deviceUUID;
	}

	void Client::SetDeviceUUID(std::string uuid)
	{
		g_deviceUUID = uuid;
	}

	std::map<std::string, std::string> Client::GetBaseInfoMap()
	{
		return g_mapAppBaseInfoMD5;
	}

	std::string Client::GetBaseInfo(std::string appID)
	{
		if (appID.empty())
		{
			return "";
		}
		auto iter = g_mapAppBaseInfoMD5.find(appID);
		if (iter == g_mapAppBaseInfoMD5.end())
		{
			return "";
		}
		return iter->second;
	}

	void Client::SetBaseInfo(std::string appID, std::string baseInfoMD5)
	{
		if (appID.empty())
		{
			return;
		}
		g_mapAppBaseInfoMD5.insert_or_assign(appID, baseInfoMD5);
	}
}