#include "Logger.h"
#include "APMBasic.h"
#include "APMReportManager.h"
#include "Util.h"
#include "BasicInfo.h"

namespace APMReport
{
	static TaskManager g_manager;

	//�ϱ��쳣��Ϣ����
	ReportErrorTask g_reportTask;

	TaskManager& APMReport::TaskManager::GetInstance()
	{
		return g_manager;
	}

	APMReport::TaskManager::~TaskManager()
	{
		if (m_pThread)
		{
			m_bInited = false;
			m_bThreadExit = true;
			m_pThread->join();
			delete m_pThread;
			m_pThread = nullptr;
		}
	}


	int APMReport::TaskManager::APMInit(PostErrorLogFunc funcPostErrorInfo)
	{
		if (nullptr == funcPostErrorInfo)
		{
			return ERROR_CODE_PARAMS;
		}
		m_funcPostErrorInfo = funcPostErrorInfo;
		if (m_bInited || m_pThread != nullptr)
		{
			LOGWARN("Inited Aready!");
			return 0;
		}
		m_bInited = true;
		m_bThreadExit = false;
		m_pThread = new std::thread();
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
			//���ó����RSA��Կ
			std::string pubKeyID = data["pub_key_id"].asString();
			std::string pubKey = data["pub_key"].asString();
			result = APMReport::Util::SetRSAPubKey(pubKeyID.c_str(), pubKey.c_str());
			if (result != 0)
			{
				return result;
			}

		}
		catch (const std::exception& e)
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
			if (g_reportTask.LoadSwitch(data))
			{
				return 0;
			}
			return ERROR_CODE_DATA;
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
		//�ܿ��أ�0���� 1������
		int allSwitch = root["switch"].asInt();
		return allSwitch != 0;
	}

	bool ReportErrorTask::LoadThresholdConfig(const Json::Value& root)
	{
		return true;
	}

	bool ReportErrorTask::LoadSwitch(const Json::Value& root)
	{
		bool allSwitch = Task::LoadSwitch(root);
		//�ӿ��ܿ��عر�
		if (!allSwitch)
		{
			g_reportTask.m_bCollectSwitch = false;
			g_reportTask.m_bReportSwitch = false;
		}
		int gatherSwitch = root["gather_switch"].asInt();
		if (gatherSwitch == 0)
		{
			g_reportTask.m_bCollectSwitch = false;
		}
		//�ϱ�����
		int upSwitch = root["up_switch"].asInt();
		if (upSwitch == 0)
		{
			g_reportTask.m_bReportSwitch = false;
		}
		return true;
	}


	int TaskManager::AddErrorLog(const char* logMessage)
	{
		std::string strMessage(logMessage);
		std::lock_guard<std::recursive_mutex> lck(m_reportMutex);
		//�ݴ���1.Ԥ���ϲ�ͻ��˶�ʱ�����ظ����ʹ�����־
		for (auto i : m_veclogMsgs)
		{
			//�����ظ���־
			if (i == strMessage)
			{
				return ERROR_CODE_LOGREPEATED;
			}
		}
		m_veclogMsgs.push_back(strMessage);
		return 0;
	}

	int TaskManager::AddTraceLog(const std::string& traceID, const std::string& moduleName, const std::string& subName, const std::string& result, const std::string& errorCode, int monitorType, const char* msgArray, int* msgLengthArray, int arrayCount)
	{
		try
		{
			//ת���������־����Ϊ�ַ����б�
			std::string strMsgs(msgArray);
			std::vector<std::string> vecMsg;
			int j = 0;
			for (int i = 0; i < arrayCount; i++)
			{
				vecMsg.push_back(strMsgs.substr(j, msgLengthArray[i]));
				j += msgLengthArray[i];
			}
			return AddTraceLog(traceID, moduleName, subName, result, errorCode, monitorType, vecMsg);
		}
		catch (const std::exception& e)
		{
			LOGFATAL(e.what());
			return ERROR_CODE_INNEREXCEPTION;
		}
	}


	int TaskManager::AddTraceLog(const std::string& traceID, const std::string& moduleName, const std::string& subName, const std::string& result, const std::string& errorCode, int monitorType, const std::vector<std::string>& msgs)
	{
		std::lock_guard<std::recursive_mutex> lck(m_reportMutex);
		//����̨���ɼ����عر�
		if (!g_reportTask.m_bCollectSwitch)
		{
			return ERROR_CODE_SWITCHOFF;
		}

		//������󻺴��ϱ�
		if (m_veclogMsgs.size() > g_reportTask.m_config.m_nCacheMaxSize)
		{
			//���������ϱ�
			UploadLogMessage();
			return ERROR_CODE_OUTOFCACHE;
		}
		BuidLogData(traceID, moduleName, subName, result, errorCode, monitorType, msgs);
		//������ֵ�����ϱ�
		if (m_veclogMsgs.size() > g_reportTask.m_config.m_nSendCache)
		{
			UploadLogMessage();
		}
		return 0;
	}


	void TaskManager::BuidLogData(const std::string& traceID, const std::string& moduleName, const std::string& subName, const std::string& result, const std::string& errorCode, int monitorType, const std::vector<std::string>& msgs)
	{
		Json::Value span;
		span["logtime"] = Util::GetTimeNowStr();
		span["module"] = monitorType;
		span["trace_id"] = traceID;
		//�ͻ���Ϊ���𷽣�spanidΪ0
		span["span_id"] = "0";
		auto userInfo = User::GetUserInfo();
		span["userId"] = userInfo.m_sUserID;
		span["reslut"] = result;
		span["code"] = errorCode;
		span["businessname"] = moduleName;
		span["subname"] = subName;
		for (int i = 0; i < msgs.size(); i++)
		{
			span["output_0"].append(msgs[i]);
		}
		auto jsonWriter = Json::FastWriter();
		jsonWriter.omitEndingLineFeed();
		std::string jsonStr = jsonWriter.write(span);
		m_veclogMsgs.push_back(jsonStr);
	}

	void TaskManager::ProcessLogDataReport(Task task)
	{
		LOGINFO("Thread in.");
		//�ϴ������s��
		int interval = 0;
		while (!m_bThreadExit)
		{
			//��С���ͼ�� ����Ϊ��ѯ���ʱ��
			int sleepInterval = task.m_config.m_nSendMinInterval;
			std::this_thread::sleep_for(std::chrono::seconds(sleepInterval));

			if (!task.m_bReportSwitch)
			{
				continue;
			}
			if (task.m_config.m_bSendImmediately)
			{
				UploadLogMessage();
				interval = 0;
			}
			if (interval >= task.m_config.m_nSendMaxInterval)
			{
				UploadLogMessage();
				interval = 0;
			}

			interval += sleepInterval;
		}
		UploadLogMessage();
	}


	int TaskManager::UploadLogMessage()
	{
		std::lock_guard<std::recursive_mutex> lck(m_reportMutex);
		if (m_veclogMsgs.size() == 0)
		{
			return 0;
		}
		Json::Value root;
		root["app_id"] = "";
		root["d_uuid"] = g_deviceUUID;
		std::string keyID, pubKey;
		if (Util::GetRSAPubKey(keyID, pubKey) < 0)
		{
			return ERROR_CODE_DATA_NULLKEY;
		}
		root["key_id"] = keyID;
		root["a_key"] = pubKey;

		auto iter = g_mapAppBaseInfoMD5.find("app_id");
		if (iter == g_mapAppBaseInfoMD5.end())
		{
			LOGERROR("appID can not find, please set clientInfo first.");
			return ERROR_CODE_NULLCLIENTINFO;
		}
		root["base_md5"] = iter->second;
		for (auto msg : m_veclogMsgs)
		{
			root["msgs"].append(msg);
		}
		auto jsonWriter = Json::FastWriter();
		jsonWriter.omitEndingLineFeed();
		std::string output = jsonWriter.write(root);
		if (nullptr != m_funcPostErrorInfo)
		{
			m_funcPostErrorInfo(output.c_str(), output.length(), "");
		}
		return 0;
	}
}