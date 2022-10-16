#include <thread>
#include <string>
#include <mutex>
#include "json/json.h"
#include "APMReport.h"

//��APM�ϱ������ݽ��й���
namespace APMReport
{
	//��̨��������
	enum TaskType
	{
		TaskType_Null = 0,
		//�ϱ��쳣����
		TaskType_ReportError = 1,
		//�ϱ���������
		TaskType_ReportPerformance = 2
	};

	//��ֵ������Ϣ
	struct ReportConfig
	{
		//Ĭ������
		ReportConfig()
		{
			m_nCacheMaxSize = 100;
			m_bSendImmediately = false;
			m_nSendMinInterval = 10;
			m_nSendMaxInterval = 180;
			m_nSendCache = 50;
			m_nRetry = 0;
		}

		//�����������
		int m_nCacheMaxSize;

		//�����ϴ�
		bool m_bSendImmediately;

		//������ֵ����������ﵽ�������ͣ�
		int m_nSendCache;

		//������Сʱ������s����Ԥ��˲ʱ����������
		int m_nSendMinInterval;

		//�������ʱ������s��
		int m_nSendMaxInterval;

		//����
		int m_nRetry;

	};

	class Task
	{
	public:
		Task() :m_taskType(TaskType_Null) { m_bCollectSwitch = true, m_bReportSwitch = true; };
		virtual ~Task() {};
	public:
		//��������
		virtual bool LoadThresholdConfig(const Json::Value& root);

		/*
		���ܣ�����SDK����
		������root �ӿڷ��ص�json����
		���أ�0 �رգ�1 ����
		*/
		virtual bool LoadSwitch(const Json::Value& root);

	public:
		TaskType m_taskType;
		//�������������Ϣ
		ReportConfig m_config;
		//�ɼ����أ�������
		bool m_bCollectSwitch;
		//�ϱ����أ�������
		bool m_bReportSwitch;
	};

	/*�ϱ�������������*/
	class ReportErrorTask :public Task
	{
	public:
		ReportErrorTask() { m_taskType = TaskType_ReportError; };
		~ReportErrorTask() {};
	public:
		bool LoadThresholdConfig(const Json::Value& root);

		/*���ܣ������쳣�ϱ��Ŀ���*/
		bool LoadSwitch(const Json::Value& root);
	};

	/*�ϱ�������������*/
	class ReportPerformanceTask :public Task
	{
	public:
		ReportPerformanceTask() { m_taskType = TaskType_ReportPerformance; };
	};

	/*�������������*/
	class TaskManager
	{
	public:
		TaskManager() : m_funcPostErrorInfo(nullptr), m_funcReplyLog(nullptr) { m_bInited = false;m_pThread = nullptr; };
		virtual ~TaskManager();
		static TaskManager& GetInstance();
	public:
		/*1.��ʼ��*/
		int APMInit(
			PostErrorLogFunc funcPostErrorInfo,		//֪ͨ�ϴ�������Ϣ
			LogFunc funcLog							//��־��ӡ����
		);

		/*2.������ֵ����*/
		int LoadThresholdConfig(const char* msg);

		/*3.���ؿ���*/
		int LoadSwitch(const char* msg);

		/*
		���ܣ���ȡ��HTTP���ӿ���Ӧ������
		��Σ�msg ԭʼ�Ľӿ���Ӧ��Ϣ
		���Σ�data �ӽӿڴӻ�ȡ����ȷ��data������
		���أ�0 �ɹ���-1 ��ȡ�ӿ����ݴ���
		*/
		int GetResponseData(const char* msg, Json::Value& data);

		void AddErrorLog(const char* logMessage);

		int AddTraceLog(const std::string& traceID, const std::string& moduleName, const std::string& subName, const std::string& result, const std::string& errorCode, int monitorType, const char* msgArray, int* msgLengthArray, int arrayCount);
		//�����·��־
		int AddTraceLog(const std::string& traceID, const std::string& moduleName, const std::string& subName, const std::string& result, const std::string& errorCode, int monitorType, const std::vector<std::string>& msgs);

	public:
		PostErrorLogFunc m_funcPostErrorInfo;
		LogFunc m_funcReplyLog;

	private:
		/*�����ϱ���̨����־����*/
		void BuidLogData(const std::string& traceID, const std::string& moduleName, const std::string& subName, const std::string& result, const std::string& errorCode, int monitorType, const std::vector<std::string>& msgs);

		/*
			���쳣��־/������Ϣ����ʱִ���ϱ�
			������task �쳣/������Ϣ�ϱ�����
		*/
		void ProcessLogDataReport(Task task);

		/*
		�ϴ���־��Ϣ
		*/
		int UploadLogMessage();

	private:
		bool m_bInited;
		//��ȡ���޸����ü���־ʱ����
		std::recursive_mutex m_reportMutex;
		std::thread* m_pThread;
		//�߳��˳�
		volatile bool m_bThreadExit;
		std::vector<std::string> m_veclogMsgs;
	};


}
