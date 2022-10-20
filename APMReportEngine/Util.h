/*
	APM������
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
		���ܣ���ȡ������ɵ�UUID
		���أ����Ψһ�ַ���
		*/
		static std::string GetRandomUUID();

		/*
		���ܣ���ȡ��ǰʱ���ַ���
		���أ���׼��ʽʱ��
		*/
		static std::string GetTimeNowStr();

		/*
		���ܣ���ȡ��Ч��URL�ַ���
		���أ���ȡ���URL�ַ���
		*/
		static std::string ExtractURL(const std::string& url);

	};

}