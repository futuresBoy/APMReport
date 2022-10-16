#include <thread>
#include <string>
#include <mutex>
#include "json/json.h"
#include "APMReport.h"

//对APM上报的数据进行管理
namespace APMReport
{
	//后台任务类型
	enum TaskType
	{
		TaskType_Null = 0,
		//上报异常数据
		TaskType_ReportError = 1,
		//上报性能数据
		TaskType_ReportPerformance = 2
	};

	//阈值配置信息
	struct ReportConfig
	{
		//默认配置
		ReportConfig()
		{
			m_nCacheMaxSize = 100;
			m_bSendImmediately = false;
			m_nSendMinInterval = 10;
			m_nSendMaxInterval = 180;
			m_nSendCache = 50;
			m_nRetry = 0;
		}

		//缓存最大容量
		int m_nCacheMaxSize;

		//立即上传
		bool m_bSendImmediately;

		//发送阈值条数（缓存达到条数则发送）
		int m_nSendCache;

		//发送最小时间间隔（s），预防瞬时请求数过多
		int m_nSendMinInterval;

		//发送最大时间间隔（s）
		int m_nSendMaxInterval;

		//重试
		int m_nRetry;

	};

	class Task
	{
	public:
		Task() :m_taskType(TaskType_Null) { m_bCollectSwitch = true, m_bReportSwitch = true; };
		virtual ~Task() {};
	public:
		//加载配置
		virtual bool LoadThresholdConfig(const Json::Value& root);

		/*
		功能：加载SDK开关
		参数：root 接口返回的json数据
		返回：0 关闭，1 开启
		*/
		virtual bool LoadSwitch(const Json::Value& root);

	public:
		TaskType m_taskType;
		//该任务的配置信息
		ReportConfig m_config;
		//采集开关（开启）
		bool m_bCollectSwitch;
		//上报开关（开启）
		bool m_bReportSwitch;
	};

	/*上报错误数据任务*/
	class ReportErrorTask :public Task
	{
	public:
		ReportErrorTask() { m_taskType = TaskType_ReportError; };
		~ReportErrorTask() {};
	public:
		bool LoadThresholdConfig(const Json::Value& root);

		/*功能：加载异常上报的开关*/
		bool LoadSwitch(const Json::Value& root);
	};

	/*上报性能数据任务*/
	class ReportPerformanceTask :public Task
	{
	public:
		ReportPerformanceTask() { m_taskType = TaskType_ReportPerformance; };
	};

	/*工作任务管理类*/
	class TaskManager
	{
	public:
		TaskManager() : m_funcPostErrorInfo(nullptr), m_funcReplyLog(nullptr) { m_bInited = false;m_pThread = nullptr; };
		virtual ~TaskManager();
		static TaskManager& GetInstance();
	public:
		/*1.初始化*/
		int APMInit(
			PostErrorLogFunc funcPostErrorInfo,		//通知上传错误信息
			LogFunc funcLog							//日志打印函数
		);

		/*2.加载阈值配置*/
		int LoadThresholdConfig(const char* msg);

		/*3.加载开关*/
		int LoadSwitch(const char* msg);

		/*
		功能：获取（HTTP）接口响应的数据
		入参：msg 原始的接口响应信息
		出参：data 从接口从获取到正确的data数据体
		返回：0 成功，-1 获取接口数据错误
		*/
		int GetResponseData(const char* msg, Json::Value& data);

		void AddErrorLog(const char* logMessage);

		int AddTraceLog(const std::string& traceID, const std::string& moduleName, const std::string& subName, const std::string& result, const std::string& errorCode, int monitorType, const char* msgArray, int* msgLengthArray, int arrayCount);
		//添加链路日志
		int AddTraceLog(const std::string& traceID, const std::string& moduleName, const std::string& subName, const std::string& result, const std::string& errorCode, int monitorType, const std::vector<std::string>& msgs);

	public:
		PostErrorLogFunc m_funcPostErrorInfo;
		LogFunc m_funcReplyLog;

	private:
		/*构建上报后台的日志数据*/
		void BuidLogData(const std::string& traceID, const std::string& moduleName, const std::string& subName, const std::string& result, const std::string& errorCode, int monitorType, const std::vector<std::string>& msgs);

		/*
			（异常日志/性能信息）定时执行上报
			参数：task 异常/性能信息上报任务
		*/
		void ProcessLogDataReport(Task task);

		/*
		上传日志信息
		*/
		int UploadLogMessage();

	private:
		bool m_bInited;
		//读取或修改配置及日志时锁定
		std::recursive_mutex m_reportMutex;
		std::thread* m_pThread;
		//线程退出
		volatile bool m_bThreadExit;
		std::vector<std::string> m_veclogMsgs;
	};


}
