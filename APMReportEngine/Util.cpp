//�����࣬����APM��ص����ݸ�ʽ������ѹ�������ܲ���

#include <base64.h>
#include <aes.h>
#include <rsa.h>
#include <modes.h>
#include <randpool.h>
#include <osrng.h>
#include <files.h>
#include <hex.h>
#include "Util.h"
#include "Logger.h"

using namespace CryptoPP;
#pragma comment(lib, "cryptlib.lib")

namespace APMReport
{
	//������ˣ���Կ���
	const char* g_keyID;
	//������ˣ�RSA��Կ
	const char* g_RSAPubkey= "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDA4JuF4q8mtCSGcaqTTVkgLc2msyh81zFLrjtEYRrl7O+fQLtI/uV4GAgVSidtpD8vsV8km/Wc/QUB0PiOYl6zRyt7/clVaWd9XH+KwE/eDneZW18QwPOoyIqrnAzQpK2gKBF0EUbo5D/FR2HU6VmoD1Of0U0Q01aZRhn9068YvwIDAQAB";

	SecByteBlock g_AESKey(AES::MIN_KEYLENGTH);

	int SetRSAKey(const char* keyID, const char* RSAkey)
	{
		if (keyID == nullptr || strlen(keyID))
		{
			return -1;
		}
		if (RSAkey == nullptr || strlen(RSAkey))
		{
			return -1;
		}
		g_keyID = g_keyID;
		g_RSAPubkey = RSAkey;
	}

	//ʹ�ù�Կ����AES��Կ
	int EncryptAESKey()
	{
		//�������һ��AES��Կ
		if (g_AESKey.data() == nullptr)
		{
			AutoSeededRandomPool rnd;
			rnd.GenerateBlock(g_AESKey, g_AESKey.size());
		}
		std::string key(reinterpret_cast<const char*>(&g_AESKey[0]), g_AESKey.size());
		auto encryptKey = RSAEncrypt(key);
	}

	//RSA��Կ����
	std::string RSAEncrypt(const std::string& plain)
	{
		// Generate keys
		AutoSeededRandomPool rng;
		/*InvertibleRSAFunction params;
		params.GenerateRandomWithKeySize(rng, 3072);
		RSA::PublicKey publicKey(params);
		RSAES_OAEP_SHA_Encryptor en(publicKey);*/

		auto source = StringSource(g_RSAPubkey, true, new Base64Encoder);
		RSAES_OAEP_SHA_Encryptor encrypt(source);
		
		std::string cipher;
		StringSource ss1(plain, true, new PK_EncryptorFilter(rng, encrypt, new StringSink(cipher)));
		return cipher;
	}

	void RSAEncrypt2(const std::string& plain, const char* encryptedFilename)
	{
		auto source = StringSource(g_RSAPubkey, true, new Base64Encoder);
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

	//AES����
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

	//AES����
	int AesEncrypt(const char* plainText, std::string cipherBase64)
	{
		std::string cipher;
		try
		{
			//�������һ��AES��Կ
			if (g_AESKey.data() == nullptr)
			{
				AutoSeededRandomPool rnd;
				rnd.GenerateBlock(g_AESKey, g_AESKey.size());
			}
			//ʹ��key��iv
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
}

