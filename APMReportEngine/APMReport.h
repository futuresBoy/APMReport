#ifndef __APM_REPORT_H__
#define __APM_REPORT_H__

#ifdef APMREPORTENGINE_EXPORTS 
#define APM_REPORT_API _declspec(dllexport)
#else
#define APM_REPORT_API _declspec(dllimport)
#endif 

extern "C"
{
	//������־��ӡ����
	typedef enum LogLevel
	{
		LOG_DEBUG = 0,
		LOG_INFO = 1,
		LOG_WARN = 2,
		LOG_ERROR = 3,
		LOG_FATAL = 4
	};

	//�ɼ���������
	typedef enum DataCategory
	{
		NONE = 0,
		//��������
		PERFORMANCE = 1,
		//�쳣����
		ERROR = 2
	};

	//�ɼ�ģ��
	typedef enum DataModule
	{
		//����
		Crash,
		//����ʱ��
		StartUpTime,
		//CPU���ڴ�
		CPUAndMemory,
		//����
		Caton,
		Web,
		Http,
		Tcp
	};

	/*��ȡ��������*/
	typedef int (*GetSwitchFunc)(const char* url);

	/*��ȡ��ֵ����*/
	typedef int (*GetConfigFunc)(const char* url);

	/*�ϴ�������Ϣ*/
	typedef int (*PostClientInfoFunc)(const char* msg, unsigned int length, const char* url);

	/*�ϴ��쳣��Ϣ*/
	typedef int (*PostErrorInfoFunc)(const char* msg, unsigned int length, const char* url);

	/*
	*	��������PostPerformanceLogFunc
	*	���ܣ��ϴ�������Ϣ
	*	������msg ��װ�õ�������Ϣ
	*	������length ���ݳ���
	*	�������ϴ��Ľӿڵ�ַ
	*/
	typedef int (*PostPerformanceInfoFunc)(const char* msg, unsigned int length, const char* url);

	/*
	*	��������LogFunc
	*	���ܣ���־��ӡ���ϱ�ģ���ڲ�û���ṩ��־����Ĵ�ӡ���ⲿ�ṩ��־�����ʵ��
	*	������logInfo �ϱ�ģ���ڲ���־��Ϣ
	*	������logLevel ��־��Ϣ����
	*/
	typedef void (*LogFunc)(const char* logInfo, LogLevel logLevel);

	/*
	����ģ���ʼ��
	����ֵ��0 �ɹ���1 ���غ���Ϊ�գ�2 ��ֵ���ú���Ϊ�գ�3 �ϴ�������Ϣ����Ϊ�գ�4 �ϴ�������Ϣ����Ϊ�գ�5 �ϴ�������Ϣ����Ϊ��
	*/
	APM_REPORT_API int APMInit(
		GetSwitchFunc funcGetSwitch,			//֪ͨ��ȡ����
		GetConfigFunc funcGetConfig,			//֪ͨ��ȡ��ֵ����
		PostClientInfoFunc funcPostClientInfo,	//֪ͨ�ϴ��豸������Ϣ
		PostErrorInfoFunc funcPostErrorInfo,		//֪ͨ�ϴ�������Ϣ
		PostPerformanceInfoFunc funcPerformancefInfo,	//֪ͨ�ϴ�������Ϣ
		LogFunc funcLog							//��־��ӡ����
	);

	/*
	��ֵ�ͻ��˻�����Ϣ
	����ֵ��0 �ɹ���-1 �����쳣��-2 ����������
	*/
	APM_REPORT_API int SetClientInfo(
		const char* appID,			//appID
		const char* appVersion,		//app�汾
		const char* OS,				//�豸ϵͳ
		const char* OSVersion,		//�豸ϵͳ�汾
		const char* deviceModel		//�豸�ͺ�
	);


	/*
	���ÿ���
	����ֵ��0 �ɹ���-1 �����쳣
	*/
	APM_REPORT_API int SetReportSwitch(const char* json);


	/*
	������ֵ����
	����ֵ��0 �ɹ���-1 �����쳣
	*/
	APM_REPORT_API int SetReportConfig(const char* json);


	/*
	��¼�쳣��Ϣ
	����ֵ��0 �ɹ���-1 �����쳣
	*/
	APM_REPORT_API int AddErrorInfo(const char* msg);


	/*
	��¼������Ϣ
	����ֵ��0 �ɹ���-1 �����쳣
	*/
	APM_REPORT_API int AddPerformanceInfo(const char* msg);

	/*
	�����ڲ���RSA������Կ
	����ֵ��0 �ɹ���-1 �����쳣
	*/
	APM_REPORT_API int SetRSAKey(const char* msg);

	/*
	ѹ���ͼ���
	����ֵ��0 �ɹ���-1 �����쳣
	*/
	APM_REPORT_API int ZipAndEncrypt(const char* msg);
}

#endif