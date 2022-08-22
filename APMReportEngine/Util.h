namespace APMReport
{
	/*
		功能：设置RSA公钥
		参数：pubKeyID 公钥编号
		参数：pubKey 公钥字符串
		返回值：0 成功，-1 参数不正确
	*/
	int SetRSAKey(const char* keyID, const char* key);


	/*
		功能：使用AES(CBC模式)加密
		参数：plainText 待加密的明文字符串
		参数：cipherText 输出的密文（base64编码）
		参数：outLen 输出的密文长度
		返回值：0 成功，-1 加密失败
	*/
	int AesEncrypt(const char* plainText, char* cipherText, int& outLen);

	/*
		功能：使用AES(CBC模式)加密
		参数：plainText 待加密的明文字符串
		参数：cipherStr 输出的密文（base64编码）
		返回值：0 成功，-1 加密失败
	*/
	int AesEncrypt(const char* plainText, std::string cipherStr);
}

