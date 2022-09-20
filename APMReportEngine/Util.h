/*
	�����࣬����APM������ݵĽ�����ѹ�������ܵȲ���
*/

#pragma warning( disable : 4996 )

#include <base64.h>
#include <aes.h>
#include <rsa.h>
#include <modes.h>
#include <randpool.h>
#include <osrng.h>
#include <hex.h>
#include <gzip.h>
#include "Logger.h"

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <md5.h>

using namespace CryptoPP;
#pragma comment(lib, "cryptlib.lib")

namespace APMReport
{
	class Util
	{

	private:
		static std::string g_AESKey;
	public:
		static std::string g_cipherAESKey;
	public:
		/*
		���ܣ���ȡ��ǰʱ���ַ���
		���أ���׼��ʽʱ��
		*/
		static std::string GetTimeNowStr();

		/*
			���ܣ�����RSA��Կ
			������pubKeyID ��Կ���
			������pubKey ��Կ�ַ���
			����ֵ��0 �ɹ���-1 ��������ȷ
		*/
		static int SetRSAPubKey(const char* pubKeyID, const char* pubKey);

		/*
			���ܣ���ȡ��Կ
			������keyID ��Կ���
			������pubKey ��Կ�ַ�
			����ֵ��0 �ɹ���-1 ��ȡʧ�ܣ�û�й�Կ����
		*/
		static int GetRSAPubKey(std::string& keyID, std::string& pubKey);

		/*
			���ܣ���ȡ��ǰAES��Կ
		*/
		static std::string GetAESKey();

		/*
			���ܣ�RSA(��Կ)����
			������plain �����ܵ�����
			����ֵ�����ܺ������
		*/
		static std::string RSAEncrypt(std::string plain);

		/*
			���ܣ�ʹ��AES(CBCģʽ)����
			������plainText �����ܵ������ַ���
			������cipherText ��������ģ�base64���룩
			������outLen ��������ĳ���
			����ֵ��0 �ɹ���-1 ����ʧ��
		*/
		static int AesEncrypt(const char* plainText, char* cipherText, int& outLen);

		/*
			���ܣ�ʹ��AES(CBCģʽ)����
			������plainText �����ܵ������ַ���
			������cipherStr ��������ģ�base64���룩
			����ֵ��0 �ɹ���-1 ����ʧ��
		*/
		static int AesEncrypt(std::string plainText, std::string& cipherStr);

		/*
			���ܣ�MD5
			������msg ��Ҫ����MD5���ַ���
			����ֵ��MD5����ַ���
		*/
		static std::string MD5(std::string msg);

		/*
			���ܣ�����ָ�����ȵ�����ַ���
			size: �ַ����ĳ���
			printable���Ƿ�ɴ�ӡ���������key������ʹ�ò��ɴ�ӡ���ַ�
		*/
		static std::string GenerateRandStr(int size, bool printable);

		/*
			���ܣ�Gzipѹ��
			������data ��Ҫѹ��������
			����ֵ��ѹ���������
		*/
		static std::string GzipCompress(std::string& data);
	};

}