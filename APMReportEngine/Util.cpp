//工具类，用于APM相关的数据格式解析、压缩、加密操作

#include <base64.h>
#include <aes.h>
#include <rsa.h>
#include <modes.h>
#include <randpool.h>
#include <osrng.h>
#include <files.h>
#include <hex.h>
#include <gzip.h>
#include "Util.h"
#include "Logger.h"

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <md5.h>

using namespace CryptoPP;
#pragma comment(lib, "cryptlib.lib")

namespace APMReport
{
	//（服务端）密钥编号
	std::string g_keyID = "6758ae5bcabf52bf1016a6803b846db5";
	//（服务端）RSA公钥
	std::string g_RSAPubkey = "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDA4JuF4q8mtCSGcaqTTVkgLc2msyh81zFLrjtEYRrl7O+fQLtI/uV4GAgVSidtpD8vsV8km/Wc/QUB0PiOYl6zRyt7/clVaWd9XH+KwE/eDneZW18QwPOoyIqrnAzQpK2gKBF0EUbo5D/FR2HU6VmoD1Of0U0Q01aZRhn9068YvwIDAQAB";

	//AES密钥
	SecByteBlock g_AESKey(AES::MIN_KEYLENGTH);
	//加密后的AES密钥
	std::string g_cipherAESKey;

	std::string GetTimeNowStr()
	{
		time_t t = time(0);
		char tmp[32] = { NULL };
		strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));
		return std::string(tmp);
	}

	int SetRSAPubKey(const char* keyID, const char* RSAPubkey)
	{
		if (keyID == nullptr || strlen(keyID))
		{
			return -1;
		}
		if (RSAPubkey == nullptr || strlen(RSAPubkey))
		{
			return -1;
		}
		std::string pubKeyID(keyID);
		g_keyID = pubKeyID;
		std::string pubKey(RSAPubkey);
		g_RSAPubkey = pubKey;
		return RSAEncryptAESKey(RSAPubkey);
	}

	int RSAEncryptAESKey(const char* pubKey)
	{
		//随机生成一个AES密钥
		if (g_AESKey.data() == nullptr)
		{
			AutoSeededRandomPool rnd;
			rnd.GenerateBlock(g_AESKey, g_AESKey.size());
		}
		std::string key(reinterpret_cast<const char*>(&g_AESKey[0]), g_AESKey.size());
		auto cipherText = RSAEncrypt(key);
		if (cipherText.length() == 0)
		{
			return -1;
		}
		g_cipherAESKey = cipherText;
		return 0;
	}

	std::string RSAEncrypt(const std::string& plain)
	{
		// Generate keys
		AutoSeededRandomPool rng;
		/*InvertibleRSAFunction params;
		params.GenerateRandomWithKeySize(rng, 3072);
		RSA::PublicKey publicKey(params);
		RSAES_OAEP_SHA_Encryptor en(publicKey);*/

		StringSource source(g_RSAPubkey, true, new Base64Encoder);
		RSAES_OAEP_SHA_Encryptor encrypt(source);

		std::string cipher;
		StringSource ss1(plain, true, new PK_EncryptorFilter(rng, encrypt, new StringSink(cipher)));
		return cipher;
	}

	int GetRSAPubKey(std::string& keyID, std::string& pubKey)
	{
		if (g_keyID.empty() || g_RSAPubkey.empty())
		{
			return -1;
		}
		keyID = g_keyID;
		pubKey = g_RSAPubkey;
		return 0;
	}

	std::string GetAESKey()
	{
		return g_cipherAESKey;
	}

	void RSAEncrypt2(const std::string& plain, const char* encryptedFilename)
	{
		StringSource source(g_RSAPubkey, true, new Base64Encoder);
		RSAES_OAEP_SHA_Encryptor pubkey(source);

		SecByteBlock sbbCipherText(pubkey.CiphertextLength(plain.size()));
		AutoSeededRandomPool rng;
		pubkey.Encrypt(
			rng,
			(byte const*)plain.data(),
			plain.size(),
			sbbCipherText.begin());

		FileSink(encryptedFilename).Put(sbbCipherText.begin(), sbbCipherText.size());
	}

	int AesEncrypt(const char* plainText, char* cipherText, int& outLen)
	{
		std::string cipherStr;
		int result = AesEncrypt(plainText, cipherStr);
		if (result != 0)
		{
			return -1;
		}
		outLen = cipherStr.length();
		memcpy(cipherText, cipherStr.c_str(), outLen);
	}

	int AesEncrypt(const char* plainText, std::string& cipherBase64)
	{
		std::string cipher;
		try
		{
			//随机生成一个AES密钥
			if (g_AESKey.data() == nullptr)
			{
				AutoSeededRandomPool rnd;
				rnd.GenerateBlock(g_AESKey, g_AESKey.size());
			}
			//使用key填iv
			SecByteBlock iv(AES::MIN_KEYLENGTH);
			memcpy(iv, g_AESKey, AES::MIN_KEYLENGTH);

			CBC_Mode<AES>::Encryption en;
			en.SetKeyWithIV(g_AESKey, g_AESKey.size(), iv);

			StringSource s(plainText, true,
				new StreamTransformationFilter(en,
					new StringSink(cipher)
				) // StreamTransformationFilter
			); // StringSource

			StringSource(cipher, true, new Base64Encoder(new StringSink(cipherBase64)));
			return 0;
		}
		catch (const std::exception & ex)
		{
			LOGFATAL(ex.what());
			return -1;
		}
	}

	std::string MD5(std::string msg)
	{
		if (msg.empty())
		{
			return msg;
		}
		Weak::MD5 hash;
		std::string digest;
		try
		{
			StringSource(msg, true, new HashFilter(hash, new StringSink(digest)));
		}
		catch (const std::exception & ex)
		{
			LOGFATAL(ex.what());
		}
		return digest;
	}

	std::string GzipCompress(std::string& data)
	{
		if (data.empty())
		{
			return data;
		}
		std::string compressed;

		Gzip zipper(new StringSink(compressed));
		zipper.Put((byte*)data.data(), data.size());
		zipper.MessageEnd();
	}

	std::string GzipCompress2(std::string& data)
	{
		if (data.empty())
		{
			return data;
		}
		std::string compressed;

		StringSource ss(data, true, new Gzip(new StringSink(compressed)));
	}
}

