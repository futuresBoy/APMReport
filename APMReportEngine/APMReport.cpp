#include "APMReport.h"
#include "ReportManager.h"
#include "Logger.h"
#include "json/json.h"
#include "Util.h"

APM_REPORT_API int APMInit(GetSwitchFunc funcGetSwitch, GetConfigFunc funcGetConfig, PostClientInfoFunc funcPostClientInfo, PostErrorInfoFunc funcPostErrorInfo, PostPerformanceInfoFunc funcPerformanceInfo, LogFunc funcLog)
{
	InitLog(funcLog);
	if (nullptr == funcGetSwitch)
	{
		return 1;
	}
	if (nullptr == funcGetConfig)
	{
		return 2;
	}
	if (nullptr == funcPostClientInfo)
	{
		return 3;
	}
	if (nullptr == funcPostErrorInfo)
	{
		return 4;
	}
	if (nullptr == funcPerformanceInfo)
	{
		return 5;
	}
	return APMReport::TaskManager::GetInstance().APMInit(funcGetSwitch, funcGetConfig, funcPostClientInfo, funcPostErrorInfo, funcPerformanceInfo, funcLog);
}

APM_REPORT_API int SetClientInfo(const char* appID, const char* appVersion, const char* OS, const char* OSVersion, const char* deviceModel)
{
	return 0;
}

APM_REPORT_API int SetReportSwitch(const char* json)
{
	APMReport::TaskManager::GetInstance().LoadSwitch(json);
	return 0;
}

APM_REPORT_API int SetReportConfig(const char* json)
{
	APMReport::TaskManager::GetInstance().LoadConfig(json);
	return 0;
}

APM_REPORT_API int AddErrorInfo(const char* json)
{
	return 0;
}


APM_REPORT_API int AddPerformanceInfo(const char* json)
{
	return 0;
}

APM_REPORT_API int SetRSAPubKey(const char* pubKeyID,const char* pubKey)
{
	return APMReport::SetRSAKey(pubKeyID, pubKey);
}

APM_REPORT_API const char* CompressEncrypt(const char* msg)
{
	char* p=nullptr;
	int i = 0;
	APMReport::AesEncrypt(msg, p, i);
	return "";
}

