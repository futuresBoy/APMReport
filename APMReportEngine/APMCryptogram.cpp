#include "APMBasic.h"
#include "APMCryptogram.h"


namespace APMReport
{
	std::string APMCryptogram::g_AESKey;
	std::string APMCryptogram::g_cipherAESKey;
	static bool g_bInited = false;

	//（服务端）默认密钥编号
	static std::string g_keyID = "6758ae5bcabf52bf1016a6803b846db5";
	//（服务端）默认RSA公钥，预防服务端密钥获取失败
	static std::string g_RSAPubkey = "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDA4JuF4q8mtCSGcaqTTVkgLc2msyh81zFLrjtEYRrl7O+fQLtI/uV4GAgVSidtpD8vsV8km/Wc/QUB0PiOYl6zRyt7/clVaWd9XH+KwE/eDneZW18QwPOoyIqrnAzQpK2gKBF0EUbo5D/FR2HU6VmoD1Of0U0Q01aZRhn9068YvwIDAQAB";

	APMCryptogram::APMCryptogram()
	{

	}

	APMCryptogram::~APMCryptogram()
	{
	}

	std::string APMCryptogram::MD5(const std::string& msg)
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


	int APMCryptogram::GetRSAPubKey(std::string& keyID, std::string& pubKey)
	{
		if (g_keyID.empty() || g_RSAPubkey.empty())
		{
			return ERROR_CODE_PARAMS;
		}
		keyID = g_keyID;
		pubKey = g_RSAPubkey;
		return 0;
	}

	int APMCryptogram::SetRSAPubKey(const char* keyID, const char* rsaPubkey)
	{
		if (CHECK_ISNULLOREMPTY(keyID) || CHECK_ISNULLOREMPTY(rsaPubkey))
		{
			LOGERROR("rsa public key is empty!");
			return ERROR_CODE_PARAMS;
		}
		//公钥只需初始化一次
		if (g_bInited)
		{
			return true;
		}

		std::string pubKeyID(keyID);
		g_keyID = pubKeyID;
		std::string pubKey(rsaPubkey);
		g_RSAPubkey = pubKey;

		//使用RSA公钥加密AES密钥
		std::string aesKey = GetAESKey();
		auto cipherText = RSAEncrypt(aesKey);
		if (cipherText.empty())
		{
			return ERROR_CODE_DATA_ENCRYPT;
		}
		g_cipherAESKey = cipherText;
		g_bInited = true;
		return 0;
	}

	std::string APMCryptogram::RSAEncrypt(const std::string& plain)
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

	std::string APMCryptogram::GetAESKey()
	{
		if (!g_AESKey.empty())
		{
			return g_AESKey;
		}
		//生成16位的随机密钥
		g_AESKey = GenerateRandStr(16, true);
		return g_AESKey;
	}

	std::string APMCryptogram::GenerateRandStr(int sz, bool printable)
	{
		std::string ret;
		ret.resize(sz);
		std::mt19937 rng(std::random_device{}());
		for (int i = 0; i < sz; ++i)
		{
			if (printable)
			{
				uint32_t x = rng() % (sizeof(CCH) - 1);
				ret[i] = CCH[x];
			}
			else
			{
				ret[i] = rng() % 0xFF;
			}
		}
		return ret;
	}

	int APMCryptogram::AesEncrypt(std::string plainText, std::string& cipherBase64)
	{
		std::string cipher;
		try
		{
			auto key = GetAESKey();
			CBC_Mode<AES>::Encryption en;
			en.SetKeyWithIV((byte*)key.c_str(), key.length(), (byte*)key.c_str());

			StringSource s(plainText, true,
				new StreamTransformationFilter(en,
					new StringSink(cipher)
				) // StreamTransformationFilter
			); // StringSource

			StringSource(cipher, true, new Base64Encoder(new StringSink(cipherBase64), false));
		}
		catch (const std::exception & ex)
		{
			LOGFATAL(ex.what());
			return ERROR_CODE_INNEREXCEPTION;
		}
		return 0;
	}


	std::string APMCryptogram::GzipCompress(std::string& data)
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

}
