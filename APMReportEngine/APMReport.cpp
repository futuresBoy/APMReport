#include "json/json.h"
#include "Logger.h"
#include "Util.h"
#include "APMBasic.h"
#include "APMCryptogram.h"
#include "APMReport.h"
#include "APMReportManager.h"
#include "ClientManager.h"
#include "User.h"
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
	return APMReportManager::APMInit(funcPostErrorLog, funcPostPerformance);
}

APM_REPORT_API const char* GetSDKVersion()
{
	return SDKVERSION;
}

APM_REPORT_API int SetClientInfo(const char* baseInfo, char* outJosn, int32_t& length)
{
	if (CHECK_ISNULLOREMPTY(baseInfo))
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
			LOGINFO(baseInfo);
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
		ClientManager::SetDeviceUUID(appID, uuid);
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
		ClientManager::SetBaseInfo(appID, appInfoMD5);

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

APM_REPORT_API int SetReportConfig(const char* appID, const char* msg)
{
	if (CHECK_ISNULLOREMPTY(appID))
	{
		LOGERROR("appID is null or empty.");
		return ERROR_CODE_PARAMS;
	}
	if (CHECK_ISNULLOREMPTY(msg))
	{
		LOGERROR("message is null or empty.");
		return ERROR_CODE_PARAMS;
	}
	return APMReportManager::Get(appID)->LoadThresholdConfig(msg);
}

APM_REPORT_API int SetReportSwitch(const char* appID, const char* msg)
{
	if (CHECK_ISNULLOREMPTY(appID))
	{
		LOGERROR("appID is null or empty.");
		return ERROR_CODE_PARAMS;
	}
	if (CHECK_ISNULLOREMPTY(msg))
	{
		LOGERROR("message is null or empty.");
		return ERROR_CODE_PARAMS;
	}
	return APMReportManager::Get(appID)->LoadSwitch(msg);
}

APM_REPORT_API int SetRSAPubKey(const char* pubKeyID, const char* pubKey)
{
	return APMCryptogram::SetRSAPubKey(pubKeyID, pubKey);
}


APM_REPORT_API int BuildPerformanceData(const char* appID, const char* msg, char* outText, int32_t& length)
{
	if (CHECK_ISNULLOREMPTY(appID))
	{
		LOGERROR("appID is null or empty.");
		return ERROR_CODE_PARAMS;
	}
	if (CHECK_ISNULLOREMPTY(msg))
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

		std::string baseInfoMD5 = ClientManager::GetBaseInfo(appID);
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

//考虑不同地域语言，统一用UTF-8编码
static std::string ConvertUTF8(const char* msg)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::wstring wideString = converter.from_bytes(msg);

	std::wstring wstrMsg(wideString);
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter2;
	return converter2.to_bytes(wstrMsg);
}

APM_REPORT_API int SetUserInfo(const char* appID, const char* userID, const char* userName, const char* userAccount)
{
	if (CHECK_ISNULL(appID) || CHECK_ISNULL(userID))
	{
		return ERROR_CODE_PARAMS;
	}
	return User::SetUserInfo(appID, userID, userName, userAccount);
}


APM_REPORT_API int SetUserInfoEx(const char* appID, const char* userInfo)
{
	if (CHECK_ISNULL(appID) || CHECK_ISNULL(userInfo))
	{
		return ERROR_CODE_PARAMS;
	}
	try
	{
		return User::SetUserInfoEx(appID, ConvertUTF8(userInfo));
	}
	catch (const std::exception & e)
	{
		LOGFATAL(e.what());
		return ERROR_CODE_INNEREXCEPTION;
	}
	return 0;
}

APM_REPORT_API const char* GetTraceID()
{
	return Util::GetRandomUUID().c_str();
}

APM_REPORT_API int AddErrorLog(const char* appID, const char* module, const char* logType, const char* bussiness, const char* subName, const char* errorCode, const char* msg, const char* extData)
{
	if (CHECK_ISNULL(appID) || CHECK_ISNULL(module) || CHECK_ISNULL(logType) || CHECK_ISNULL(bussiness)
		|| CHECK_ISNULL(subName) || CHECK_ISNULL(errorCode) || CHECK_ISNULL(msg))
	{
		LOGERROR("one of params is null.");
		return ERROR_CODE_PARAMS;
	}
	try
	{
		std::string baseInfoMD5 = ClientManager::GetBaseInfo(appID);
		if (baseInfoMD5.empty())
		{
			LOGERROR("appID can not find, please set clientInfo first.");
			return ERROR_CODE_NULLCLIENTINFO;
		}

		std::string strMsg = ConvertUTF8(msg);
		int result = APMReportManager::Get(appID)->AddTraceLog(module, logType, bussiness, subName, errorCode, strMsg, extData);

#ifdef _DEBUG
		std::string sType(logType);
		std::string out = "AddErrorLog: " + sType + " " + strMsg + " result:" + std::to_string(result);
		LOGINFO(out.c_str());
#endif // DEBUG

		return result;
	}
	catch (const std::exception & e)
	{
		LOGFATAL(e.what());
		return ERROR_CODE_INNEREXCEPTION;
	}
	return 0;
}

APM_REPORT_API int AddHTTPLog(const char* appID, const char* logType, const char* bussiness, const char* url, const char* errorCode, int costTime, const char* msg, const char* extData)
{
	if (CHECK_ISNULL(appID) || CHECK_ISNULL(logType) || CHECK_ISNULL(bussiness)
		|| CHECK_ISNULL(url) || CHECK_ISNULL(errorCode) || CHECK_ISNULL(msg))
	{
		LOGERROR("one of params is null.");
		return ERROR_CODE_PARAMS;
	}
	try
	{
		std::string baseInfoMD5 = ClientManager::GetBaseInfo(appID);
		if (baseInfoMD5.empty())
		{
			LOGERROR("appID can not find, please set clientInfo first.");
			return ERROR_CODE_NULLCLIENTINFO;
		}

		return APMReportManager::Get(appID)->AddHTTPLog(logType, bussiness, url, errorCode, costTime, ConvertUTF8(msg), extData);
	}
	catch (const std::exception & e)
	{
		LOGFATAL(e.what());
		return ERROR_CODE_INNEREXCEPTION;
	}
	return 0;
}

APM_REPORT_API int GetHttpHeader(const char* traceID, char* outBuffer, int32_t& length)
{
	if (outBuffer == nullptr)
	{
		return ERROR_CODE_PARAMS;
	}
	if (CHECK_ISNULLOREMPTY(traceID))
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

APM_REPORT_API int32_t Close(const char* appID)
{
	if (CHECK_ISNULLOREMPTY(appID))
	{
		return ERROR_CODE_PARAMS;
	}
	return APMReportManager::Close(appID) ? 0 : ERROR_CODE_INNEREXCEPTION;
}

APM_REPORT_API int32_t CloseAll()
{
	return APMReportManager::Close() ? 0 : ERROR_CODE_INNEREXCEPTION;
}
