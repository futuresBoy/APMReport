#include "Logger.h"
#include "APMBasic.h"
#include "APMCryptogram.h"
#include "APMReportProcess.h"
#include "Util.h"
#include "ClientManager.h"
#include "User.h"
#include <codecvt>

namespace APMReport
{
	//上报异常信息任务
	ReportErrorTask g_reportErrorTask;

	//上传性能信息任务
	ReportPerformanceTask g_reportPerfTask;

	TaskProcess::TaskProcess()
	{
		m_bThreadExit = false;
		m_pThreadErrorLog = nullptr;
		m_pThreadPerformance = nullptr;
	}

	APMReport::TaskProcess::~TaskProcess()
	{
		m_bThreadExit = true;
		if (nullptr != m_pThreadErrorLog)
		{
			if (m_pThreadErrorLog->joinable())
			{
				m_pThreadErrorLog->join();
			}
			delete m_pThreadErrorLog;
			m_pThreadErrorLog = nullptr;
		}
		if (nullptr != m_pThreadPerformance)
		{
			if (m_pThreadPerformance->joinable())
			{
				m_pThreadPerformance->join();
			}
			delete m_pThreadPerformance;
			m_pThreadPerformance = nullptr;
		}
	}

	int APMReport::TaskProcess::Init(std::string appID, PostErrorLogFunc funcPostErrorInfo, PostPerformanceFunc funcPostPerformanceInfo)
	{
		if (nullptr == funcPostErrorInfo || nullptr == funcPostPerformanceInfo)
		{
			LOGERROR("PostErrorLogFunc or PostPerformanceFunc is Null.");
			return ERROR_CODE_PARAMS;
		}
		m_appID = appID;
		m_funcPostErrorInfo = funcPostErrorInfo;
		m_funcPostPerformance = funcPostPerformanceInfo;
		m_pThreadErrorLog = new std::thread([this] { this->TaskProcess::ProcessErrorLogReport(g_reportErrorTask); });

		return 0;
	}

	int TaskProcess::LoadThresholdConfig(const char* msg)
	{
		try
		{
			Json::Value data;
			int result = GetResponseData(msg, data);
			if (result != 0)
			{
				return result;
			}
			//设置程序的RSA公钥
			std::string pubKeyID = data["pub_key_id"].asString();
			std::string pubKey = data["pub_key"].asString();
			return APMReport::APMCryptogram::SetRSAPubKey(pubKeyID.c_str(), pubKey.c_str());
		}
		catch (const std::exception & e)
		{
			LOGERROR(e.what());
			return ERROR_CODE_INNEREXCEPTION;
		}
		return 0;
	}


	int TaskProcess::LoadSwitch(const char* msg)
	{
		try
		{
			Json::Value data;
			int result = TaskProcess::GetResponseData(msg, data);
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


	int TaskProcess::GetResponseData(const char* msg, Json::Value& data)
	{
		if (CHECK_ISNULLOREMPTY(msg))
		{
			LOGERROR("Response data is null or empty.");
			return ERROR_CODE_DATA_JSON;
		}
		try
		{
			Json::Value root;
			Json::Reader reader;
			if (!reader.parse(msg, root))
			{
				LOGERROR("Parse ResponseData to json failed! %s", msg);
				return ERROR_CODE_DATA_JSON;
			}

			int statusCode = root["status_code"].asInt();
			if (statusCode != 0)
			{
				auto statusMsg = root["status_msg"].asCString();
				LOGERROR("ResponseData is error,status_msg: %s", statusMsg);
				return ERROR_CODE_DATA_JSON;
			}
			data = root["data"];
		}
		catch (const std::exception & e)
		{
			LOGERROR(e.what());
			LOGINFO(msg);
			return ERROR_CODE_DATA_JSON;
		}
		return 0;
	}

	bool Task::LoadThresholdConfig(const Json::Value& root)
	{
		return true;
	}

	bool Task::LoadSwitch(const Json::Value& root)
	{
		try
		{
			std::string appID = root["app_id"].asCString();
			//总开关（0：关 1：开）
			int allSwitch = root["switch"].asInt();
			if (allSwitch == 0)
			{
				this->m_bCollectSwitch = this->m_bReportSwitch = false;
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
		}
		catch (const std::exception & e)
		{
			LOGERROR(e.what());
			//记录解析异常的json
			Json::FastWriter writer;
			LOGINFO(writer.write(root).c_str());
			return false;
		}
		return true;
	}


	int TaskProcess::AddTraceLog(const std::string& module, const std::string& logType, const std::string& bussiness, const std::string& subName, const std::string& errorCode, const std::string& msg, const std::string& extData)
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

	int TaskProcess::AddHTTPLog(const std::string& logType, const std::string& bussiness, const std::string& url, const std::string& errorCode, int costTime, const std::string& msg, const std::string& extData)
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
				m_pThreadPerformance = new std::thread([this] { this->TaskProcess::ProcessPerformanceReport(g_reportPerfTask); });
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
		}
		catch (const std::exception& e)
		{
			LOGFATAL(e.what());
			return ERROR_CODE_INNEREXCEPTION;
		}
		return 0;
	}


	int TaskProcess::BuildLogData(const std::string& moduleName, const std::string& logType, const std::string& bussiness, const std::string& subName, const std::string& errorCode, const std::string& msg, const std::string& extData)
	{
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

	int TaskProcess::GenerateRoot(const std::string& msg, Json::Value& root)
	{
		//上层未传信息，不作处理，正常返回
		if (msg.empty())
		{
			return 0;
		}
		try
		{
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
			auto userInfoEx = User::GetUserInfoEx(m_appID);
			if (userInfoEx.empty())
			{
				auto userInfo = User::GetUserInfo(m_appID);
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
		}
		catch (const std::exception& e)
		{
			LOGERROR(e.what());
			return ERROR_CODE_INNEREXCEPTION;
		}
		return 0;
	}

	void TaskProcess::ProcessErrorLogReport(ReportErrorTask& task)
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
			if (task.m_config.m_bSendImmediately || interval >= task.m_config.m_nSendMaxInterval)
			{
				UploadErrorLogData();
				interval = 0;
			}

			interval += sleepInterval;
		}
	}


	void TaskProcess::ProcessPerformanceReport(ReportPerformanceTask& task)
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
			if (task.m_config.m_bSendImmediately || interval >= task.m_config.m_nSendMaxInterval)
			{
				UploadPerformanceData();
				interval = 0;
			}

			interval += sleepInterval;
		}
	}

	int TaskProcess::UploadErrorLogData()
	{
		Json::Value root;
		{
			std::lock_guard<std::recursive_mutex> lck(m_reportMutex);
			if (m_veclogMsgs.size() == 0)
			{
				LOGINFO("ProcessErrorLog...");
				return 0;
			}
			//异常日志上报开关关了
			if (!g_reportErrorTask.m_bReportSwitch)
			{
				LOGINFO("ProcessErrorLog...ReportSwitch Off");
				m_veclogMsgs.clear();
				return 0;
			}
			std::string localLog = "ProcessErrorLog...Count:" + std::to_string(m_veclogMsgs.size());
			LOGINFO(localLog.c_str());

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

	int TaskProcess::UploadPerformanceData()
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

	int TaskProcess::CreateRequestJson(Json::Value& root)
	{
		std::string baseInfo = ClientManager::GetBaseInfo(m_appID);
		if (baseInfo.empty())
		{
			LOGERROR("appID can not find, please set clientInfo first.");
			return ERROR_CODE_NULLCLIENTINFO;
		}
		root["app_id"] = m_appID;
		root["d_uuid"] = ClientManager::GetDeviceUUID(m_appID);
		std::string keyID, pubKey;
		if (APMCryptogram::GetRSAPubKey(keyID, pubKey) < 0)
		{
			return ERROR_CODE_DATA_NULLKEY;
		}
		root["key_id"] = keyID;
		root["a_key"] = APMCryptogram::g_cipherAESKey;
		root["base_md5"] = baseInfo;
		return 0;
	}

	void TaskProcess::Stop()
	{
		m_bThreadExit = true;
		UploadErrorLogData();
		UploadPerformanceData();
	}

	std::string TaskProcess::ConvertModuleText(int moduleType)
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