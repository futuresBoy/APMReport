namespace APMReport
{
	/*
		功能：获取当前时间字符串
		返回：标准格式时间
	*/
	std::string GetTimeNowStr();

	/*
		功能：设置RSA公钥
		参数：pubKeyID 公钥编号
		参数：pubKey 公钥字符串
		返回值：0 成功，-1 参数不正确
	*/
	int SetRSAPubKey(const char* pubKeyID, const char* pubKey);

	/*
		功能：获取公钥
		参数：keyID 公钥编号
		参数：pubKey 公钥字符
		返回值：0 成功，-1 获取失败，没有公钥生成
	*/
	int GetRSAPubKey(std::string& keyID, std::string& pubKey);

	/*
		功能：获取当前AES密钥
		返回：加密后的密钥
	*/
	std::string GetAESKey();

	/*
		功能：RSA(公钥)加密
		参数：plain 待加密的明文
		返回值：加密后的密文
	*/
	std::string RSAEncrypt(const std::string& plain);

	/*
		功能：RSA（公钥）加密AES密钥
		参数：pubKey RSA的公钥
		返回值：0 加密成功，-1 加密失败
	*/
	int RSAEncryptAESKey(const char* pubKey);

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
	int AesEncrypt(const char* plainText, std::string& cipherStr);

	/*
		功能：MD5
		参数：msg 需要进行MD5的字符串
		返回值：MD5后的字符串
	*/
	std::string MD5(std::string msg);

	/*
		功能：Gzip压缩
		参数：data 需要压缩的数据
		返回值：压缩后的数据
	*/
	std::string GzipCompress(std::string& data);
}

