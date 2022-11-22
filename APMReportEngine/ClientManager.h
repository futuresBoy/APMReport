#pragma once

#include <map>
#include <string>
#include "json/json.h"

/*处理客户端或用户基础信息*/
namespace APMReport
{
	//当前设备uuid
	static std::string g_deviceUUID;
	//各appID对应的基础信息
	static std::map<std::string, std::string> g_mapAppBaseInfoMD5;

	/*用户客户端信息*/
	class ClientManager
	{
	public:
		/*获取客户端设备UUID*/
		static std::string GetDeviceUUID();
		static void SetDeviceUUID(std::string uuid);
		/*获取客户端基础信息字典*/
		static std::map<std::string, std::string> GetBaseInfoMap();
		/*获取指定appID的客户端基础信息*/
		static std::string GetBaseInfo(std::string appID);
		static void SetBaseInfo(std::string appID, std::string baseInfoMD5);
	};
}


