
#include "Util.h"
#include "random_generator.h"

namespace APMReport
{

	std::string Util::GetRandomUUID()
	{
		RandomGeneratorImpl random;
		return random.uuid();
	}

	std::string Util::GetTimeNowStr()
	{
		time_t t = time(0);
		char tmp[32] = { NULL };
		strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));
		return std::string(tmp);
	}

	std::string Util::ExtractURL(const std::string& url)
	{
		if (url.empty())
		{
			return "";
		}
		int index = url.find_first_of('?', 0);
		return url.substr(0, index);
	}

}

