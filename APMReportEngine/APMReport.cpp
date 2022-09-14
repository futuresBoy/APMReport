#include "json/json.h"
#include "Logger.h"
#include "Util.h"
#include "APMReport.h"
#include "ReportManager.h"

#define SDKVERSION "1.0.0"

//缓存设备基础数据的MD5信息
std::string g_baseMD5Info;
std::string g_appID;

APM_REPORT_API int APMInit(GetSwitchFunc funcGetSwitch, GetConfigFunc funcGetConfig, PostErrorLogFunc funcPostErrorLog, LogFunc funcLog)
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
	if (nullptr == funcPostErrorLog)
	{
		return 4;
	}
	return APMReport::TaskManager::GetInstance().APMInit(funcGetSwitch, funcGetConfig, funcPostErrorLog, funcLog);
}

APM_REPORT_API int SetClientInfo(const char* appID, const char* UUID, const char* baseInfo, char* outJosn, int& outLen)
{
	if (appID == nullptr || appID == "")
	{
		LOGERROR("appID is null or empty.");
		return 1;
	}
	g_appID = appID;
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
	g_baseMD5Info = APMReport::MD5(base.asString());
	if (g_baseMD5Info.empty())
	{
		LOGERROR("baseInfo to MD5 Error.");
		return -1;
	}

	//基础信息，该字段需要加密+base64转码传输
	std::string encodeBaseInfo;
	if (APMReport::AesEncrypt(base.asCString(), encodeBaseInfo) < 0)
	{
		LOGERROR("AesEncrypt baseInfo Error.");
		return -1;
	}

	std::string keyID, pubKey;
	if (APMReport::GetRSAPubKey(keyID, pubKey) < 0)
	{
		LOGERROR("RSAPubkey is empty ,should set RSA public key first.");
		return -1;
	}

	Json::Value root;
	root["app_id"] = appID;
	root["d_uuid"] = UUID;
	root["key_id"] = keyID;
	root["a_key"] = APMReport::GetAESKey();
	root["base_md5"] = g_baseMD5Info;
	root["logtime"] = APMReport::GetTimeNowStr();
	root["base_info"] = encodeBaseInfo;
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

APM_REPORT_API int AddErrorLog(const char* json)
{
	return 0;
}

APM_REPORT_API int SetRSAPubKey(const char* pubKeyID, const char* pubKey)
{
	return APMReport::SetRSAPubKey(pubKeyID, pubKey);
}

APM_REPORT_API int BuildPerformanceInfo(const char* msg, char* outText, int& outLen)
{
	if (msg == nullptr || msg == "")
	{
		LOGERROR("msg is null or empty.");
		return 1;
	}

	//指标数组，该字段内容需要压缩+加密
	std::string data(msg);
	std::string zipData = APMReport::GzipCompress(data);
	std::string metrics;
	APMReport::AesEncrypt(zipData.c_str(), metrics);

	std::string keyID, pubKey;
	if (APMReport::GetRSAPubKey(keyID, pubKey) < 0)
	{
		LOGERROR("RSAPubkey is empty ,should set RSA public key first.");
		return -1;
	}

	Json::Value root;
	root["app_id"] = g_appID;
	root["key_id"] = keyID;
	root["a_key"] = APMReport::GetAESKey();
	root["base_md5"] = g_baseMD5Info;
	root["metrics"] = metrics;
	std::string jsonStr = root.asString();
	outLen = jsonStr.length();
	memcpy(outText, jsonStr.c_str(), outLen);
	return 0;
}

