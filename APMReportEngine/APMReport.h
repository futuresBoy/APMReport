#include <stdint.h>

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

	/*
	*	��������PostErrorLogFunc
	*	���ܣ�֪ͨ�ϴ��쳣��־��Ϣ
	*	������msg �쳣��Ϣ�ַ���
	*	������length �쳣��Ϣ�ַ�����
	*	������url �ϴ���Url��ַ
	*	������url Url��ַ����
	*/

	typedef int32_t(*PostErrorLogFunc)(const char* msg, int32_t msgLength, const char* url, int32_t urlLength);

	/*
	*	��������PostPerformanceFunc
	*	���ܣ�֪ͨ�ϴ�������Ϣ
	*	������msg ������Ϣ�ַ���
	*	������length ������Ϣ�ַ�����
	*	������url �ϴ���Url��ַ
	*	������url Url��ַ����
	*/

	typedef int32_t(*PostPerformanceFunc)(const char* msg, int32_t msgLength, const char* url, int32_t urlLength);

	/*
	*	��������LogFunc
	*	���ܣ���־��ӡ֪ͨ��ΪSDK�ڲ���������־��Ϣ���ⲿ������־�����ʵ��
	*	������logInfo �ϱ�ģ���ڲ���־��Ϣ
	*	������logLevel ��־��Ϣ����
	*/
	typedef void (*LogFunc)(const char* logInfo, LogLevel logLevel);

	/*
		���ܣ���ʼ����־���
		������funcLog �ڲ���־���֪ͨ
		����ֵ��0 �ɹ���-1 �쳣
	*/
	APM_REPORT_API int32_t InitLogger(LogFunc funcLog);

	/*
		���ܣ�SDK��ʼ����������ʼ����־InitLogger��
		������funcPostErrorLog ֪ͨ�ϴ�������Ϣ
		������funcLog �ڲ���־���֪ͨ
		����ֵ��0 �ɹ���-1 �쳣
	*/
	APM_REPORT_API int32_t APMInit(PostErrorLogFunc funcPostErrorLog, PostPerformanceFunc funcPostPerformance, LogFunc funcLog);

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
		���Σ�outText ��װ����豸������Ϣ���ռ����ⲿ���룬�ⲿ�ͷţ�
		��/���Σ�length �ⲿΪ�ַ�����Ŀռ䳤��Ϊ��Σ�����Ϊ������ַ�����
		����ֵ��0 �ɹ���-1 �����쳣��-2 ������ַ����ռ䳤�Ȳ�����-3 �ڲ��쳣
	*/
	APM_REPORT_API int32_t SetClientInfo(const char* baseInfo, char* outText, int32_t& length);

	/*
		���ܣ�������ֵ����
		����ֵ��0 �ɹ���-1 �����쳣��-3 �ڲ��쳣
	*/
	APM_REPORT_API int32_t SetReportConfig(const char* json);

	/*
		���ܣ����ÿ���
		����ֵ��0 �ɹ���-1 �����쳣��-3 �ڲ��쳣
	*/
	APM_REPORT_API int32_t SetReportSwitch(const char* json);

	/*
		���ܣ�����RSA��Կ
		������pubKeyID ��Կ���
		������pubKey ��Կ�ַ���
		����ֵ��0 �ɹ���-1 ��������ȷ��-3 �ڲ��쳣
	*/
	APM_REPORT_API int32_t SetRSAPubKey(const char* pubKeyID, const char* pubKey);


	/*
		���ܣ���װ�������ݣ������м���+ѹ��
		������appID Ӧ�ñ�ʶ��cmdb�ϵǼǵĿͻ��˳���Ӣ�ı���
		������msg ����ָ������
		������outText ��������������ַ����ռ����ⲿ���룬�ⲿ�ͷţ�
		��/���Σ�length �ⲿΪ�ַ�����Ŀռ䳤��Ϊ��Σ�����Ϊ������ַ�����
		����ֵ��0 �ɹ���-1 ʧ�ܣ�-2 ������ַ����ռ䳤�Ȳ�����-3 �ڲ��쳣
	*/
	APM_REPORT_API int32_t BuildPerformanceData(const char* appID, const char* msg, char* outText, int32_t& length);

	/*
		���ܣ������û���Ϣ
		�������˺�userid��Ψһ��
		�������˻�����/�ǳƣ���Ψһ����Ϊ�գ�
		�������˻����루Ψһ����Ϊ�գ�
		����ֵ��0 �ɹ���-1 �����쳣��-3 �ڲ��쳣
	*/
	APM_REPORT_API int32_t SetUserInfo(const char* userID, const char* userName, const char* userAccount);

	/*
		���ܣ���¼������־
		������appID Ӧ�ñ�ʶ��cmdb�ϵǼǵĿͻ��˳���Ӣ�ı���
		������moduleName ģ�����ƣ��ͻ����Լ����壩
		������subName ����ģ�����ƣ���Ϊ�գ�
		������errorCode ������루�ͻ��˶��壩
		������msg ��־��Ϣ
		����ֵ��0 �ɹ���-1 �����쳣��-3 �ڲ��쳣
	*/
	APM_REPORT_API int32_t AddErrorLog(const char* appID, const char* moduleName, const char* subName, const char* errorCode, const wchar_t* msg);

	/*
		���ܣ���¼HTTP��־
		������appID Ӧ�ñ�ʶ��cmdb�ϵǼǵĿͻ��˳���Ӣ�ı���
		������moduleName ģ�����ƣ���Ϊ�գ��ͻ����Լ����壩
		������url �����URL��ַ
		������errorCode ������루0��ձ�ʾ�ɹ���
		������costTime ����HTTP��ʱ����λ�����룩
		������msg ��־��Ϣ����Ϊ�գ�
		����ֵ��0 �ɹ���-1 �����쳣��-3 �ڲ��쳣
	*/
	APM_REPORT_API int32_t AddHTTPLog(const char* appID, const char* moduleName, const char* url, const char* errorCode, int32_t costTime, const wchar_t* msg);

	/*
		���ܣ���ȡ��·׷��ID,���ڿͻ����ṩTradeID������ҵ��
		���Σ�outBuffer ��·׷��ID���ռ����ⲿ���룬�ⲿ�ͷţ�
		��/���Σ�length �ⲿΪ�ַ�����Ŀռ䳤��Ϊ��Σ�����Ϊ������ַ�����
		����ֵ��0 �ɹ���-1 ��������ȷ��-2 ������ַ����ռ䳤�Ȳ�����-3 �ڲ��쳣
	*/
	APM_REPORT_API int32_t GetTraceID(char* outBuffer, int32_t& length);

	/*
		���ܣ���ȡ��·׷��ID��Header,���ڿͻ����ṩTradeID������ҵ��
			����HTTP��ʱ�򣬸���TradeID������ͷ��
		������traceID ��·׷��ID����GetTraceID��ȡ��
		���Σ�outBuffer ��·׷��ID
		��/���Σ�length �ⲿΪ�ַ�����Ŀռ䳤��Ϊ��Σ�����Ϊ������ַ�����
		����ֵ��0 �ɹ���-1 ��������ȷ��-2 ������ַ����ռ䳤�Ȳ�����-3 �ڲ��쳣
	*/
	APM_REPORT_API int32_t GetHttpHeader(const char* traceID, char* outBuffer, int32_t& length);

#pragma region ClientMonitor��ӿ�

	/*
		���ܣ������־
		������appID Ӧ�ñ�ʶ��cmdb�ϵǼǵĿͻ��˳���Ӣ�ı���
		������moduleName ģ�����ƣ��ͻ����Լ����壩
		������subName ����ģ�����ƣ���Ϊ�գ�
		������errorCode ������루�ɿͻ����Զ��壩
		������moduleType ���ģ����𣨲ο�APMBasic.h�е�ģ�鶨�壩
		������isSucceed �Ƿ�ɹ�
		������msg ��־��Ϣ
		����ֵ��0 �ɹ���-1 �����쳣��-3 �ڲ��쳣
	*/
	APM_REPORT_API int32_t AddTraceLog(const char* appID,
		const char* moduleName,
		const char* subName,
		const char* errorCode,
		int32_t moduleType,
		bool isSucceed,
		const wchar_t* msg);

	/*
		���ܣ��ϴ��ɹ���־
		������appID Ӧ�ñ�ʶ��cmdb�ϵǼǵĿͻ��˳���Ӣ�ı���
		������traceID ��·׷��ID
		������moduleName ģ�����ƣ��ͻ����Լ����壩
		������subName ����ģ�����ƣ���Ϊ�գ�
		������errorCode �������
		������moduleType ���ģ�����
		������msg ��־��Ϣ
		����ֵ��0 �ɹ���-1 �����쳣��-3 �ڲ��쳣
	*/
	APM_REPORT_API int32_t TradeLogOK(const char* appID,
		const char* traceID,
		const char* moduleName,
		const char* subName,
		const char* errorCode,
		int32_t moduleType,
		const wchar_t* msg);

	/*
		���ܣ��ϴ�ʧ����־
		������appID Ӧ�ñ�ʶ��cmdb�ϵǼǵĿͻ��˳���Ӣ�ı���
		������traceID ��·׷��ID
		������moduleName ģ�����ƣ��ͻ����Լ����壩
		������subName ����ģ�����ƣ���Ϊ�գ�
		������errorCode �������
		������moduleType ���ģ�����
		������msg ��־��Ϣ
		����ֵ��0 �ɹ���-1 �����쳣��-3 �ڲ��쳣
	*/
	APM_REPORT_API int32_t TradeLogErr(const char* appID,
		const char* traceID,
		const char* moduleName,
		const char* subName,
		const char* errorCode,
		int32_t moduleType,
		const wchar_t* msg);

	/*
		���ܣ��ϴ���ʱ��־
		������appID Ӧ�ñ�ʶ��cmdb�ϵǼǵĿͻ��˳���Ӣ�ı���
		������traceID ��·׷��ID
		������moduleName ģ�����ƣ��ͻ����Լ����壩
		������subName ����ģ�����ƣ���Ϊ�գ�
		������errorCode �������
		������moduleType ���ģ�����
		������msg ��־��Ϣ
		����ֵ��0 �ɹ���-1 �����쳣��-3 �ڲ��쳣
	*/
	APM_REPORT_API int32_t TradeLogTimeOut(const char* appID,
		const char* traceID,
		const char* moduleName,
		const char* subName,
		const char* errorCode,
		int32_t moduleType,
		const wchar_t* msg);

#pragma endregion

}
#endif // __cplusplus
#endif