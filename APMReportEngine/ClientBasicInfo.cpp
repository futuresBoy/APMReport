#include "APMBasic.h"
#include "ClientBasicInfo.h"
#include <string>
#include "json/json.h"


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

	int User::SetUserInfoEx(const char* msg)
	{
		if (msg == nullptr)
		{
			return ERROR_CODE_PARAMS;
		}
		Json::Value root;
		Json::Reader reader;
		if (!reader.parse(msg, root))
		{
			return ERROR_CODE_DATA_JSON;
		}
		g_jsonUserInfo = root;
		return 0;
	}

	Json::Value User::GetUserInfoEx()
	{
		return g_jsonUserInfo;
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