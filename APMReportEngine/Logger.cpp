#include "Logger.h"

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

void AddLog(const char* msg)
{

}

void AddLog(LogLevel level, const char* file, const char* func, unsigned int line, const char* msg, ...)
{
	if (nullptr == g_logger)
	{
		return;
	}
	g_logger(msg, level);
}

