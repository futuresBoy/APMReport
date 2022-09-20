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
		//�ϱ��쳣����
		TaskType_ReportError = 1,
		//�ϱ���������
		TaskType_ReportPerformance = 2
	};

	//��ֵ������Ϣ
	class ReportConfig
	{
	public:
		//�����������
		int m_nCacheMaxSize = 100;
		//�Ƿ���������
		int m_bSendImmediately = false;
		//���ͼ����s��
		int m_nSendInterval = 10;
		//������ֵ������ﵽ�������ͣ�
		int m_nSendCache = 50;
		//����
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

	//�ϱ�������������
	class ReportErrorTask :public Task
	{
	public:
		ReportErrorTask() { m_taskType = TaskType_ReportError; };
	public:
		virtual bool LoadConfig(const Json::Value& root);
	};

	//�ϱ�������������
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
		TaskManager() : m_funcPostErrorInfo(nullptr) {};
		virtual ~TaskManager();
		static TaskManager& GetInstance();
	public:
		int APMInit(
			PostErrorLogFunc funcPostErrorInfo,		//֪ͨ�ϴ�������Ϣ
			LogFunc funcLog							//��־��ӡ����
		);
	public:
		PostErrorLogFunc m_funcPostErrorInfo;
	public:
		//���ؿ���
		int LoadSwitch(const char* msg);
		//������ֵ����
		int LoadConfig(const char* msg);
		int GetResponseData(const char* msg, Json::Value& data);
	private:
		bool m_bInited = false;
		std::recursive_mutex m_configMutex;
	};


}
