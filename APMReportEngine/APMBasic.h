#pragma once

//�汾��
#define SDKVERSION "1.0.1.1"

/*�ɼ�ģ�鶨��*/
//CPU���ڴ�
#define DATA_MODULE_CPU_MEMORY	0x00000001
//����ʱ��
#define DATA_MODULE_START_UP	0x00000002
//����(������ֹ)
#define DATA_MODULE_CRASH		0x00000004
//����
#define DATA_MODULE_CATON		0x00000008
#define DATA_MODULE_WEB			0x00000010
#define DATA_MODULE_HTTP		0x00000020
#define DATA_MODULE_TCP			0x00000040


/*APM_SDK�����붨��*/
#define ERROR_CODE_OK				0	//�ɹ�
#define ERROR_CODE_PARAMS			-1	//��������
#define ERROR_CODE_OUTOFSIZE		-2	//�ռ䳤�Ȳ���
#define ERROR_CODE_INNEREXCEPTION	-3	//�ڲ��쳣

#define ERROR_CODE_DATA				-11	//���ݴ���
#define ERROR_CODE_DATA_JSON		-12	//����json��ʽ����
#define ERROR_CODE_DATA_ENCODE		-13	//���ݱ���ʧ��
#define ERROR_CODE_DATA_ENCRYPT		-14	//���ݼ���ʧ��
#define ERROR_CODE_DATA_DECRYPT		-15	//���ݽ���ʧ��
#define ERROR_CODE_DATA_NULLKEY		-16	//δ������ԿD

#define ERROR_CODE_NULLCLIENTINFO	-21	//δ���ÿͻ��˻�����Ϣ
#define ERROR_CODE_NULLUSERINFO		-22	//δ�����û���Ϣ

/*ҵ������ʾ*/
#define ERROR_CODE_SWITCHOFF		1	//���عر�
#define ERROR_CODE_OUTOFCACHE		2	//���������������
#define ERROR_CODE_LOGREPEATED		2	//��־�ظ�