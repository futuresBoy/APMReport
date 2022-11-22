#include "APMBasic.h"
#include "ClientManager.h"
#include <string>
#include "json/json.h"


namespace APMReport
{

	std::string ClientManager::GetDeviceUUID()
	{
		return g_deviceUUID;
	}

	void ClientManager::SetDeviceUUID(std::string uuid)
	{
		g_deviceUUID = uuid;
	}

	std::map<std::string, std::string> ClientManager::GetBaseInfoMap()
	{
		return g_mapAppBaseInfoMD5;
	}

	std::string ClientManager::GetBaseInfo(std::string appID)
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

	void ClientManager::SetBaseInfo(std::string appID, std::string baseInfoMD5)
	{
		if (appID.empty())
		{
			return;
		}
		g_mapAppBaseInfoMD5.insert_or_assign(appID, baseInfoMD5);
	}
}