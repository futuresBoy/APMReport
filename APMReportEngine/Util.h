namespace APMReport
{
	/*
		���ܣ���ȡ��ǰʱ���ַ���
		���أ���׼��ʽʱ��
	*/
	std::string GetTimeNowStr();

	/*
		���ܣ�����RSA��Կ
		������pubKeyID ��Կ���
		������pubKey ��Կ�ַ���
		����ֵ��0 �ɹ���-1 ��������ȷ
	*/
	int SetRSAPubKey(const char* pubKeyID, const char* pubKey);

	/*
		���ܣ���ȡ��Կ
		������keyID ��Կ���
		������pubKey ��Կ�ַ�
		����ֵ��0 �ɹ���-1 ��ȡʧ�ܣ�û�й�Կ����
	*/
	int GetRSAPubKey(std::string& keyID, std::string& pubKey);

	/*
		���ܣ���ȡ��ǰAES��Կ
		���أ����ܺ����Կ
	*/
	std::string GetAESKey();

	/*
		���ܣ�RSA(��Կ)����
		������plain �����ܵ�����
		����ֵ�����ܺ������
	*/
	std::string RSAEncrypt(const std::string& plain);

	/*
		���ܣ�RSA����Կ������AES��Կ
		������pubKey RSA�Ĺ�Կ
		����ֵ��0 ���ܳɹ���-1 ����ʧ��
	*/
	int RSAEncryptAESKey(const char* pubKey);

	/*
		���ܣ�ʹ��AES(CBCģʽ)����
		������plainText �����ܵ������ַ���
		������cipherText ��������ģ�base64���룩
		������outLen ��������ĳ���
		����ֵ��0 �ɹ���-1 ����ʧ��
	*/
	int AesEncrypt(const char* plainText, char* cipherText, int& outLen);

	/*
		���ܣ�ʹ��AES(CBCģʽ)����
		������plainText �����ܵ������ַ���
		������cipherStr ��������ģ�base64���룩
		����ֵ��0 �ɹ���-1 ����ʧ��
	*/
	int AesEncrypt(const char* plainText, std::string& cipherStr);

	/*
		���ܣ�MD5
		������msg ��Ҫ����MD5���ַ���
		����ֵ��MD5����ַ���
	*/
	std::string MD5(std::string msg);

	/*
		���ܣ�Gzipѹ��
		������data ��Ҫѹ��������
		����ֵ��ѹ���������
	*/
	std::string GzipCompress(std::string& data);
}

