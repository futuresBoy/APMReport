#pragma once

#include <map>
#include <string>
#include "json/json.h"

/*处理客户端或用户基础信息*/
namespace APMReport
{
	//各appID对应的设备uuid
	static std::map<std::string, std::string> g_mapDeviceUUID;
	//各appID对应的基础信息
	static std::map<std::string, std::string> g_mapAppBaseInfoMD5;

	/*用户客户端信息*/
	class ClientManager
	{
	public:
		/*获取客户端设备UUID*/
		static std::string GetDeviceUUID(const std::string& appID);

		/*设置客户端设备UUID*/
		static void SetDeviceUUID(const std::string& appID, const std::string& uuid);

		/*获取指定appID的客户端基础信息*/
		static std::string GetBaseInfo(const std::string& appID);

		/*设置指定appID的客户端基础信息*/
		static void SetBaseInfo(const std::string& appID, const std::string& baseInfoMD5);
	};
}


