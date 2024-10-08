/*
	APM密码安全类，提供包括Hash、加解密等操作
*/
#pragma once

#include <base64.h>
#include <aes.h>
#include <rsa.h>
#include <modes.h>
#include <randpool.h>
#include <osrng.h>
#include <hex.h>
#include <random>
#include <gzip.h>
#include "Logger.h"

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <md5.h>

using namespace CryptoPP;
#pragma comment(lib, "cryptlib.lib")

namespace APMReport
{
	// 采样字符集
	static constexpr char CCH[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

	class APMCryptogram
	{
	private:
		//AES密钥
		static std::string g_AESKey;
	public:
		//加密后的AES密钥
		static std::string g_cipherAESKey;
	public:
		APMCryptogram();
		~APMCryptogram();
	public:
		/*
			功能：MD5
			参数：msg 需要进行MD5的字符串
			返回值：MD5后的字符串
		*/
		static std::string MD5(const std::string& msg);

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
		static std::string RSAEncrypt(const std::string& plain);

		/*
			功能：使用AES(CBC模式)加密
			参数：plainText 待加密的明文字符串
			参数：cipherStr 输出的密文（base64编码）
			返回值：0 成功，-1 加密失败
		*/
		static int AesEncrypt(const std::string& plainText, std::string& cipherStr);

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


