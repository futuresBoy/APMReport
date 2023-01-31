#pragma once

#include <map>
#include <string>
#include "json/json.h"

/*����ͻ��˻��û�������Ϣ*/
namespace APMReport
{
	//��appID��Ӧ���豸uuid
	static std::map<std::string, std::string> g_mapDeviceUUID;
	//��appID��Ӧ�Ļ�����Ϣ
	static std::map<std::string, std::string> g_mapAppBaseInfoMD5;

	/*�û��ͻ�����Ϣ*/
	class ClientManager
	{
	public:
		/*��ȡ�ͻ����豸UUID*/
		static std::string GetDeviceUUID(const std::string& appID);

		/*���ÿͻ����豸UUID*/
		static void SetDeviceUUID(const std::string& appID, const std::string& uuid);

		/*��ȡָ��appID�Ŀͻ��˻�����Ϣ*/
		static std::string GetBaseInfo(const std::string& appID);

		/*����ָ��appID�Ŀͻ��˻�����Ϣ*/
		static void SetBaseInfo(const std::string& appID, const std::string& baseInfoMD5);
	};
}


