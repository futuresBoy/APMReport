
#include "Util.h"

namespace APMReport
{
	//（服务端）密钥编号
	std::string Util::g_keyID = "6758ae5bcabf52bf1016a6803b846db5";

	//（服务端）RSA公钥
	std::string Util::g_RSAPubkey = "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDA4JuF4q8mtCSGcaqTTVkgLc2msyh81zFLrjtEYRrl7O+fQLtI/uV4GAgVSidtpD8vsV8km/Wc/QUB0PiOYl6zRyt7/clVaWd9XH+KwE/eDneZW18QwPOoyIqrnAzQpK2gKBF0EUbo5D/FR2HU6VmoD1Of0U0Q01aZRhn9068YvwIDAQAB";

	//AES密钥
	SecByteBlock Util::g_AESKey;

	//加密后的AES密钥
	std::string Util::g_cipherAESKey;

	std::string Util::GetTimeNowStr()
	{
		time_t t = time(0);
		char tmp[32] = { NULL };
		strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));
		return std::string(tmp);
	}

	int Util::SetRSAPubKey(const char* keyID, const char* RSAPubkey)
	{
		if (keyID == nullptr || RSAPubkey == nullptr)
		{
			return -1;
		}

		std::string pubKeyID(keyID);
		g_keyID = pubKeyID;
		std::string pubKey(RSAPubkey);
		g_RSAPubkey = pubKey;
		return RSAEncryptAESKey(RSAPubkey);
	}

	int Util::RSAEncryptAESKey(const char* pubKey)
	{
		auto aesKey = GetAESKey();
		std::string key(reinterpret_cast<const char*>(&aesKey[0]), aesKey.size());
		auto cipherText = RSAEncrypt(key);
		if (cipherText.length() == 0)
		{
			return -1;
		}
		g_cipherAESKey = cipherText;
		return 0;
	}

	std::string Util::RSAEncrypt(std::string plain)
	{
		AutoSeededRandomPool randPool;
		std::string cipher;
		try
		{
			StringSource source(g_RSAPubkey, true, new Base64Decoder);
			//PKCS标准
			RSAES_PKCS1v15_Encryptor encrypt(source);
			StringSource(plain, true, new PK_EncryptorFilter(randPool, encrypt, new Base64Encoder(new StringSink(cipher), false)));
		}
		catch (const std::exception & ex)
		{
			LOGFATAL(ex.what());
		}
		return cipher;
	}

	int Util::GetRSAPubKey(std::string& keyID, std::string& pubKey)
	{
		if (g_keyID.empty() || g_RSAPubkey.empty())
		{
			return -1;
		}
		keyID = g_keyID;
		pubKey = g_RSAPubkey;
		return 0;
	}

	SecByteBlock Util::GetAESKey()
	{
		if (!g_AESKey.empty())
		{
			return g_AESKey;
		}
		//随机生成一个AES密钥
		SecByteBlock g_AESKey(AES::MIN_KEYLENGTH);
		AutoSeededRandomPool rnd;
		rnd.GenerateBlock(g_AESKey, g_AESKey.size());
		return g_AESKey;
	}

	int Util::AesEncrypt(const char* plainText, char* cipherText, int& outLen)
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

	int Util::AesEncrypt(std::string plainText, std::string& cipherBase64)
	{
		std::string cipher;
		try
		{
			auto key = GetAESKey();
			//使用key填iv
			SecByteBlock iv(AES::MIN_KEYLENGTH);
			memcpy(iv, key, AES::MIN_KEYLENGTH);

			CBC_Mode<AES>::Encryption en;
			en.SetKeyWithIV(key, key.size(), iv);

			StringSource s(plainText, true,
				new StreamTransformationFilter(en,
					new StringSink(cipher)
				) // StreamTransformationFilter
			); // StringSource

			StringSource(cipher, true, new Base64Encoder(new StringSink(cipherBase64), false));
			return 0;
		}
		catch (const std::exception & ex)
		{
			LOGFATAL(ex.what());
			return -1;
		}
	}

	std::string Util::MD5(std::string msg)
	{
		if (msg.empty())
		{
			return msg;
		}
		Weak::MD5 hash;
		std::string digest;
		try
		{
			StringSource(msg, true, new HashFilter(hash, new Base64Encoder(new StringSink(digest), false)));
		}
		catch (const std::exception & ex)
		{
			LOGFATAL(ex.what());
		}
		return digest;
	}

	std::string Util::GzipCompress(std::string& data)
	{
		if (data.empty())
		{
			return data;
		}
		std::string compressed;
		try
		{
			Gzip zipper(new StringSink(compressed));
			zipper.Put((byte*)data.data(), data.size());
			zipper.MessageEnd();
		}
		catch (const std::exception & ex)
		{
			LOGFATAL(ex.what());
		}
		return compressed;
	}

	/*std::string GzipCompress2(std::string& data)
	{
		if (data.empty())
		{
			return data;
		}
		std::string compressed;
		try
		{
			StringSource ss(data, true, new Gzip(new StringSink(compressed)));
		}
		catch (const std::exception & ex)
		{
			LOGFATAL(ex.what());
		}
		return compressed;
	}*/
}

