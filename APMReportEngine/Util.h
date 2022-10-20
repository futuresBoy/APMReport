/*
	APM工具类
*/

#pragma warning( disable : 4996 )

#include <string>
#include <time.h>

namespace APMReport
{
	class Util
	{
	public:
		/*
		功能：获取随机生成的UUID
		返回：随机唯一字符串
		*/
		static std::string GetRandomUUID();

		/*
		功能：获取当前时间字符串
		返回：标准格式时间
		*/
		static std::string GetTimeNowStr();

		/*
		功能：截取有效的URL字符串
		返回：截取后的URL字符串
		*/
		static std::string ExtractURL(const std::string& url);

	};

}