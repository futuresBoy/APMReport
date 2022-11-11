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
		ReportConfig(int minInterval = 15, int maxInterval = 120, int count = 50, int maxSize = 100, bool sendImmediately = false, int retry = 0)
		{
			m_nSendMinInterval = minInterval;
			m_nSendMaxInterval = maxInterval;
			m_nSendCount = count;
			m_nCacheMaxSize = maxSize;
			m_bSendImmediately = sendImmediately;
			m_nRetry = retry;
		}

		//发送最小时间间隔（s），预防瞬时请求数过多
		int m_nSendMinInterval;

		//发送最大时间间隔（s）
		int m_nSendMaxInterval;

		//发送阈值条数（缓存达到条数则发送）
		int m_nSendCount;

		//缓存最大容量
		int m_nCacheMaxSize;

		//立即上传
		bool m_bSendImmediately;

		//重试
		int m_nRetry;

	};

	class Task
	{
	public:
		Task() :m_taskType(TaskType_Null) { m_bCollectSwitch = true, m_bReportSwitch = true; };
		virtual ~Task() {};
	public:
		/*加载阈值配置*/
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
	};

	/*上报性能数据任务*/
	class ReportPerformanceTask :public Task
	{
	public:
		ReportPerformanceTask() { m_taskType = TaskType_ReportPerformance; };
	public:
		bool LoadThresholdConfig(const Json::Value& root);
	};

	/*工作任务管理类*/
	class TaskManager
	{
	public:
		TaskManager();
		virtual ~TaskManager();
		static TaskManager& GetInstance();
	public:
		/*1.初始化*/
		int APMInit(
			PostErrorLogFunc funcPostErrorInfo,		//通知上传错误信息
			PostPerformanceFunc funcPostPerformance	//通知上传异常信息
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

		/*创建基础的HTTP请求json结构*/
		int CreateRequestJson(Json::Value& root);

		//添加链路日志
		int AddTraceLog(const std::string& module, const std::string& logType, const std::string& bussiness, const std::string& subName, const std::string& errorCode, const std::string& msg, const std::string& extData);

		//添加HTTP日志
		int AddHTTPLog(const std::string& logType, const std::string& bussiness, const std::string& url, const std::string& errorCode, int costTime, const std::string& msg, const std::string& extData);

	public:
		PostErrorLogFunc m_funcPostErrorInfo;
		PostErrorLogFunc m_funcPostPerformance;
	private:

		/*生成基础的上报Json结构体*/
		int GenerateRoot(const std::string& msg, Json::Value& root);

		/*构建上报后台的日志数据*/
		int BuildLogData(const std::string& module, const std::string& logType, const std::string& bussiness, const std::string& subName, const std::string& errorCode, const std::string& msg, const std::string& extData);

		/*监控模块定义转换为对应的文本，用于构建后台索引*/
		std::string ConvertModuleText(int moduleType);

		/*
			异常日志定时执行上报
			参数：task 异常信息上报任务
		*/
		void ProcessErrorLogReport(ReportErrorTask& task);

		/*
			性能信息定时执行上报
			参数：task 性能信息上报任务
		*/
		void ProcessPerformanceReport(ReportPerformanceTask& task);

		/*
		上传错误日志信息
		*/
		int UploadErrorLogData();

		/*
		上传性能信息
		*/
		int UploadPerformanceData();

		//停止运行
		void Stop();

	private:
		bool m_bInited;
		//读取或修改配置及日志时锁定
		std::recursive_mutex m_reportMutex;
		//后台处理日志线程
		std::thread* m_pThreadErrorLog;
		std::thread* m_pThreadPerformance;
		//线程退出
		volatile bool m_bThreadExit;
		//收集日志数组
		std::vector<Json::Value> m_veclogMsgs;
		//收集URL对应的请求次数
		std::map<std::string, int> m_mapUrls;
	};


}
