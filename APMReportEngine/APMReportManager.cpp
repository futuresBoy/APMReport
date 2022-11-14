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


	int TaskManager::AddTraceLog(const std::string& module, const std::string& logType, const std::string& bussiness, const std::string& subName, const std::string& errorCode, const std::string& msg, const std::string& extData)
	{
		std::lock_guard<std::recursive_mutex> lck(m_reportMutex);
		//（后台）采集开关关闭
		if (!g_reportErrorTask.m_bCollectSwitch)
		{
			if (m_veclogMsgs.size() > 0)
			{
				m_veclogMsgs.clear();
			}
			return ERROR_CODE_SWITCHOFF;
		}

		//超过最大缓存上报
		if (m_veclogMsgs.size() >= g_reportErrorTask.m_config.m_nCacheMaxSize)
		{
			//触发立即上报
			UploadErrorLogData();
			return ERROR_CODE_OUTOFCACHE;
		}

		int result = BuildLogData(module, logType, bussiness, subName, errorCode, msg, extData);
		if (result != 0)
		{
			return result;
		}

		//到达阈值条数上报
		if (m_veclogMsgs.size() >= g_reportErrorTask.m_config.m_nSendCount)
		{
			UploadErrorLogData();
		}
		return 0;
	}

	int TaskManager::AddHTTPLog(const std::string& logType, const std::string& bussiness, const std::string& url, const std::string& errorCode, int costTime, const std::string& msg, const std::string& extData)
	{
		try
		{
			std::lock_guard<std::recursive_mutex> lck(m_reportMutex);
			//（后台）采集开关关闭
			if (!g_reportErrorTask.m_bCollectSwitch)
			{
				return ERROR_CODE_SWITCHOFF;
			}

			//需要用到性能上报线程时再创建
			if (m_pThreadPerformance == nullptr)
			{
				m_pThreadPerformance = new std::thread([this] { this->TaskManager::ProcessPerformanceReport(g_reportPerfTask); });
			}

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
			
			//构建HTTP错误日志
			Json::Value root;
			if (GenerateRoot(msg, root) != 0)
			{
				return ERROR_CODE_DATA_JSON;
			}
			root["logtime"] = Util::GetTimeNowStr();
			root["module"] = ConvertModuleText(DATA_MODULE_HTTP);

			root["httpURL"] = url;
			if (logType.empty())
			{
				root["error_type"] = costTime >= 3000 ? "apm_http_slow_request" : "apm_http_error_request";
			}
			else
			{
				root["error_type"] = logType;
			}
			root["business"] = bussiness;
			root["totalTime"] = costTime;
			root["url"] = extractUrl;
			root["errorCode"] = errorCode;

			Json::Reader readerExt;
			Json::Value extJson;
			if (!extData.empty() && readerExt.parse(extData, extJson))
			{
				root["extData"] = extJson;
			}

			m_veclogMsgs.push_back(root);
			return 0;
		}
		catch (const std::exception & e)
		{
			LOGFATAL(e.what());
			return ERROR_CODE_INNEREXCEPTION;
		}
	}


	int TaskManager::BuildLogData(const std::string& moduleName, const std::string& logType, const std::string& bussiness, const std::string& subName, const std::string& errorCode, const std::string& msg, const std::string& extData)
	{
		////容错预防：上层客户端短时间内发送过量重复日志
		//for (auto i : m_veclogMsgs)
		//{
		//	//过滤重复日志
		//	if (i["msg"] == msg)
		//	{
		//		return ERROR_CODE_LOGREPEATED;
		//	}
		//}

		Json::Value root;
		if (GenerateRoot(msg, root) != 0)
		{
			return ERROR_CODE_DATA_JSON;
		}

		root["logtime"] = Util::GetTimeNowStr();
		root["module"] = moduleName.empty() ? "pc" : moduleName;
		root["error_type"] = logType.empty() ? "error" : logType;
		root["business"] = bussiness;
		root["subname"] = subName;
		root["errCode"] = errorCode;

		Json::Reader readerExt;
		Json::Value extJson;
		if (!extData.empty() && readerExt.parse(extData, extJson))
		{
			root["extData"] = extJson;
		}

		m_veclogMsgs.push_back(root);
		return 0;
	}

	int TaskManager::GenerateRoot(const std::string& msg, Json::Value& root)
	{
		if (msg.empty())
		{
			return 0;
		}
		//区分普通字符串和Json字符串
		if (msg[0] != '{')
		{
			root["msg"] = msg;
		}
		else
		{
			Json::Reader reader;
			if (!reader.parse(msg, root))
			{
				return ERROR_CODE_DATA_JSON;
			}
		}
		//赋值用户信息
		auto userInfoEx = User::GetUserInfoEx();
		if (userInfoEx.empty())
		{
			auto userInfo = User::GetUserInfo();
			root["userId"] = userInfo.m_sUserID;
		}
		else
		{
			Json::Value::Members member = userInfoEx.getMemberNames();
			for (Json::Value::Members::iterator iter = member.begin(); iter != member.end(); ++iter)
			{
				std::string name = *iter;
				std::string value = userInfoEx[name].asString();
				root[name] = value;
			}
		}
		return 0;
	}

	void TaskManager::ProcessErrorLogReport(ReportErrorTask& task)
	{
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
		Json::Value root;
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

			int result = CreateRequestJson(root);
			if (result != 0)
			{
				LOGERROR("UploadErrorLog...CreateRequestJson Error");
				return result;
			}
			for (auto msg : m_veclogMsgs)
			{
				root["msgs"].append(msg);
			}
			m_veclogMsgs.clear();
		}

		auto jsonWriter = Json::FastWriter();
		jsonWriter.omitEndingLineFeed();

		//对日志数组进行压缩加密处理
		auto josnMsg = jsonWriter.write(root["msgs"]);
		std::string zipData = APMCryptogram::GzipCompress(josnMsg);
		std::string aesMsg;
		if (APMCryptogram::AesEncrypt(zipData, aesMsg) != 0)
		{
			LOGERROR("UploadErrorLog...Encrypt Error");
			return ERROR_CODE_DATA_ENCRYPT;
		}
		root["msgs"] = aesMsg;

		std::string output = jsonWriter.write(root);
		if (nullptr != m_funcPostErrorInfo)
		{
			LOGINFO("UploadErrorLog...");
			m_funcPostErrorInfo(output.c_str(), output.length(), "", 0);
		}
		return 0;
	}

	int TaskManager::UploadPerformanceData()
	{
		Json::Value root;
		int totalCount = 0;
		{
			std::lock_guard<std::recursive_mutex> lck(m_reportMutex);
			if (m_mapUrls.size() == 0)
			{
				return 0;
			}

			int result = CreateRequestJson(root);
			if (result != 0)
			{
				return result;
			}

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
		}

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