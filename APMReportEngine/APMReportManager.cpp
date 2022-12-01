#include "APMReportManager.h"

namespace APMReport
{
	std::map<std::string, TaskProcess*> APMReportManager::g_manager;

	static bool m_bInited;
	static PostErrorLogFunc m_funcPostErrorInfo;
	static PostErrorLogFunc m_funcPostPerformance;

	std::recursive_mutex m_reportMutex;

	APMReportManager::APMReportManager()
	{
		m_bInited = false;
		m_funcPostErrorInfo = nullptr;
		m_funcPostPerformance = nullptr;
	}

	APMReportManager::~APMReportManager()
	{
		m_bInited = false;
		m_funcPostErrorInfo = nullptr;
		m_funcPostPerformance = nullptr;
	}

	bool APMReportManager::Exist(std::string appID)
	{
		std::lock_guard<std::recursive_mutex> lck(m_reportMutex);
		auto iter = g_manager.find(appID);
		return iter == g_manager.end() ? false : true;
	}

	TaskProcess* APMReportManager::Get(std::string appID)
	{
		std::lock_guard<std::recursive_mutex> lck(m_reportMutex);
		auto iter = g_manager.find(appID);
		if (iter == g_manager.end())
		{
			auto process = new TaskProcess();
			process->Init(appID, m_funcPostErrorInfo, m_funcPostPerformance);
			g_manager.insert_or_assign(appID, process);
			return process;
		}
		return iter->second;
	}

	int APMReportManager::APMInit(PostErrorLogFunc funcPostErrorInfo, PostPerformanceFunc funcPostPerformance)
	{
		if (nullptr == funcPostErrorInfo || nullptr == funcPostPerformance)
		{
			LOGERROR("PostErrorLogFunc or PostPerformanceFunc is Null.");
			return ERROR_CODE_PARAMS;
		}
		m_funcPostErrorInfo = funcPostErrorInfo;
		m_funcPostPerformance = funcPostPerformance;
		if (m_bInited)
		{
			LOGWARN("Inited Aready!");
			return 0;
		}
		m_bInited = true;

		LOGINFO("Init finished!");
		return 0;
	}

	bool APMReportManager::Close(std::string appID)
	{
		if (appID.empty())
		{
			return false;
		}
		std::lock_guard<std::recursive_mutex> lck(m_reportMutex);
		auto iter = g_manager.find(appID);
		if (iter != g_manager.end())
		{
			iter->second->Stop();
			delete iter->second;
			g_manager.erase(iter);
		}
		return true;
	}

	bool APMReportManager::Close()
	{
		std::lock_guard<std::recursive_mutex> lck(m_reportMutex);
		for (auto t : g_manager)
		{
			t.second->Stop();
			delete t.second;
		}
		g_manager.clear();
		return true;
	}
}
