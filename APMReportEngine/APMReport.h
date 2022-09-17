#ifndef __APM_REPORT_H__
#define __APM_REPORT_H__

#ifdef APMREPORTENGINE_EXPORTS 
#define APM_REPORT_API _declspec(dllexport)
#else
#define APM_REPORT_API _declspec(dllimport)
#endif 

#ifdef __cplusplus
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

	/*�ϴ��쳣��Ϣ*/
	typedef int (*PostErrorLogFunc)(const char* msg, unsigned int length, const char* url);

	/*
	*	��������LogFunc
	*	���ܣ���־��ӡ֪ͨ���ⲿ���ṩ��־�����ʵ��
	*	������logInfo �ϱ�ģ���ڲ���־��Ϣ
	*	������logLevel ��־��Ϣ����
	*/
	typedef void (*LogFunc)(const char* logInfo, LogLevel logLevel);

	/*
		���ܣ�����ģ���ʼ��
		����ֵ��0 �ɹ���1 ���غ���Ϊ�գ�2 ��ֵ���ú���Ϊ�գ�3  �ϴ�������Ϣ����Ϊ��
	*/
	APM_REPORT_API int APMInit(
		GetSwitchFunc funcGetSwitch,			//֪ͨ��ȡ����
		GetConfigFunc funcGetConfig,			//֪ͨ��ȡ��ֵ����
		PostErrorLogFunc funcPostErrorLog,		//֪ͨ�ϴ�������Ϣ
		LogFunc funcLog							//��־��ӡ����
	);

	/*
		���ܣ���ȡSDK�汾��
		����ֵ��SDK�汾��
	*/
	APM_REPORT_API const char* GetSDKVersion();

	/*
		���ÿͻ��˻�����Ϣ
		������baseInfo �ͻ��˻�����Ϣ��json��ʽ����
			{"app_id": "Ӧ�ñ�ʶ��cmdb�ϵǼǵĿͻ��˳���Ӣ�ı���",
			"d_uuid": "�豸uuid���豸Ψһ��ʶ����Mac��ַ"
			"a_bundle_id": "���������",
			"a_ver_app": "����汾��",
			"d_os": "�豸ϵͳ",
			"d_model": "�豸�ͺ�",
			"d_ver": "�豸ϵͳ�汾����Ϊ�գ�"}
		���Σ�outText ��װ����豸������Ϣ
		��/���Σ�length �ⲿΪ�ַ�����Ŀռ䳤��Ϊ��Σ�����Ϊ������ַ�����
		����ֵ��0 �ɹ���-1 �����쳣��-2 ������ַ����ռ䳤�Ȳ���
	*/
	APM_REPORT_API int SetClientInfo(const char* baseInfo,char* outText,int& length);

	/*
		������ֵ����
		����ֵ��0 �ɹ���-1 �����쳣
	*/
	APM_REPORT_API int SetReportConfig(const char* json);

	/*
		���ÿ���
		����ֵ��0 �ɹ���-1 �����쳣
	*/
	APM_REPORT_API int SetReportSwitch(const char* json);

	/*
		��¼���ͻ��ˣ��쳣��־
		����ֵ��0 �ɹ���-1 �����쳣
	*/
	APM_REPORT_API int AddErrorLog(const char* msg);


	/*
		���ܣ���װ�������ݣ������м���+ѹ��
		������msg ����ָ������
		������outText ��������������ַ�
		��/���Σ�length �ⲿΪ�ַ�����Ŀռ䳤��Ϊ��Σ�����Ϊ������ַ�����
		����ֵ��0 �ɹ���-1 ʧ�ܣ�-2 ������ַ����ռ䳤�Ȳ���
	*/
	APM_REPORT_API int BuildPerformanceData(const char* msg, char* outText, int& length);

	/*
		���ܣ�����RSA��Կ
		������pubKeyID ��Կ���
		������pubKey ��Կ�ַ���
		����ֵ��0 �ɹ���-1 ��������ȷ
	*/
	APM_REPORT_API int SetRSAPubKey(const char* pubKeyID, const char* pubKey);

}
#endif // __cplusplus
#endif