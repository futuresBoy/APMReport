#include "User.h"
#include "APMBasic.h"
#include "Logger.h"
#include <string>
#include "json/json.h"


namespace APMReport
{
	int User::SetUserInfo(const char* sAppID, const char* sUserID, const char* sUserName, const char* sUserAccount)
	{
		if (CHECK_ISNULLOREMPTY(sUserID))
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

	int User::SetUserInfoEx(std::string msg)
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
			g_jsonUserInfo = root;
		}
		catch (const std::exception& e)
		{
			LOGERROR(e.what());
			return ERROR_CODE_DATA_JSON;
		}
		return 0;
	}

	Json::Value User::GetUserInfoEx()
	{
		return g_jsonUserInfo;
	}

}
