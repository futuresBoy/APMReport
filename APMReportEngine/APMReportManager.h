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
			m_nSendMinInterval = 15;
			m_nSendMaxInterval = 120;
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
		/*������ֵ����*/
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
	};

	/*�ϱ�������������*/
	class ReportPerformanceTask :public Task
	{
	public:
		ReportPerformanceTask() { m_taskType = TaskType_ReportPerformance; };
	public:
		bool LoadThresholdConfig(const Json::Value& root);
	};

	/*�������������*/
	class TaskManager
	{
	public:
		TaskManager();
		virtual ~TaskManager();
		static TaskManager& GetInstance();
	public:
		/*1.��ʼ��*/
		int APMInit(
			PostErrorLogFunc funcPostErrorInfo,		//֪ͨ�ϴ�������Ϣ
			PostPerformanceFunc funcPostPerformance	//֪ͨ�ϴ��쳣��Ϣ
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

		/*����������HTTP����json�ṹ*/
		int CreateRequestJson(Json::Value& root);

		//�����·��־
		int AddTraceLog(const std::string& traceID, const std::string& moduleName, const std::string& subName, const std::string& result, const std::string& errorCode, int moduleType, const wchar_t* msg);
		//�����·��־
		int AddTraceLog(const std::string& traceID, const std::string& moduleName, const std::string& subName, const std::string& result, const std::string& errorCode, int moduleType, const std::string& msg);

		//���HTTP��־
		int AddHTTPLog(const std::string& traceID, const std::string& moduleName, const std::string& url, const std::string& errorCode, int costTime, const wchar_t* msg);

	public:
		PostErrorLogFunc m_funcPostErrorInfo;
		PostErrorLogFunc m_funcPostPerformance;
	private:
		/*�����ϱ���̨����־����*/
		int BuildLogData(const std::string& traceID, const std::string& moduleName, const std::string& subName, const std::string& result, const std::string& errorCode, int moduleType, const std::string& msg);

		/*���ģ�鶨��ת��Ϊ��Ӧ���ı������ڹ�����̨����*/
		std::string ConvertModuleText(int moduleType);

		/*
			�쳣��־��ʱִ���ϱ�
			������task �쳣��Ϣ�ϱ�����
		*/
		void ProcessErrorLogReport(ReportErrorTask& task);

		/*
			������Ϣ��ʱִ���ϱ�
			������task ������Ϣ�ϱ�����
		*/
		void ProcessPerformanceReport(ReportPerformanceTask& task);

		/*
		�ϴ�������־��Ϣ
		*/
		int UploadErrorLogData();

		/*
		�ϴ�������Ϣ
		*/
		int UploadPerformanceData();

		//ֹͣ����
		void Stop();

	private:
		bool m_bInited;
		//��ȡ���޸����ü���־ʱ����
		std::recursive_mutex m_reportMutex;
		//��̨������־�߳�
		std::thread* m_pThreadErrorLog;
		std::thread* m_pThreadPerformance;
		//�߳��˳�
		volatile bool m_bThreadExit;
		//�ռ���־����
		std::vector<Json::Value> m_veclogMsgs;
		//�ռ�URL��Ӧ���������
		std::map<std::string, int> m_mapUrls;
	};


}
