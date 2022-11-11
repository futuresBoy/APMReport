#include <stdio.h>
#include "Logger.h"
#ifdef  WIN32
#include <Windows.h>
#endif //  WIN32

#define	MAX_LOGGER_LENGHT 1024

static LogFunc g_logger = nullptr;
static LogLevel g_level = LOG_WARN;

void InitLog(LogFunc logger)
{
	if (nullptr == logger)
	{
		return;
	}
	g_logger = logger;
}

static const char* GetLogLevelText(LogLevel level)
{
	const char* loggerLevel = "logger";
	switch (level)
	{
	case LOG_DEBUG:
		loggerLevel = "debug";
		break;
	case LOG_INFO:
		loggerLevel = "info";
		break;
	case LOG_WARN:
		loggerLevel = "warn";
		break;
	case LOG_ERROR:
		loggerLevel = "error";
		break;
	case LOG_FATAL:
		loggerLevel = "fatal";
		break;
	default:
		break;
	}
	return loggerLevel;
}

static const char* GetFileNameWithoutPath(const char* file)
{
	if (nullptr == file)
	{
		return "";
	}
	const char* tmp = file;
	tmp = file + strlen(file);
	while (tmp != file)
	{
		if ('\\' == *tmp || '/' == *tmp || ':' == *tmp)
		{
			tmp++;
			break;
		}
		tmp--;
	}
	return tmp;
}

void AddLog(LogLevel level, const char* file, const char* func, unsigned int line, const char* msg, ...)
{
	if (nullptr == g_logger || nullptr == msg)
	{
		return;
	}
	file = GetFileNameWithoutPath(file);

	char buf[MAX_LOGGER_LENGHT];
	int len = snprintf(buf, MAX_LOGGER_LENGHT, "[%s] [%s %s %u] %s", GetLogLevelText(level), file, func ? func : "", line, msg);
	g_logger(buf, level);
}


