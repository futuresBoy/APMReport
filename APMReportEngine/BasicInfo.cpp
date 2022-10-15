#include "APMBasic.h"
#include "BasicInfo.h"
#include <string>


namespace APMReport
{
	UserInfo g_userInfo;

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
		if (g_userInfo == nullptr)
		{
			return UserInfo();
		}
		return g_userInfo;
	}
}