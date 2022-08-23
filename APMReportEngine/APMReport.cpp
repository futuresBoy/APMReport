#include "json/json.h"
#include "Logger.h"
#include "Util.h"
#include "APMReport.h"
#include "ReportManager.h"

#define SDKVERSION "1.0.0"

//缓存设备基础数据的MD5信息
std::string g_baseMD5Info;

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

APM_REPORT_API int BuildClientInfo(const char* appID, const char* UUID, const char* baseInfo, char* outJosn, int& outLen)
{
	if (appID == nullptr || appID == "")
	{
		LOGERROR("appID is null or empty.");
		return 1;
	}
	if (UUID == nullptr || UUID == "")
	{
		LOGERROR("UUID is null or empty.");
		return 2;
	}
	if (baseInfo == nullptr || baseInfo == "")
	{
		LOGERROR("baseInfo is null or empty.");
		return 3;
	}
	Json::Value base;
	Json::Reader reader;
	if (!reader.parse(baseInfo, base))
	{
		LOGERROR("baseInfo is not json.");
		return -1;
	}
	base["s_ver"] = SDKVERSION;

	std::string keyID, pubKey;
	if (APMReport::GetRSAPubKey(keyID, pubKey) < 0)
	{
		LOGERROR("RSAPubkey is empty ,should set RSA public key.");
		return -1;
	}

	Json::Value root;
	root["app_id"] = appID;
	root["d_uuid"] = UUID;
	root["key_id"] = keyID;
	root["a_key"] = APMReport::GetAESKey();
	g_baseMD5Info = APMReport::MD5(base.asString());
	root["base_md5"] = g_baseMD5Info;
	root["logtime"] = APMReport::GetTimeNowStr();
	root["base_info"] = base;
	std::string jsonStr = root.asString();
	outLen = jsonStr.length();
	memcpy(outJosn, jsonStr.c_str(), outLen);
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

APM_REPORT_API int SetRSAPubKey(const char* pubKeyID, const char* pubKey)
{
	return APMReport::SetRSAPubKey(pubKeyID, pubKey);
}

APM_REPORT_API int BuildPerformanceInfo(const char* msg, char* outText, int& outLen)
{

	return 0;
}

