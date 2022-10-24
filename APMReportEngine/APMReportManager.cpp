#include "Logger.h"
#include "APMBasic.h"
#include "APMCryptogram.h"
#include "APMReportManager.h"
#include "Util.h"
#include "ClientBasicInfo.h"
#include <codecvt>

namespace APMReport
{
	static TaskManager g_manager;

	//上报异常信息任务
	ReportErrorTask g_reportErrorTask;

	//上传性能信息任务
	ReportPerformanceTask g_reportPerfTask;

	TaskManager& APMReport::TaskManager::GetInstance()
	{
		return g_manager;
	}

	TaskManager::TaskManager()
	{
		m_bInited = false;
	}

	APMReport::TaskManager::~TaskManager()
	{
		if (m_pThreadErrorLog)
		{
			m_bInited = false;
			m_bThreadExit = true;
			m_pThreadErrorLog->join();
			delete m_pThreadErrorLog;
			m_pThreadErrorLog = nullptr;
			m_pThreadPerformance->join();
			delete m_pThreadPerformance;
			m_pThreadPerformance = nullptr;
		}
	}


	int APMReport::TaskManager::APMInit(PostErrorLogFunc funcPostErrorInfo, PostPerformanceFunc funcPostPerformanceInfo)
	{
		if (nullptr == funcPostErrorInfo || nullptr == funcPostPerformanceInfo)
		{
			return ERROR_CODE_PARAMS;
		}
		m_funcPostErrorInfo = funcPostErrorInfo;
		m_funcPostPerformance = funcPostPerformanceInfo;
		if (m_bInited)
		{
			LOGWARN("Inited Aready!");
			return 0;
		}
		m_bInited = true;
		m_bThreadExit = false;
		m_pThreadErrorLog = new std::thread([this] { this->TaskManager::ProcessErrorLogReport(g_reportErrorTask); });
		m_pThreadPerformance = new std::thread([this] { this->TaskManager::ProcessPerformanceReport(g_reportPerfTask); });
		LOGINFO("Init finished!");
		return 0;
	}


	int TaskManager::LoadThresholdConfig(const char* msg)
	{
		try
		{
			Json::Value data;
			int result = GetResponseData(msg, data);
			if (result != 0)
			{
				return ERROR_CODE_DATA_JSON;
			}
			//设置程序的RSA公钥
			std::string pubKeyID = data["pub_key_id"].asString();
			std::string pubKey = data["pub_key"].asString();
			result = APMReport::APMCryptogram::SetRSAPubKey(pubKeyID.c_str(), pubKey.c_str());
			if (result != 0)
			{
				return result;
			}

		}
		catch (const std::exception & e)
		{
			LOGERROR(e.what());
			return ERROR_CODE_INNEREXCEPTION;
		}
		return 0;
	}


	int TaskManager::LoadSwitch(const char* msg)
	{
		try
		{
			Json::Value data;
			int result = TaskManager::GetResponseData(msg, data);
			if (result != 0)
			{
				return result;
			}
			std::lock_guard<std::recursive_mutex> lck(m_reportMutex);
			if (!g_reportErrorTask.LoadSwitch(data))
			{
				return ERROR_CODE_DATA;
			}
			if (!g_reportPerfTask.LoadSwitch(data))
			{
				return ERROR_CODE_DATA;
			}
			return 0;
		}
		catch (const std::exception&)
		{
			LOGERROR("Load switch config failed!");
			return ERROR_CODE_INNEREXCEPTION;
		}
		return 0;
	}


	int TaskManager::GetResponseData(const char* msg, Json::Value& data)
	{
		Json::Value root;
		Json::Reader reader;
		if (!reader.parse(msg, root))
		{
			LOGERROR("Parse ResponseData to json failed!");
			return ERROR_CODE_DATA_JSON;
		}

		int statusCode = root["status_code"].asInt();
		auto statusMsg = root["status_msg"].asCString();
		if (statusCode != 0)
		{
			LOGERROR("ResponseData is error,status_msg: %s", statusMsg);
			return ERROR_CODE_DATA_JSON;
		}
		data = root["data"];
		return 0;
	}

	bool Task::LoadThresholdConfig(const Json::Value& root)
	{
		return true;
	}

	bool Task::LoadSwitch(const Json::Value& root)
	{
		std::string appID = root["app_id"].asCString();
		//总开关（0：关 1：开）
		int allSwitch = root["switch"].asInt();
		if (allSwitch == 0)
		{
			this->m_bCollectSwitch = false;
			this->m_bReportSwitch = false;
			return true;
		}
		int gatherSwitch = root["gather_switch"].asInt();
		if (gatherSwitch == 0)
		{
			this->m_bCollectSwitch = false;
		}
		//上报开关
		int upSwitch = root["up_switch"].asInt();
		if (upSwitch == 0)
		{
			this->m_bReportSwitch = false;
		}
		return true;
	}

	bool ReportErrorTask::LoadThresholdConfig(const Json::Value& root)
	{
		return true;
	}


	bool ReportPerformanceTask::LoadThresholdConfig(const Json::Value& root)
	{
		return false;
	}

	int TaskManager::AddTraceLog(const std::string& traceID, const std::string& moduleName, const std::string& subName, const std::string& result, const std::string& errorCode, int moduleType, const wchar_t* msg)
	{
		try
		{
			//考虑针对不同地域语言，统一转UTF-8编码
			std::wstring wstrMsg(msg);
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter2;
			std::string strMsg = converter2.to_bytes(wstrMsg);

			return AddTraceLog(traceID, moduleName, subName, result, errorCode, moduleType, strMsg);
		}
		catch (const std::exception & e)
		{
			LOGFATAL(e.what());
			return ERROR_CODE_INNEREXCEPTION;
		}
	}


	int TaskManager::AddTraceLog(const std::string& traceID, const std::string& moduleName, const std::string& subName, const std::string& result, const std::string& errorCode, int moduleType, const std::string& msg)
	{
		std::lock_guard<std::recursive_mutex> lck(m_reportMutex);
		//（后台）采集开关关闭
		if (!g_reportErrorTask.m_bCollectSwitch)
		{
			return ERROR_CODE_SWITCHOFF;
		}

		//超过最大缓存上报
		if (m_veclogMsgs.size() > g_reportErrorTask.m_config.m_nCacheMaxSize)
		{
			//触发立即上报
			UploadErrorLogData();
			return ERROR_CODE_OUTOFCACHE;
		}

		int build = BuildLogData(traceID, moduleName, subName, result, errorCode, moduleType, msg);
		if (build != 0)
		{
			return build;
		}


		//到达阈值条数上报
		if (m_veclogMsgs.size() > g_reportErrorTask.m_config.m_nSendCache)
		{
			UploadErrorLogData();
		}
		return 0;
	}

	int TaskManager::AddHTTPLog(const std::string& traceID, const std::string& moduleName, const std::string& url, const std::string& errorCode, int costTime, const wchar_t* msg)
	{
		try
		{
			std::wstring wstrMsg(msg);
			std::wstring_convert<std::codecvt_utf8<wchar_t>> converter2;
			std::string strMsg = converter2.to_bytes(wstrMsg);

			std::lock_guard<std::recursive_mutex> lck(m_reportMutex);

			//裁剪后的URL，用于后端计数
			std::string extractUrl = Util::ExtractURL(url);
			auto iter = m_mapUrls.find(extractUrl);
			if (iter == m_mapUrls.end())
			{
				m_mapUrls.insert_or_assign(extractUrl, 1);
			}
			else
			{
				m_mapUrls[extractUrl] = iter->second + 1;
			}

			if ((errorCode.empty() || errorCode == "0") && costTime < 3000)
			{
				return 0;
			}
			//（后台）采集开关关闭
			if (!g_reportErrorTask.m_bCollectSwitch)
			{
				return ERROR_CODE_SWITCHOFF;
			}

			//构建HTTP错误日志
			Json::Value span;
			span["logtime"] = Util::GetTimeNowStr();
			span["module"] = ConvertModuleText(DATA_MODULE_HTTP);
			span["trace_id"] = traceID;
			auto userInfo = User::GetUserInfo();
			span["userId"] = userInfo.m_sUserID;
			span["httpURL"] = url;
			if (costTime >= 3000)
			{
				span["error_type"] = "apm_http_slow_request";
			}
			else
			{
				span["error_type"] = "apm_http_error_request";
			}
			span["totalTime"] = costTime;
			span["url"] = extractUrl;
			span["errorCode"] = errorCode;
			span["business"] = moduleName;
			span["msg"] = msg;

			m_veclogMsgs.push_back(span);
			return 0;
		}
		catch (const std::exception & e)
		{
			LOGFATAL(e.what());
			return ERROR_CODE_INNEREXCEPTION;
		}
	}


	int TaskManager::BuildLogData(const std::string& traceID, const std::string& moduleName, const std::string& subName, const std::string& result, const std::string& errorCode, int moduleType, const std::string& msg)
	{
		//容错预防：上层客户端短时间内发送过量重复日志
		for (auto i : m_veclogMsgs)
		{
			//过滤重复日志
			if (i["msg"] == msg)
			{
				return ERROR_CODE_LOGREPEATED;
			}
		}

		Json::Value span;
		span["logtime"] = Util::GetTimeNowStr();
		span["module"] = ConvertModuleText(moduleType);
		span["trace_id"] = traceID;
		auto userInfo = User::GetUserInfo();
		//账户登录前的异常上报，此时允许userID为空
		if (userInfo.m_sUserID.empty())
		{
			LOGWARN("userID is empty,please invoke SetUserInfo() ");
			return ERROR_CODE_NULLUSERINFO;
		}
		span["userId"] = userInfo.m_sUserID;
		span["reslut"] = result;
		span["code"] = errorCode;
		span["business"] = moduleName;
		span["subname"] = subName;
		span["msg"] = msg;

		m_veclogMsgs.push_back(span);
		return 0;
	}

	void TaskManager::ProcessErrorLogReport(ReportErrorTask& task)
	{
		LOGINFO("ReportError Thread in.");
		//上传间隔（s）
		int interval = 0;
		while (!m_bThreadExit)
		{
			//最小发送间隔 可作为轮询间隔时间
			int sleepInterval = task.m_config.m_nSendMinInterval;
			std::this_thread::sleep_for(std::chrono::seconds(sleepInterval));

			if (!task.m_bReportSwitch)
			{
				continue;
			}
			if (task.m_config.m_bSendImmediately)
			{
				UploadErrorLogData();
				interval = 0;
			}
			if (interval >= task.m_config.m_nSendMaxInterval)
			{
				UploadErrorLogData();
				interval = 0;
			}

			interval += sleepInterval;
		}
		UploadErrorLogData();
	}


	void TaskManager::ProcessPerformanceReport(ReportPerformanceTask& task)
	{
		LOGINFO("ReportPerformance Thread in.");
		//上传间隔（s）
		int interval = 0;
		while (!m_bThreadExit)
		{
			//最小发送间隔 可作为轮询间隔时间
			int sleepInterval = task.m_config.m_nSendMinInterval;
			std::this_thread::sleep_for(std::chrono::seconds(sleepInterval));

			if (!task.m_bReportSwitch)
			{
				continue;
			}
			if (task.m_config.m_bSendImmediately)
			{
				UploadPerformanceData();
				interval = 0;
			}
			if (interval >= task.m_config.m_nSendMaxInterval)
			{
				UploadPerformanceData();
				interval = 0;
			}

			interval += sleepInterval;
		}
		UploadPerformanceData();
	}

	int TaskManager::UploadErrorLogData()
	{
		std::lock_guard<std::recursive_mutex> lck(m_reportMutex);
		if (m_veclogMsgs.size() == 0)
		{
			return 0;
		}
		//异常日志上报开关关了
		if (!g_reportErrorTask.m_bReportSwitch)
		{
			m_veclogMsgs.clear();
			return 0;
		}
		Json::Value root;
		int result = CreateRequestJson(root);
		if (result != 0)
		{
			return result;
		}
		for (auto msg : m_veclogMsgs)
		{
			root["msgs"].append(msg);
		}
		m_veclogMsgs.clear();

		auto jsonWriter = Json::FastWriter();
		jsonWriter.omitEndingLineFeed();

		//对日志数组进行压缩加密处理
		auto josnMsg = jsonWriter.write(root["msgs"]);
		std::string zipData = APMCryptogram::GzipCompress(josnMsg);
		std::string aesMsg;
		if (APMCryptogram::AesEncrypt(zipData, aesMsg) != 0)
		{
			return ERROR_CODE_DATA_ENCRYPT;
		}
		root["msgs"] = aesMsg;

		std::string output = jsonWriter.write(root);
		if (nullptr != m_funcPostErrorInfo)
		{
			m_funcPostErrorInfo(output.c_str(), output.length(), "", 0);
		}
		return 0;
	}

	int TaskManager::UploadPerformanceData()
	{
		std::lock_guard<std::recursive_mutex> lck(m_reportMutex);
		if (m_mapUrls.size() == 0)
		{
			return 0;
		}
		Json::Value root;
		int result = CreateRequestJson(root);
		if (result != 0)
		{
			return result;
		}

		int totalCount = 0;
		for (auto msg : m_mapUrls)
		{
			totalCount += msg.second;
			Json::Value jurl;
			jurl["name"] = "apm_client_http_request_url_count";
			jurl["type"] = 0;
			jurl["count"] = msg.second;
			jurl["sum"] = msg.second;
			Json::Value jItem;
			jItem["url"] = msg.first;
			jurl["label"] = jItem;
			root["metrics"].append(jurl);
		}
		m_mapUrls.clear();
		Json::Value jTotal;
		jTotal["name"] = "apm_client_http_request_count";
		jTotal["type"] = 0;
		jTotal["count"] = totalCount;
		jTotal["sum"] = totalCount;
		Json::Value lable;
		jTotal["label"] = lable;
		root["metrics"].append(jTotal);

		auto jsonWriter = Json::FastWriter();
		jsonWriter.omitEndingLineFeed();

		//对日志数组进行压缩加密处理
		auto josnMsg = jsonWriter.write(root["metrics"]);
		std::string zipData = APMCryptogram::GzipCompress(josnMsg);
		std::string aesMsg;
		if (APMCryptogram::AesEncrypt(zipData, aesMsg) != 0)
		{
			return ERROR_CODE_DATA_ENCRYPT;
		}
		root["metrics"] = aesMsg;

		std::string output = jsonWriter.write(root);
		if (nullptr != m_funcPostPerformance)
		{
			m_funcPostPerformance(output.c_str(), output.length(), "", 0);
		}
		return 0;
	}

	int TaskManager::CreateRequestJson(Json::Value& root)
	{
		auto baseInfo = Client::GetBaseInfoMap();
		auto iter = baseInfo.begin();
		if (iter == baseInfo.end())
		{
			LOGERROR("appID can not find, please set clientInfo first.");
			return ERROR_CODE_NULLCLIENTINFO;
		}
		root["app_id"] = iter->first;
		root["d_uuid"] = Client::GetDeviceUUID();
		std::string keyID, pubKey;
		if (APMCryptogram::GetRSAPubKey(keyID, pubKey) < 0)
		{
			return ERROR_CODE_DATA_NULLKEY;
		}
		root["key_id"] = keyID;
		root["a_key"] = APMCryptogram::g_cipherAESKey;
		root["base_md5"] = iter->second;
		return 0;
	}

	void TaskManager::Stop()
	{
		m_bThreadExit = true;
	}

	std::string TaskManager::ConvertModuleText(int moduleType)
	{
		switch (moduleType)
		{
		case DATA_MODULE_UNKNOW:
		case DATA_MODULE_CPU_MEMORY:
			return "pc";
		case DATA_MODULE_CATON:
			return "pc-caton";
		case DATA_MODULE_CRASH:
			return "pc-crash";
		case DATA_MODULE_HTTP:
			return "pc-http";
		case DATA_MODULE_TCP:
			return "pc-tcp";
		case DATA_MODULE_WEB:
			return "pc-web";
		default:
			break;
		}
		return "pc";
	}

}