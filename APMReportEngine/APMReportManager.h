#pragma once
#include "Logger.h"
#include "APMBasic.h"
#include "APMReportProcess.h"

namespace APMReport
{
	/*
		用途：管理日志上报任务，用于同时支持多个不同的appID进行日志上报，互相隔离不受影响
	*/
	class APMReportManager
	{
	private:
		//存放不同appID对应的日志上报任务
		static std::map<std::string, APMReport::TaskProcess&> g_manager;

	public:
		APMReportManager();
		~APMReportManager();

		/*判断该appID是否有对应的日志上报任务*/
		static bool Exist(std::string appID);

		/*获取appID对应的日志上报任务*/
		static TaskProcess& Get(std::string appID);

		/*1.初始化*/
		static int APMInit(
			PostErrorLogFunc funcPostErrorInfo,		//通知上传错误信息
			PostPerformanceFunc funcPostPerformance	//通知上传异常信息
		);

		/*关闭指定appID的日志上报*/
		static bool Close(std::string appID);

		/*关闭所有的日志上报*/
		static bool Close();
	};

}


