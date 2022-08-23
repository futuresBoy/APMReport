#include <thread>
#include <string>
#include <mutex>
#include "json/json.h"
#include "APMReport.h"


namespace APMReport
{
	enum TaskType
	{
		TaskType_Null = 0,
		//上报异常数据
		TaskType_ReportError = 1,
		//上报性能数据
		TaskType_ReportPerformance = 2
	};

	//阈值配置信息
	class ReportConfig
	{
	public:
		//缓存最大容量
		int m_nCacheMaxSize = 100;
		//是否立即发送
		int m_bSendImmediately = false;
		//发送间隔（s）
		int m_nSendInterval = 10;
		//发送阈值（缓存达到容量发送）
		int m_nSendCache = 50;
		//重试
		int m_nRetry = 0;

	};

	class Task
	{
	public:
		Task() :m_taskType(TaskType_Null) {};
		virtual ~Task() {};
	public:
		virtual bool LoadConfig(const Json::Value& root);
		virtual bool TaskRun(const Json::Value& root);
	public:
		TaskType m_taskType = TaskType_Null;
		ReportConfig m_config;
		std::string m_strUrl;
		std::thread m_pThread;
	};

	//上报错误数据任务
	class ReportErrorTask :public Task
	{
	public:
		ReportErrorTask() { m_taskType = TaskType_ReportError; };
	public:
		virtual bool LoadConfig(const Json::Value& root);
	};

	//上报性能数据任务
	class ReportPerformanceTask :public Task
	{
	public:
		ReportPerformanceTask() { m_taskType = TaskType_ReportPerformance; };
	public:
		virtual bool LoadConfig(const Json::Value& root);
	};


	class TaskManager
	{
	public:
		TaskManager() :m_funcGetSwitch(nullptr), m_funcGetConfig(nullptr), m_funcPostClientInfo(nullptr), m_funcPostErrorInfo(nullptr), m_funcPerformanceInfo(nullptr) {};
		virtual ~TaskManager();
		static TaskManager& GetInstance();
	public:
		int APMInit(
			GetSwitchFunc funcGetSwitch,			//通知获取开关
			GetConfigFunc funcGetConfig,			//通知获取阈值配置
			PostClientInfoFunc funcPostClientInfo,	//通知上传设备基础信息
			PostErrorInfoFunc funcPostErrorInfo,		//通知上传错误信息
			PostPerformanceInfoFunc funcPerfInfo,	//通知上传性能信息
			LogFunc funcLog							//日志打印函数
		);
	public:
		GetSwitchFunc m_funcGetSwitch;
		GetConfigFunc m_funcGetConfig;
		PostClientInfoFunc m_funcPostClientInfo;
		PostErrorInfoFunc m_funcPostErrorInfo;
		PostPerformanceInfoFunc m_funcPerformanceInfo;
	public:
		//加载开关
		bool LoadSwitch(const char* msg);
		//加载阈值配置
		bool LoadConfig(const char* msg);
	private:
		bool m_bInited = false;
		std::recursive_mutex m_configMutex;
	};


}
