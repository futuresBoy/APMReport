#include "Logger.h"
#include "ReportManager.h"


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

	/*初始化*/
	int APMReport::TaskManager::APMInit(GetSwitchFunc funcGetSwitch, GetConfigFunc funcGetConfig, PostErrorLogFunc funcPostErrorInfo, LogFunc funcLog)
	{
		if (nullptr == funcGetSwitch || nullptr == funcGetConfig || nullptr == funcPostErrorInfo)
		{
			LOGERROR("Required parameter error!");
			return -1;
		}
		if (m_bInited)
		{
			LOGWARN("Inited Aready!");
			return 0;
		}
		m_funcGetSwitch = funcGetSwitch;
		m_funcGetConfig = funcGetConfig;
		m_funcPostErrorInfo = funcPostErrorInfo;
		m_bInited = true;
		LOGINFO("Init finished!");
		return 0;
	}

	/*加载开关*/
	bool TaskManager::LoadSwitch(const char* msg)
	{
		Json::Value root;
		Json::Reader reader;
		try
		{
			if (!reader.parse(msg, root))
			{
				LOGERROR("Parse switch config json failed!");
			}

			int statusCode = root["status_code"].asInt();
			auto statusMsg = root["status_msg"].asCString();
			if (statusCode != 0)
			{
				LOGERROR("Get switch config api error: %s", statusMsg);
				return statusCode;
			}
			
			std::lock_guard<std::recursive_mutex> lck(m_configMutex);


		}
		catch (const std::exception&)
		{
			LOGERROR("Load switch config json failed!");
		}


		return false;
	}

	/*加载阈值配置*/
	bool TaskManager::LoadConfig(const char* msg)
	{
		return false;
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