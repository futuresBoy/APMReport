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
	//本地日志打印级别
	typedef enum LogLevel
	{
		LOG_DEBUG = 0,
		LOG_INFO = 1,
		LOG_WARN = 2,
		LOG_ERROR = 3,
		LOG_FATAL = 4
	};

	//采集模块
	typedef enum DataModule
	{
		//崩溃
		Crash,
		//启动时间
		StartUpTime,
		//CPU和内存
		CPUAndMemory,
		//卡顿
		Caton,
		Web,
		Http,
		Tcp
	};

	/*
	*	函数名：PostErrorLogFunc
	*	功能：通知上传异常信息
	*	参数：msg 异常信息字符串
	*	参数：length 异常信息字符长度
	*	参数：url 上传的Url地址
	*/
	typedef int (*PostErrorLogFunc)(const char* msg, unsigned int length, const char* url);

	/*
	*	函数名：LogFunc
	*	功能：日志打印通知，告知SDK内部产生的日志信息，外部进行日志输出的实现
	*	参数：logInfo 上报模块内部日志信息
	*	参数：logLevel 日志信息级别
	*/
	typedef void (*LogFunc)(const char* logInfo, LogLevel logLevel);

	/*
		功能：初始化日志输出
		参数：funcLog 内部日志输出通知
		返回值：0 成功，-1 异常
	*/
	APM_REPORT_API int InitLogger(LogFunc funcLog);

	/*
		功能：发送模块初始化
		参数：funcPostErrorLog 通知上传错误信息
		参数：funcLog 内部日志输出通知
		返回值：0 成功，-1 异常
	*/
	APM_REPORT_API int APMInit(PostErrorLogFunc funcPostErrorLog, LogFunc funcLog);

	/*
		功能：获取SDK版本号
		返回值：SDK版本号
	*/
	APM_REPORT_API const char* GetSDKVersion();

	/*
		功能：设置客户端基础信息
		参数：baseInfo 客户端基础信息（json格式）：
			{"app_id": "应用标识，cmdb上登记的客户端程序英文编码",
			"d_uuid": "设备uuid，设备唯一标识，如Mac地址"
			"a_bundle_id": "程序进程名",
			"a_ver_app": "程序版本号",
			"d_os": "设备系统",
			"d_model": "设备型号",
			"d_ver": "设备系统版本（可为空）"}
		出参：outText 组装后的设备基础信息
		入/出参：length 外部为字符申请的空间长度为入参，出参为输出的字符长度
		返回值：0 成功，-1 参数异常，-2 输出的字符串空间长度不够
	*/
	APM_REPORT_API int SetClientInfo(const char* baseInfo, char* outText, int& length);

	/*
		功能：设置阈值配置
		返回值：0 成功，-1 参数异常
	*/
	APM_REPORT_API int SetReportConfig(const char* json);

	/*
		功能：设置开关
		返回值：0 成功，-1 参数异常
	*/
	APM_REPORT_API int SetReportSwitch(const char* json);

	/*
		功能：记录（客户端）异常日志
		返回值：0 成功，-1 参数异常
	*/
	APM_REPORT_API int AddErrorLog(const char* msg);


	/*
		功能：组装性能数据，并进行加密+压缩
		参数：appID 应用标识，cmdb上登记的客户端程序英文编码
		参数：msg 性能指标数据
		参数：outText 输出的性能数据字符
		入/出参：length 外部为字符申请的空间长度为入参，出参为输出的字符长度
		返回值：0 成功，-1 失败，-2 输出的字符串空间长度不够
	*/
	APM_REPORT_API int BuildPerformanceData(const char* appID, const char* msg, char* outText, int& length);

	/*
		功能：设置RSA公钥
		参数：pubKeyID 公钥编号
		参数：pubKey 公钥字符串
		返回值：0 成功，-1 参数不正确
	*/
	APM_REPORT_API int SetRSAPubKey(const char* pubKeyID, const char* pubKey);

}
#endif // __cplusplus
#endif