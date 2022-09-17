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

	/*获取开关配置*/
	typedef int (*GetSwitchFunc)(const char* url);

	/*获取阈值配置*/
	typedef int (*GetConfigFunc)(const char* url);

	/*上传异常信息*/
	typedef int (*PostErrorLogFunc)(const char* msg, unsigned int length, const char* url);

	/*
	*	函数名：LogFunc
	*	功能：日志打印通知，外部可提供日志输出的实现
	*	参数：logInfo 上报模块内部日志信息
	*	参数：logLevel 日志信息级别
	*/
	typedef void (*LogFunc)(const char* logInfo, LogLevel logLevel);

	/*
		功能：发送模块初始化
		返回值：0 成功，1 开关函数为空，2 阈值配置函数为空，3  上传错误信息函数为空
	*/
	APM_REPORT_API int APMInit(
		GetSwitchFunc funcGetSwitch,			//通知获取开关
		GetConfigFunc funcGetConfig,			//通知获取阈值配置
		PostErrorLogFunc funcPostErrorLog,		//通知上传错误信息
		LogFunc funcLog							//日志打印函数
	);

	/*
		功能：获取SDK版本号
		返回值：SDK版本号
	*/
	APM_REPORT_API const char* GetSDKVersion();

	/*
		设置客户端基础信息
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
	APM_REPORT_API int SetClientInfo(const char* baseInfo,char* outText,int& length);

	/*
		设置阈值配置
		返回值：0 成功，-1 参数异常
	*/
	APM_REPORT_API int SetReportConfig(const char* json);

	/*
		设置开关
		返回值：0 成功，-1 参数异常
	*/
	APM_REPORT_API int SetReportSwitch(const char* json);

	/*
		记录（客户端）异常日志
		返回值：0 成功，-1 参数异常
	*/
	APM_REPORT_API int AddErrorLog(const char* msg);


	/*
		功能：组装性能数据，并进行加密+压缩
		参数：msg 性能指标数据
		参数：outText 输出的性能数据字符
		入/出参：length 外部为字符申请的空间长度为入参，出参为输出的字符长度
		返回值：0 成功，-1 失败，-2 输出的字符串空间长度不够
	*/
	APM_REPORT_API int BuildPerformanceData(const char* msg, char* outText, int& length);

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