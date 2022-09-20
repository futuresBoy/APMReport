#include "Logger.h"
#include "ReportManager.h"
#include "Util.h"

namespace APMReport
{
	static TaskManager g_manager;

	TaskManager& APMReport::TaskManager::GetInstance()
	{
		return g_manager;
	}

	APMReport::TaskManager::~TaskManager()
	{
	}

	/*��ʼ��*/
	int APMReport::TaskManager::APMInit(PostErrorLogFunc funcPostErrorInfo, LogFunc funcLog)
	{
		if (nullptr != funcPostErrorInfo)
		{
			m_funcPostErrorInfo = funcPostErrorInfo;
		}
		if (m_bInited)
		{
			LOGWARN("Inited Aready!");
			return 0;
		}
		m_bInited = true;
		LOGINFO("Init finished!");
		return 0;
	}

	/*���ؿ���*/
	int TaskManager::LoadSwitch(const char* msg)
	{

		try
		{
			Json::Value data;
			int result = TaskManager::GetResponseData(msg, data);
			if (result != 0)
			{
				return result;
			}
			std::lock_guard<std::recursive_mutex> lck(m_configMutex);

		}
		catch (const std::exception&)
		{
			LOGERROR("Load switch config failed!");
			return -1;
		}
		return 0;
	}

	/*��ȡ��HTTP����Ӧ����*/
	int TaskManager::GetResponseData(const char* msg, Json::Value& data)
	{
		Json::Value root;
		Json::Reader reader;
		if (!reader.parse(msg, root))
		{
			LOGERROR("Parse ResponseData to json failed!");
			return -1;
		}

		int statusCode = root["status_code"].asInt();
		auto statusMsg = root["status_msg"].asCString();
		if (statusCode != 0)
		{
			LOGERROR("ResponseData is error,status_msg: %s", statusMsg);
			return -1;
		}
		data = root["data"];
		return 0;
	}

	/*������ֵ����*/
	int TaskManager::LoadConfig(const char* msg)
	{
		try
		{
			Json::Value data;
			int result = GetResponseData(msg, data);
			if (result != 0)
			{
				return result;
			}
			//���ó����RSA��Կ
			std::string pubKeyID = data["pub_key_id"].asString();
			std::string pubKey = data["pub_key"].asString();
			result = APMReport::Util::SetRSAPubKey(pubKeyID.c_str(), pubKey.c_str());
			if (result != 0)
			{
				return result;
			}
		}
		catch (const std::exception & e)
		{
			LOGERROR(e.what());
			return -1;
		}
		return 0;
	}

	bool APMReport::Task::LoadConfig(const Json::Value& root)
	{
		return false;
	}

	bool APMReport::Task::TaskRun(const Json::Value& root)
	{
		return false;
	}

	bool APMReport::ReportErrorTask::LoadConfig(const Json::Value& root)
	{
		return false;
	}

	bool APMReport::ReportPerformanceTask::LoadConfig(const Json::Value& root)
	{
		return false;
	}

}