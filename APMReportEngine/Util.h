/*
	工具类，用于APM相关数据的解析、压缩、加密等操作
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
		功能：获取当前时间字符串
		返回：标准格式时间
		*/
		static std::string GetTimeNowStr();

		/*
			功能：设置RSA公钥
			参数：pubKeyID 公钥编号
			参数：pubKey 公钥字符串
			返回值：0 成功，-1 参数不正确
		*/
		static int SetRSAPubKey(const char* pubKeyID, const char* pubKey);

		/*
			功能：获取公钥
			参数：keyID 公钥编号
			参数：pubKey 公钥字符
			返回值：0 成功，-1 获取失败，没有公钥生成
		*/
		static int GetRSAPubKey(std::string& keyID, std::string& pubKey);

		/*
			功能：获取当前AES密钥
		*/
		static std::string GetAESKey();

		/*
			功能：RSA(公钥)加密
			参数：plain 待加密的明文
			返回值：加密后的密文
		*/
		static std::string RSAEncrypt(std::string plain);

		/*
			功能：使用AES(CBC模式)加密
			参数：plainText 待加密的明文字符串
			参数：cipherText 输出的密文（base64编码）
			参数：outLen 输出的密文长度
			返回值：0 成功，-1 加密失败
		*/
		static int AesEncrypt(const char* plainText, char* cipherText, int& outLen);

		/*
			功能：使用AES(CBC模式)加密
			参数：plainText 待加密的明文字符串
			参数：cipherStr 输出的密文（base64编码）
			返回值：0 成功，-1 加密失败
		*/
		static int AesEncrypt(std::string plainText, std::string& cipherStr);

		/*
			功能：MD5
			参数：msg 需要进行MD5的字符串
			返回值：MD5后的字符串
		*/
		static std::string MD5(std::string msg);

		/*
			功能：生成指定长度的随机字符串
			size: 字符串的长度
			printable：是否可打印。如果用作key，可以使用不可打印的字符
		*/
		static std::string GenerateRandStr(int size, bool printable);

		/*
			功能：Gzip压缩
			参数：data 需要压缩的数据
			返回值：压缩后的数据
		*/
		static std::string GzipCompress(std::string& data);
	};

}