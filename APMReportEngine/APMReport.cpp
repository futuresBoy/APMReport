#include "json/json.h"
#include "Logger.h"
#include "Util.h"
#include "APMBasic.h"
#include "APMCryptogram.h"
#include "APMReport.h"
#include "APMReportManager.h"
#include "ClientBasicInfo.h"
#include <codecvt>

using namespace APMReport;

APM_REPORT_API int InitLogger(LogFunc funcLog)
{
	InitLog(funcLog);
	LOGINFO("InitLogger.", SDKVERSION);
	return 0;
}

APM_REPORT_API int APMInit(PostErrorLogFunc funcPostErrorLog, PostPerformanceFunc funcPostPerformance, LogFunc funcLog)
{
	InitLog(funcLog);
	return TaskManager::GetInstance().APMInit(funcPostErrorLog, funcPostPerformance);
}

APM_REPORT_API const char* GetSDKVersion()
{
	return SDKVERSION;
}

APM_REPORT_API int SetClientInfo(const char* baseInfo, char* outJosn, int32_t& length)
{
	if (baseInfo == nullptr || baseInfo == "")
	{
		LOGERROR("baseInfo is null or empty.");
		return ERROR_CODE_PARAMS;
	}

	try
	{
		Json::Value base;
		Json::Reader reader;
		if (!reader.parse(baseInfo, base))
		{
			LOGERROR("baseInfo is not json.");
			return ERROR_CODE_DATA_JSON;
		}
		std::string appID = base["app_id"].asString();
		if (appID.empty())
		{
			LOGERROR("app_id is null or empty.");
			return ERROR_CODE_DATA_JSON;
		}
		std::string uuid = base["d_uuid"].asString();
		if (uuid.empty())
		{
			LOGERROR("d_uuid is null or empty.");
			return ERROR_CODE_DATA_JSON;
		}
		Client::SetDeviceUUID(uuid);
		base.removeMember("d_uuid");
		base["s_ver"] = SDKVERSION;

		auto jsonWriter = Json::FastWriter();
		jsonWriter.omitEndingLineFeed();
		auto baseStr = jsonWriter.write(base);
		std::string appInfoMD5 = APMCryptogram::MD5(baseStr);
		if (appInfoMD5.empty())
		{
			LOGERROR("baseInfo to MD5 Error.");
			return ERROR_CODE_DATA_ENCODE;
		}
		Client::SetBaseInfo(appID, appInfoMD5);

		//基础信息，该字段需要加密+base64转码传输
		std::string encodeBaseInfo;
		if (APMCryptogram::AesEncrypt(baseStr, encodeBaseInfo) < 0)
		{
			LOGERROR("AesEncrypt baseInfo Error.");
			return ERROR_CODE_DATA_ENCRYPT;
		}

		std::string keyID, pubKey;
		if (APMCryptogram::GetRSAPubKey(keyID, pubKey) < 0)
		{
			LOGERROR("RSAPubkey is empty ,should set RSA public key first.");
			return ERROR_CODE_DATA_NULLKEY;
		}

		Json::Value root;
		root["app_id"] = appID;
		root["d_uuid"] = uuid;
		root["key_id"] = keyID;
		root["a_key"] = APMCryptogram::g_cipherAESKey;
		root["base_md5"] = appInfoMD5;
		root["logtime"] = Util::GetTimeNowStr();
		root["base_info"] = encodeBaseInfo;
		std::string jsonStr = jsonWriter.write(root);

		if (jsonStr.length() >= length)
		{
			length = jsonStr.length() + 1;
			return ERROR_CODE_OUTOFSIZE;
		}
		length = jsonStr.length() + 1;
		memcpy(outJosn, jsonStr.c_str(), length);
	}
	catch (const std::exception & e)
	{
		LOGFATAL(e.what());
		return ERROR_CODE_INNEREXCEPTION;
	}
	return 0;
}

APM_REPORT_API int SetReportConfig(const char* msg)
{
	if (msg == nullptr || msg == "")
	{
		LOGERROR("message is null or empty.");
		return ERROR_CODE_PARAMS;
	}
	return TaskManager::GetInstance().LoadThresholdConfig(msg);
}

APM_REPORT_API int SetReportSwitch(const char* msg)
{
	if (msg == nullptr || msg == "")
	{
		LOGERROR("message is null or empty.");
		return ERROR_CODE_PARAMS;
	}
	return TaskManager::GetInstance().LoadSwitch(msg);
}

APM_REPORT_API int SetRSAPubKey(const char* pubKeyID, const char* pubKey)
{
	return APMCryptogram::SetRSAPubKey(pubKeyID, pubKey);
}


APM_REPORT_API int BuildPerformanceData(const char* appID, const char* msg, char* outText, int32_t& length)
{
	if (appID == nullptr || appID == "")
	{
		LOGERROR("appID is null or empty.");
		return ERROR_CODE_PARAMS;
	}
	if (msg == nullptr || msg == "")
	{
		LOGERROR("msg is null or empty.");
		return ERROR_CODE_PARAMS;
	}

	try
	{
		//指标数组，该字段内容需要压缩+加密
		std::string data(msg);
		std::string zipData = APMCryptogram::GzipCompress(data);
		std::string metrics;
		if (APMCryptogram::AesEncrypt(zipData, metrics) != 0)
		{
			return ERROR_CODE_DATA_ENCRYPT;
		}

		std::string keyID, pubKey;
		if (APMCryptogram::GetRSAPubKey(keyID, pubKey) < 0)
		{
			LOGERROR("RSAPubkey is empty ,should set RSA public key first.");
			return ERROR_CODE_DATA_NULLKEY;
		}

		std::string baseInfoMD5 = Client::GetBaseInfo(appID);
		if (baseInfoMD5.empty())
		{
			LOGERROR("appID can not find, please set clientInfo first.");
			return ERROR_CODE_NULLCLIENTINFO;
		}

		Json::Value root;
		root["app_id"] = appID;
		root["key_id"] = keyID;
		root["a_key"] = APMCryptogram::g_cipherAESKey;
		root["base_md5"] = baseInfoMD5;
		root["metrics"] = metrics;
		auto jsonWriter = Json::FastWriter();
		jsonWriter.omitEndingLineFeed();
		std::string jsonStr = jsonWriter.write(root);
		if (jsonStr.length() >= length)
		{
			length = jsonStr.length() + 1;
			return ERROR_CODE_OUTOFSIZE;
		}
		length = jsonStr.length() + 1;
		memcpy(outText, jsonStr.c_str(), length);
	}
	catch (const std::exception & e)
	{
		LOGFATAL(e.what());
		return ERROR_CODE_INNEREXCEPTION;
	}
	return 0;
}

APM_REPORT_API int SetUserInfo(const char* userID, const char* userName, const char* userAccount)
{
	return User::SetUserInfo(userID, userName, userAccount);
}


APM_REPORT_API int AddErrorLog(const char* appID, const char* module, const char* logType, const char* bussiness, const char* subName, const char* errorCode, const char* msg, const char* extData)
{
	if (appID == nullptr || module == nullptr || logType == nullptr || bussiness == nullptr || subName == nullptr || errorCode == nullptr || msg == nullptr)
	{
		return ERROR_CODE_PARAMS;
	}
	auto traceID = Util::GetRandomUUID();
	try
	{
		//考虑针对不同地域语言，统一用UTF-8编码
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		std::wstring wideString = converter.from_bytes(msg);

		std::wstring wstrMsg(wideString);
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter2;
		std::string strMsg = converter2.to_bytes(wstrMsg);

		return TaskManager::GetInstance().AddTraceLog(traceID, module, logType, bussiness, subName, errorCode, strMsg, extData);
	}
	catch (const std::exception & e)
	{
		LOGFATAL(e.what());
		return ERROR_CODE_INNEREXCEPTION;
	}
}

APM_REPORT_API int AddHTTPLog(const char* appID, const char* logType, const char* bussiness, const char* url, const char* errorCode, int costTime, const char* msg, const char* extData)
{
	if (appID == nullptr || bussiness == nullptr || url == nullptr || errorCode == nullptr)
	{
		return ERROR_CODE_PARAMS;
	}
	auto traceID = Util::GetRandomUUID();
	try
	{
		//考虑针对不同地域语言，统一用UTF-8编码
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		std::wstring wideString = converter.from_bytes(msg);

		std::wstring wstrMsg(wideString);
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter2;
		std::string strMsg = converter2.to_bytes(wstrMsg);
		
		return TaskManager::GetInstance().AddHTTPLog(traceID, logType, bussiness, url, errorCode, costTime, strMsg, extData);
	}
	catch (const std::exception & e)
	{
		LOGFATAL(e.what());
		return ERROR_CODE_INNEREXCEPTION;
	}
}

APM_REPORT_API int GetTraceID(char* outBuffer, int32_t& length)
{
	if (outBuffer == nullptr)
	{
		return ERROR_CODE_PARAMS;
	}
	try
	{
		//skywalking的traceID格式
		std::string uuid = Util::GetRandomUUID();

		if (uuid.length() >= length)
		{
			length = uuid.length() + 1;
			return ERROR_CODE_OUTOFSIZE;
		}
		length = uuid.length() + 1;
		memcpy(outBuffer, uuid.c_str(), length);
		return 0;
	}
	catch (const std::exception & e)
	{
		LOGFATAL(e.what());
		return ERROR_CODE_INNEREXCEPTION;
	}
}

APM_REPORT_API int GetHttpHeader(const char* traceID, char* outBuffer, int32_t& length)
{
	if (outBuffer == nullptr)
	{
		return ERROR_CODE_PARAMS;
	}
	if (traceID == nullptr || traceID == "")
	{
		return ERROR_CODE_PARAMS;
	}
	try
	{
		std::string strTraceID(traceID);
		auto header = "trace_id:" + strTraceID + "\r\nspan_id:" + "0\r\n";
		if (header.length() >= length)
		{
			length = header.length() + 1;
			return ERROR_CODE_OUTOFSIZE;
		}
		length = header.length() + 1;
		memcpy(outBuffer, header.c_str(), length);
		return 0;
	}
	catch (const std::exception & e)
	{
		LOGFATAL(e.what());
		return ERROR_CODE_INNEREXCEPTION;
	}
}
