#include	"APMReport.h"

void InitLog(LogFunc logger);

void AddLog(LogLevel level, const char* file, const char* func, unsigned int line, const char* msg, ...);
#define LOGDEBUG(msg, ...) AddLog(LOG_DEBUG, __FILE__, __FUNCTION__, __LINE__, msg, ##__VA_ARGS__)
#define LOGINFO(msg, ...) AddLog(LOG_INFO, __FILE__, __FUNCTION__, __LINE__, msg ,##__VA_ARGS__)
#define LOGWARN(msg, ...) AddLog(LOG_WARN, __FILE__, __FUNCTION__, __LINE__, msg,##__VA_ARGS__)
#define LOGERROR(msg, ...) AddLog(LOG_ERROR, __FILE__, __FUNCTION__, __LINE__, msg,##__VA_ARGS__)
#define LOGFATAL(msg, ...) AddLog(LOG_FATAL, __FILE__, __FUNCTION__, __LINE__, msg,##__VA_ARGS__)
