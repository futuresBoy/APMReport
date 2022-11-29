#pragma once
#include "Logger.h"
#include "APMBasic.h"
#include "APMReportProcess.h"

namespace APMReport
{
	/*
		��;��������־�ϱ���������ͬʱ֧�ֶ����ͬ��appID������־�ϱ���������벻��Ӱ��
	*/
	class APMReportManager
	{
	private:
		//��Ų�ͬappID��Ӧ����־�ϱ�����
		static std::map<std::string, APMReport::TaskProcess&> g_manager;

	public:
		APMReportManager();
		~APMReportManager();

		/*�жϸ�appID�Ƿ��ж�Ӧ����־�ϱ�����*/
		static bool Exist(std::string appID);

		/*��ȡappID��Ӧ����־�ϱ�����*/
		static TaskProcess& Get(std::string appID);

		/*1.��ʼ��*/
		static int APMInit(
			PostErrorLogFunc funcPostErrorInfo,		//֪ͨ�ϴ�������Ϣ
			PostPerformanceFunc funcPostPerformance	//֪ͨ�ϴ��쳣��Ϣ
		);

		/*�ر�ָ��appID����־�ϱ�*/
		static bool Close(std::string appID);

		/*�ر����е���־�ϱ�*/
		static bool Close();
	};

}


