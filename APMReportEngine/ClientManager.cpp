#include "APMBasic.h"
#include "ClientManager.h"
#include <string>
#include "json/json.h"


namespace APMReport
{

	std::string ClientManager::GetDeviceUUID(const std::string& appID)
	{
		if (appID.empty())
		{
			return "";
		}
		auto iter = g_mapDeviceUUID.find(appID);
		if (iter == g_mapDeviceUUID.end())
		{
			return "";
		}
		return iter->second;
	}

	void ClientManager::SetDeviceUUID(const std::string& appID, const std::string& uuid)
	{
		if (appID.empty())
		{
			return;
		}
		g_mapDeviceUUID.insert_or_assign(appID, uuid);
	}

	std::string ClientManager::GetBaseInfo(const std::string& appID)
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

	void ClientManager::SetBaseInfo(const std::string& appID, const std::string& baseInfoMD5)
	{
		if (appID.empty())
		{
			return;
		}
		g_mapAppBaseInfoMD5.insert_or_assign(appID, baseInfoMD5);
	}
}