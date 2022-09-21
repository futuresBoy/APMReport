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

	/*
	*	��������PostErrorLogFunc
	*	���ܣ�֪ͨ�ϴ��쳣��Ϣ
	*	������msg �쳣��Ϣ�ַ���
	*	������length �쳣��Ϣ�ַ�����
	*	������url �ϴ���Url��ַ
	*/
	typedef int (*PostErrorLogFunc)(const char* msg, unsigned int length, const char* url);

	/*
	*	��������LogFunc
	*	���ܣ���־��ӡ֪ͨ����֪SDK�ڲ���������־��Ϣ���ⲿ������־�����ʵ��
	*	������logInfo �ϱ�ģ���ڲ���־��Ϣ
	*	������logLevel ��־��Ϣ����
	*/
	typedef void (*LogFunc)(const char* logInfo, LogLevel logLevel);

	/*
		���ܣ���ʼ����־���
		������funcLog �ڲ���־���֪ͨ
		����ֵ��0 �ɹ���-1 �쳣
	*/
	APM_REPORT_API int InitLogger(LogFunc funcLog);

	/*
		���ܣ�����ģ���ʼ��
		������funcPostErrorLog ֪ͨ�ϴ�������Ϣ
		������funcLog �ڲ���־���֪ͨ
		����ֵ��0 �ɹ���-1 �쳣
	*/
	APM_REPORT_API int APMInit(PostErrorLogFunc funcPostErrorLog, LogFunc funcLog);

	/*
		���ܣ���ȡSDK�汾��
		����ֵ��SDK�汾��
	*/
	APM_REPORT_API const char* GetSDKVersion();

	/*
		���ܣ����ÿͻ��˻�����Ϣ
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
	APM_REPORT_API int SetClientInfo(const char* baseInfo, char* outText, int& length);

	/*
		���ܣ�������ֵ����
		����ֵ��0 �ɹ���-1 �����쳣
	*/
	APM_REPORT_API int SetReportConfig(const char* json);

	/*
		���ܣ����ÿ���
		����ֵ��0 �ɹ���-1 �����쳣
	*/
	APM_REPORT_API int SetReportSwitch(const char* json);

	/*
		���ܣ���¼���ͻ��ˣ��쳣��־
		����ֵ��0 �ɹ���-1 �����쳣
	*/
	APM_REPORT_API int AddErrorLog(const char* msg);


	/*
		���ܣ���װ�������ݣ������м���+ѹ��
		������appID Ӧ�ñ�ʶ��cmdb�ϵǼǵĿͻ��˳���Ӣ�ı���
		������msg ����ָ������
		������outText ��������������ַ�
		��/���Σ�length �ⲿΪ�ַ�����Ŀռ䳤��Ϊ��Σ�����Ϊ������ַ�����
		����ֵ��0 �ɹ���-1 ʧ�ܣ�-2 ������ַ����ռ䳤�Ȳ���
	*/
	APM_REPORT_API int BuildPerformanceData(const char* appID, const char* msg, char* outText, int& length);

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