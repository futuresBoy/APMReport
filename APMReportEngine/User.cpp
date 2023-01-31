#include "User.h"
#include "APMBasic.h"
#include "Logger.h"
#include <string>
#include "json/json.h"


namespace APMReport
{
	int User::SetUserInfo(const char* sAppID, const char* sUserID, const char* sUserName, const char* sUserAccount)
	{
		if (CHECK_ISNULLOREMPTY(sAppID) || CHECK_ISNULLOREMPTY(sUserID))
		{
			return ERROR_CODE_PARAMS;
		}
		std::string userID(sUserID);
		UserInfo user;
		user.m_sUserID = userID;

		if (nullptr != sUserName)
		{
			std::string userName(sUserName);
			if (userName.length() > 1)
			{
				user.m_sUserName = userName;
			}
		}
		if (nullptr != sUserAccount)
		{
			std::string userAccount(sUserAccount);
			if (userAccount.length() > 1)
			{
				user.m_sUserAccount = userAccount;
			}
		}
		g_mapUserInfo.insert_or_assign(sAppID, user);
		return 0;
	}

	UserInfo User::GetUserInfo(const std::string& appID)
	{
		UserInfo user;
		if (appID.empty())
		{
			return user;
		}
		auto iter = g_mapUserInfo.find(appID);
		if (iter == g_mapUserInfo.end())
		{
			return user;
		}
		return iter->second;
	}

	int User::SetUserInfoEx(const std::string& appID, const std::string& msg)
	{
		if (msg.empty())
		{
			return ERROR_CODE_PARAMS;
		}
		try
		{
			Json::Value root;
			Json::Reader reader;
			if (!reader.parse(msg, root))
			{
				return ERROR_CODE_DATA_JSON;
			}
			g_jsonUserInfo.insert_or_assign(appID, root);
		}
		catch (const std::exception& e)
		{
			LOGERROR(e.what());
			return ERROR_CODE_DATA_JSON;
		}
		return 0;
	}

	Json::Value User::GetUserInfoEx(const std::string& appID)
	{
		if (appID.empty())
		{
			return new Json::Value();
		}
		auto iter = g_jsonUserInfo.find(appID);
		if (iter == g_jsonUserInfo.end())
		{
			return new Json::Value();
		}
		return iter->second;
	}

}
