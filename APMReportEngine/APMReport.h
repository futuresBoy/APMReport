#ifndef __APM_REPORT_H__
#define __APM_REPORT_H__

#ifdef APMREPORTENGINE_EXPORTS 
#define APM_REPORT_API _declspec(dllexport)
#else
#define APM_REPORT_API _declspec(dllimport)
#endif 

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

	/*上传基础信息*/
	typedef int (*PostClientInfoFunc)(const char* msg, unsigned int length, const char* url);

	/*上传异常信息*/
	typedef int (*PostErrorInfoFunc)(const char* msg, unsigned int length, const char* url);

	/*
	*	函数名：PostPerformanceLogFunc
	*	功能：上传性能信息
	*	参数：msg 组装好的性能信息
	*	参数：length 数据长度
	*	参数：上传的接口地址
	*/
	typedef int (*PostPerformanceInfoFunc)(const char* msg, unsigned int length, const char* url);

	/*
	*	函数名：LogFunc
	*	功能：日志打印，上报模块内部没有提供日志输出的打印，外部提供日志输出的实现
	*	参数：logInfo 上报模块内部日志信息
	*	参数：logLevel 日志信息级别
	*/
	typedef void (*LogFunc)(const char* logInfo, LogLevel logLevel);

	/*
		发送模块初始化
		返回值：0 成功，1 开关函数为空，2 阈值配置函数为空，3 上传基础信息函数为空，4 上传错误信息函数为空，5 上传性能信息函数为空
	*/
	APM_REPORT_API int APMInit(
		GetSwitchFunc funcGetSwitch,			//通知获取开关
		GetConfigFunc funcGetConfig,			//通知获取阈值配置
		PostClientInfoFunc funcPostClientInfo,	//通知上传设备基础信息
		PostErrorInfoFunc funcPostErrorInfo,		//通知上传错误信息
		PostPerformanceInfoFunc funcPerformancefInfo,	//通知上传性能信息
		LogFunc funcLog							//日志打印函数
	);

	/*
		设置客户端基础信息
		返回值：0 成功，-1 参数异常，-2 参数不完整
	*/
	APM_REPORT_API int SetClientInfo(
		const char* appID,			//appID
		const char* appVersion,		//app版本
		const char* OS,				//设备系统
		const char* OSVersion,		//设备系统版本
		const char* deviceModel		//设备型号
	);

	/*
		构建客户端基础信息
		参数：appID 应用标识，cmdb上登记的客户端程序英文编码
		参数：UUID 通用唯一识别码，通常为Mac地址
		参数：baseInfo 基础信息（json格式）：
			{
			"a_bundle_id": "程序进程名",
			"a_ver_app": "程序版本号",
			"d_os": "设备系统",
			"d_model": "设备型号",
			"d_ver": "设备系统版本",
			"s_ver": ""
			}
		参数：outJosn 输出组装好的设备基础信息
		参数：outLen 输出的设备基础信息字符串长度
		返回值：0 成功，-1 参数异常，-2 参数不完整
	*/
	APM_REPORT_API int BuildClientInfo(const char* appID,const char* UUID,const char* baseInfo,char* outJosn,int& outLen);

	/*
		设置开关
		返回值：0 成功，-1 参数异常
	*/
	APM_REPORT_API int SetReportSwitch(const char* json);


	/*
		设置阈值配置
		返回值：0 成功，-1 参数异常
	*/
	APM_REPORT_API int SetReportConfig(const char* json);


	/*
		记录异常信息
		返回值：0 成功，-1 参数异常
	*/
	APM_REPORT_API int AddErrorInfo(const char* msg);


	/*
		记录性能信息
		返回值：0 成功，-1 参数异常
	*/
	APM_REPORT_API int AddPerformanceInfo(const char* msg);

	/*
		功能：构建服务端需要的性能数据体（加密+压缩）
		参数：msg 性能指标数据
		参数：outText 组装好的数据体
		参数：outLen 组装好的数据长度
		返回值：0 成功，-1 失败
	*/
	APM_REPORT_API int BuildPerformanceInfo(const char* msg, char* outText, int& outLen);

	/*
		功能：设置RSA公钥
		参数：pubKeyID 公钥编号
		参数：pubKey 公钥字符串
		返回值：0 成功，-1 参数不正确
	*/
	APM_REPORT_API int SetRSAPubKey(const char* pubKeyID, const char* pubKey);

}

#endif