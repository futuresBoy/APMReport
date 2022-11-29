#pragma once

#include <map>
#include <string>
#include "json/json.h"

/*����ͻ��˻��û�������Ϣ*/
namespace APMReport
{
	//��appID��Ӧ���豸uuid
	static std::map<std::string,std::string> g_mapDeviceUUID;
	//��appID��Ӧ�Ļ�����Ϣ
	static std::map<std::string, std::string> g_mapAppBaseInfoMD5;

	/*�û��ͻ�����Ϣ*/
	class ClientManager
	{
	public:
		/*��ȡ�ͻ����豸UUID*/
		static std::string GetDeviceUUID(std::string appID);
		static void SetDeviceUUID(std::string appID, std::string uuid);
		/*��ȡ�ͻ��˻�����Ϣ�ֵ�*/
		static std::map<std::string, std::string> GetBaseInfoMap();
		/*��ȡָ��appID�Ŀͻ��˻�����Ϣ*/
		static std::string GetBaseInfo(std::string appID);
		static void SetBaseInfo(std::string appID, std::string baseInfoMD5);
	};
}


