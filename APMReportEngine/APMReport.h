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
	//本地日志打印级别
	typedef enum LogLevel
	{
		LOG_DEBUG = 0,
		LOG_INFO = 1,
		LOG_WARN = 2,
		LOG_ERROR = 3,
		LOG_FATAL = 4
	};

	/*
	*	函数名：PostErrorLogFunc
	*	功能：通知上传异常日志信息
	*	参数：msg 异常信息字符串
	*	参数：length 异常信息字符长度
	*	参数：url 上传的Url地址
	*	参数：url Url地址长度
	*/

	typedef int32_t(*PostErrorLogFunc)(const char* msg, int32_t msgLength, const char* url, int32_t urlLength);

	/*
	*	函数名：PostPerformanceFunc
	*	功能：通知上传性能信息
	*	参数：msg 性能信息字符串
	*	参数：length 性能信息字符长度
	*	参数：url 上传的Url地址
	*	参数：url Url地址长度
	*/

	typedef int32_t(*PostPerformanceFunc)(const char* msg, int32_t msgLength, const char* url, int32_t urlLength);

	/*
	*	函数名：LogFunc
	*	功能：日志打印通知，为SDK内部产生的日志信息，外部进行日志输出的实现
	*	参数：logInfo 上报模块内部日志信息
	*	参数：logLevel 日志信息级别
	*/
	typedef void (*LogFunc)(const char* logInfo, LogLevel logLevel);

	/*
		功能：SDK初始化
		参数：funcPostErrorLog 通知上传错误信息
		参数：funcPostPerformance 通知上传性能信息
		参数：funcLog 内部日志输出通知
		返回值：0 成功，-1 异常
	*/
	APM_REPORT_API int32_t APMInit(PostErrorLogFunc funcPostErrorLog, PostPerformanceFunc funcPostPerformance, LogFunc funcLog);

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
		出参：outText 组装后的设备基础信息（空间由外部申请，外部释放）
		入/出参：length 外部为字符申请的空间长度为入参，出参为输出的字符长度
		返回值：0 成功，-1 参数异常，-2 输出的字符串空间长度不够，-3 内部异常
	*/
	APM_REPORT_API int32_t SetClientInfo(const char* baseInfo, char* outText, int32_t& length);

	/*
		功能：设置阈值配置
		参数：json 阈值配置接口的json数据
		返回值：0 成功，-1 参数异常，-3 内部异常
	*/
	APM_REPORT_API int32_t SetReportConfig(const char* appID, const char* json);

	/*
		功能：设置开关
		参数：json 开关配置接口的json数据
		返回值：0 成功，-1 参数异常，-3 内部异常
	*/
	APM_REPORT_API int32_t SetReportSwitch(const char* appID, const char* json);

	/*
		功能：设置RSA公钥
		参数：pubKeyID 公钥编号
		参数：pubKey 公钥字符串
		返回值：0 成功，-1 参数不正确，-3 内部异常
	*/
	APM_REPORT_API int32_t SetRSAPubKey(const char* pubKeyID, const char* pubKey);


	/*
		功能：组装性能数据，并进行加密+压缩
		参数：appID 应用标识，cmdb上登记的客户端程序英文编码
		参数：msg 性能指标数据
		参数：outText 输出的性能数据字符（空间由外部申请，外部释放）
		入/出参：length 外部为字符申请的空间长度为入参，出参为输出的字符长度
		返回值：0 成功，-1 失败，-2 输出的字符串空间长度不够，-3 内部异常
	*/
	APM_REPORT_API int32_t BuildPerformanceData(const char* appID, const char* msg, char* outText, int32_t& length);

	/*
		功能：设置用户信息
		参数：appID 应用标识，cmdb上登记的客户端程序英文编码
		参数：账号userid（唯一）
		参数：账户名称/昵称（不唯一，可为空）
		参数：账户代码（唯一，可为空）
		返回值：0 成功，-1 参数异常，-3 内部异常
	*/
	APM_REPORT_API int32_t SetUserInfo(const char* appID, const char* userID, const char* userName, const char* userAccount);

	/*
		功能：设置拓展的用户信息
		参数：appID 应用标识，cmdb上登记的客户端程序英文编码
		参数：客户端用户信息（指定Json格式，参考：{"userID":"","userName":"","level2":"","tradeAccount":""}）
		返回值：0 成功，-1 参数异常，-3 内部异常
	*/
	APM_REPORT_API int32_t SetUserInfoEx(const char* appID, const char* userInfo);

	/*
		功能：获取链路追踪ID,用于客户端提供TradeID给其他业务方
		返回值：随机唯一字符串
	*/
	APM_REPORT_API const char* GetTraceID();

	/*
		功能：记录异常日志
		参数：appID 应用标识，cmdb上登记的客户端程序英文编码
		参数：module 上传elk的索引，如web,http,tcp，默认为pc
		参数：logType 错误分类，如apm_web_slow_request/apm_web_white_screen/apm_web_crash
		参数：bussiness 业务模块名称，如selfStock
		参数：subName 二级模块名称，如updateTable
		参数：errorCode 错误代码
		参数：msg 日志详情
		参数：extData 扩展字段（指定Json格式）
		返回值：0 成功，-1 参数异常，-3 内部异常
	*/
	APM_REPORT_API int32_t AddErrorLog(const char* appID,
		const char* module,
		const char* logType,
		const char* bussiness,
		const char* subName,
		const char* errorCode,
		const char* msg,
		const char* extData);

	/*
		功能：记录HTTP异常日志
		参数：appID 应用标识，cmdb上登记的客户端程序英文编码
		参数：logType 错误分类，如apm_http_slow_request/apm_http_error_request
		参数：bussiness 业务模块名称
		参数：url 请求的URL地址
		参数：errorCode 错误代码（0或空表示成功，建议statusCode为400-600时传statusCode）
		参数：costTime 请求HTTP耗时（单位：毫秒）
		参数：msg 日志详情（可为空）
		参数：extData 扩展字段（Json格式）
		返回值：0 成功，-1 参数异常，-3 内部异常
	*/
	APM_REPORT_API int32_t AddHTTPLog(const char* appID,
		const char* logType,
		const char* bussiness,
		const char* url,
		const char* errorCode,
		int32_t costTime,
		const char* msg,
		const char* extData);

	/*
		功能：获取链路追踪ID的Header,用于客户端提供TradeID给其他业务方
			请求HTTP的时候，附上TradeID在请求头里
		参数：traceID 链路追踪ID，由GetTraceID获取到
		出参：outBuffer 链路追踪ID
		入/出参：length 外部为字符申请的空间长度为入参，出参为输出的字符长度
		返回值：0 成功，-1 参数不正确，-2 输出的字符串空间长度不够，-3 内部异常
	*/
	APM_REPORT_API int32_t GetHttpHeader(const char* traceID, char* outBuffer, int32_t& length);

	/*
		功能：关闭指定appID的日志上报
	*/
	APM_REPORT_API int32_t Close(const char* appID);

	/*
		功能：关闭所有的日志上报
	*/
	APM_REPORT_API int32_t CloseAll();

}
#endif // __cplusplus
#endif