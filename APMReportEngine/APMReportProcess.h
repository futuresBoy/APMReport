#include <thread>
#include <string>
#include <mutex>
#include "json/json.h"
#include "APMReport.h"

//��APM�ϱ������ݽ��й���
namespace APMReport
{
	/*
		��̨��������
	*/
	enum TaskType
	{
		TaskType_Null = 0,
		//�ϱ��쳣����
		TaskType_ReportError = 1,
		//�ϱ���������
		TaskType_ReportPerformance = 2
	};

	/*
		��ֵ������Ϣ
	*/
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

		//������Сʱ������s����Ԥ��˲ʱ����������
		int m_nSendMinInterval;

		//�������ʱ������s��
		int m_nSendMaxInterval;

		//������ֵ����������ﵽ�������ͣ�
		int m_nSendCount;

		//�����������
		int m_nCacheMaxSize;

		//�����ϴ�
		bool m_bSendImmediately;

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

	/*
		�ϱ�������������
	*/
	class ReportErrorTask :public Task
	{
	public:
		ReportErrorTask() { m_taskType = TaskType_ReportError; };
		~ReportErrorTask() {};
	public:
		bool LoadThresholdConfig(const Json::Value& root);
	};

	/*
		�ϱ�������������
	*/
	class ReportPerformanceTask :public Task
	{
	public:
		ReportPerformanceTask() { m_taskType = TaskType_ReportPerformance; };
	public:
		bool LoadThresholdConfig(const Json::Value& root);
	};

	/*
		������־�ϱ������������쳣��־�ϱ���������־�ϱ�
	*/
	class TaskProcess
	{
	public:
		TaskProcess();
		virtual ~TaskProcess();
		TaskProcess& operator=(const TaskProcess& val)
		{
			return *this;
		}
	public:
		/*1.��ʼ����־�ϱ�*/
		int Init(std::string appID, PostErrorLogFunc funcPostErrorInfo, PostPerformanceFunc funcPostPerformanceInfo);

		/*2.������ֵ����*/
		int LoadThresholdConfig(const char* msg);

		/*3.���ؿ���*/
		int LoadSwitch(const char* msg);

		//�����·��־
		int AddTraceLog(const std::string& module, const std::string& logType, const std::string& bussiness, const std::string& subName, const std::string& errorCode, const std::string& msg, const std::string& extData);

		//���HTTP��־
		int AddHTTPLog(const std::string& logType, const std::string& bussiness, const std::string& url, const std::string& errorCode, int costTime, const std::string& msg, const std::string& extData);

		//ֹͣ����
		void Stop();

	private:

		/*
		���ܣ���ȡ��HTTP���ӿ���Ӧ������
		��Σ�msg ԭʼ�Ľӿ���Ӧ��Ϣ
		���Σ�data �ӽӿڴӻ�ȡ����ȷ��data������
		���أ�0 �ɹ���-1 ��ȡ�ӿ����ݴ���
		*/
		int GetResponseData(const char* msg, Json::Value& data);

		/*����������HTTP����json�ṹ*/
		int CreateRequestJson(Json::Value& root);

		/*���ɻ������ϱ�Json�ṹ��*/
		int GenerateRoot(const std::string& msg, Json::Value& root);

		/*�����ϱ���̨����־����*/
		int BuildLogData(const std::string& module, const std::string& logType, const std::string& bussiness, const std::string& subName, const std::string& errorCode, const std::string& msg, const std::string& extData);

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

	private:
		std::string m_appID;
		PostErrorLogFunc m_funcPostErrorInfo;
		PostErrorLogFunc m_funcPostPerformance;
	private:
		//��ȡ���޸����ü���־ʱ����
		std::recursive_mutex m_reportMutex;
		//���������־�߳�
		std::thread* m_pThreadErrorLog;
		//����������־�߳�
		std::thread* m_pThreadPerformance;
		//�߳��˳�
		volatile bool m_bThreadExit;
		//�ռ���־����
		std::vector<Json::Value> m_veclogMsgs;
		//�ռ�URL��Ӧ���������
		std::map<std::string, int> m_mapUrls;
	};


}
