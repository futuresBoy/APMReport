#include "json/json.h"
#include "Logger.h"
#include "Util.h"
#include "APMReport.h"
#include "ReportManager.h"

#define SDKVERSION "1.0.0.1"

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
		return 3;
	}
	return APMReport::TaskManager::GetInstance().APMInit(funcGetSwitch, funcGetConfig, funcPostErrorLog, funcLog);
}

APM_REPORT_API const char* GetSDKVersion()
{
	return SDKVERSION;
}

APM_REPORT_API int SetClientInfo(const char* baseInfo, char* outJosn, int& length)
{
	if (baseInfo == nullptr || baseInfo == "")
	{
		LOGERROR("baseInfo is null or empty.");
		return -1;
	}
	
	try
	{
		Json::Value base;
		Json::Reader reader;
		if (!reader.parse(baseInfo, base))
		{
			LOGERROR("baseInfo is not json.");
			return -1;
		}
		g_appID = base["app_id"].asString();
		if (g_appID.empty())
		{
			LOGERROR("app_id is null or empty.");
			return -1;
		}
		std::string uuid = base["d_uuid"].asString();
		if (uuid.empty())
		{
			LOGERROR("d_uuid is null or empty.");
			return -1;
		}
		base["s_ver"] = SDKVERSION;

		auto baseStr = Json::FastWriter().write(base);
		g_baseMD5Info = APMReport::Util::MD5(baseStr);
		if (g_baseMD5Info.empty())
		{
			LOGERROR("baseInfo to MD5 Error.");
			return -1;
		}

		//基础信息，该字段需要加密+base64转码传输
		std::string encodeBaseInfo;
		if (APMReport::Util::AesEncrypt(baseStr, encodeBaseInfo) < 0)
		{
			LOGERROR("AesEncrypt baseInfo Error.");
			return -1;
		}

		std::string keyID, pubKey;
		if (APMReport::Util::GetRSAPubKey(keyID, pubKey) < 0)
		{
			LOGERROR("RSAPubkey is empty ,should set RSA public key first.");
			return -1;
		}

		Json::Value root;
		root["app_id"] = g_appID;
		root["d_uuid"] = uuid;
		root["key_id"] = keyID;
		root["a_key"] = APMReport::Util::g_cipherAESKey;
		root["base_md5"] = g_baseMD5Info;
		root["logtime"] = APMReport::Util::GetTimeNowStr();
		root["base_info"] = encodeBaseInfo;
		std::string jsonStr = Json::FastWriter().write(root);
		
		if (jsonStr.length() >= length)
		{
			length = jsonStr.length() + 1;
			return -2;
		}
		length = jsonStr.length() + 1;
		memcpy(outJosn, jsonStr.c_str(), length);
	}
	catch (const std::exception & e)
	{
		LOGFATAL(e.what());
		return -1;
	}
	return 0;
}

APM_REPORT_API int SetReportConfig(const char* json)
{
	APMReport::TaskManager::GetInstance().LoadConfig(json);
	return 0;
}

APM_REPORT_API int SetReportSwitch(const char* json)
{
	APMReport::TaskManager::GetInstance().LoadSwitch(json);
	return 0;
}

APM_REPORT_API int AddErrorLog(const char* json)
{
	return 0;
}

APM_REPORT_API int SetRSAPubKey(const char* pubKeyID, const char* pubKey)
{
	return APMReport::Util::SetRSAPubKey(pubKeyID, pubKey);
}

APM_REPORT_API int BuildPerformanceData(const char* msg, char* outText, int& length)
{
	if (msg == nullptr || msg == "")
	{
		LOGERROR("msg is null or empty.");
		return 1;
	}

	try
	{
		//指标数组，该字段内容需要压缩+加密
		std::string data(msg);
		std::string zipData = APMReport::Util::GzipCompress(data);
		std::string metrics;
		APMReport::Util::AesEncrypt(zipData.c_str(), metrics);

		std::string keyID, pubKey;
		if (APMReport::Util::GetRSAPubKey(keyID, pubKey) < 0)
		{
			LOGERROR("RSAPubkey is empty ,should set RSA public key first.");
			return -1;
		}

		Json::Value root;
		root["app_id"] = g_appID;
		root["key_id"] = keyID;
		root["a_key"] = APMReport::Util::g_cipherAESKey;
		root["base_md5"] = g_baseMD5Info;
		root["metrics"] = metrics;
		std::string jsonStr = Json::FastWriter().write(root);
		if (jsonStr.length() >= length)
		{
			length = jsonStr.length() + 1;
			return -2;
		}
		length = jsonStr.length() + 1;
		memcpy(outText, jsonStr.c_str(), length);
	}
	catch (const std::exception& e)
	{
		LOGFATAL(e.what());
		return -1;
	}
	return 0;
}

