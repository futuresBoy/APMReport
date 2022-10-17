#pragma once

//版本号
#define SDKVERSION "1.0.1.1"

/*采集模块定义*/
//CPU和内存
#define DATA_MODULE_CPU_MEMORY	0x00000001
//启动时间
#define DATA_MODULE_START_UP	0x00000002
//崩溃(程序终止)
#define DATA_MODULE_CRASH		0x00000004
//卡顿
#define DATA_MODULE_CATON		0x00000008
#define DATA_MODULE_WEB			0x00000010
#define DATA_MODULE_HTTP		0x00000020
#define DATA_MODULE_TCP			0x00000040


/*APM_SDK错误码定义*/
#define ERROR_CODE_OK				0	//成功
#define ERROR_CODE_PARAMS			-1	//参数错误
#define ERROR_CODE_OUTOFSIZE		-2	//空间长度不够
#define ERROR_CODE_INNEREXCEPTION	-3	//内部异常

#define ERROR_CODE_DATA				-11	//数据错误
#define ERROR_CODE_DATA_JSON		-12	//数据json格式错误
#define ERROR_CODE_DATA_ENCODE		-13	//数据编码失败
#define ERROR_CODE_DATA_ENCRYPT		-14	//数据加密失败
#define ERROR_CODE_DATA_DECRYPT		-15	//数据解密失败
#define ERROR_CODE_DATA_NULLKEY		-16	//未设置密钥D

#define ERROR_CODE_NULLCLIENTINFO	-21	//未设置客户端基础信息
#define ERROR_CODE_NULLUSERINFO		-22	//未设置用户信息

/*业务类提示*/
#define ERROR_CODE_SWITCHOFF		1	//开关关闭
#define ERROR_CODE_OUTOFCACHE		2	//超出缓存最大条数
#define ERROR_CODE_LOGREPEATED		2	//日志重复