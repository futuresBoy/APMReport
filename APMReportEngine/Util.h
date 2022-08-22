namespace APMReport
{
	/*
		���ܣ�����RSA��Կ
		������pubKeyID ��Կ���
		������pubKey ��Կ�ַ���
		����ֵ��0 �ɹ���-1 ��������ȷ
	*/
	int SetRSAKey(const char* keyID, const char* key);


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
	int AesEncrypt(const char* plainText, std::string cipherStr);
}

