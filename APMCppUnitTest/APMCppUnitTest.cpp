#include "pch.h"
#include "CppUnitTest.h"
#include"..\APMReportEngine\APMReport.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace APMCppUnitTest
{
	TEST_CLASS(APMCppUnitTest)
	{
	public:

		TEST_METHOD(TestMethod1)
		{

		}

		TEST_METHOD(TestSDKVersion)
		{
			const char* ver = "1.0.0.2";
			auto version = GetSDKVersion();
			Assert::AreEqual(ver, version);
		}

		TEST_METHOD(TestSetRSAKey2)
		{
			int result = SetRSAPubKey(nullptr, nullptr);
			Assert::AreEqual(-1, result);
		}

		TEST_METHOD(TestSetRSAKey3)
		{
			int result = SetRSAPubKey("", "");
			Assert::AreEqual(-1, result);
		}

		TEST_METHOD(TestSetRSAKey4)
		{
			int result = SetRSAPubKey("1111", "2222");
			Assert::AreEqual(-1, result);
		}

		TEST_METHOD(TestSetConfig1)
		{
			int result = SetReportConfig(nullptr);
			Assert::AreEqual(-1, result);
		}

		TEST_METHOD(TestSetConfig2)
		{
			int result = SetReportConfig("");
			Assert::AreEqual(-1, result);
		}

		TEST_METHOD(TestSetConfig3)
		{
			int result = SetReportConfig("ssssss");
			Assert::AreEqual(-1, result);
		}

		TEST_METHOD(TestSetConfig4)
		{
			int result = SetReportConfig("{\"status_code\":-1,\"status_msg\":\"bad\"");
			Assert::AreEqual(-1, result);
		}

		TEST_METHOD(TestSetConfig5)
		{
			int result = SetReportConfig("{\"status_code\":0,\"status_msg\":\"success\",\"data\":{\"pub_key_id\":\"6758ae5bcabf52bf1016a6803b846db5\",\"pub_key\":\"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDA4JuF4q8mtCSGcaqTTVkgLc2msyh81zFLrjtEYRrl7O+fQLtI/uV4GAgVSidtpD8vsV8km/Wc/QUB0PiOYl6zRyt7/clVaWd9XH+KwE/eDneZW18QwPOoyIqrnAzQpK2gKBF0EUbo5D/FR2HU6VmoD1Of0U0Q01aZRhn9068YvwIDAQAB\",\"configs\":[{\"module\":\"http\",\"config\":\"{\\\"sampling_rate\\\":\\\"1000\\\",\\\"interval\\\":\\\"10\\\",\\\"aggre_time\\\":\\\"600\\\",\\\"aggre_count\\\":\\\"100\\\"}\"}]}} ");
			Assert::AreEqual(0, result);
		}

		TEST_METHOD(TestSetClientInfo1)
		{
			int length = 0;
			int result = SetClientInfo(nullptr, nullptr, length);
			Assert::AreEqual(-1, result);
		}

		TEST_METHOD(TestSetClientInfo2)
		{
			int length = 0;
			int result = SetClientInfo("", "", length);
			Assert::AreEqual(-1, result);
		}

		TEST_METHOD(TestSetClientInfo3)
		{
			int length = 1;
			char* data = new char[length];
			int result = SetClientInfo("hhh", data, length);
			delete data;
			Assert::AreEqual(-1, result);
		}

		TEST_METHOD(TestSetClientInfo4)
		{
			int length = 1;
			char* data = new char[length];
			int result = SetClientInfo("{\"app_id\":\"\",\"d_uuid\":\"\",\"a_bundle_id\":\"happ.exe\",\"a_ver_app\":\"1.0.0\",\"d_os\":\"windows\",\"d_model\":\"\"}", data, length);
			delete data;
			Assert::AreEqual(-1, result);
		}

		TEST_METHOD(TestSetClientInfo5)
		{
			int length = 1;
			char* data = new char[length];
			int result = SetClientInfo("{\"app_id\":\"TestClient\",\"d_uuid\":\"\",\"a_bundle_id\":\"happ.exe\",\"a_ver_app\":\"1.0.0\",\"d_os\":\"windows\",\"d_model\":\"\"}", data, length);
			delete data;
			Assert::AreEqual(-1, result);
		}

		TEST_METHOD(TestSetClientInfo6)
		{
			int length = 1024;
			char* data = new char[length];
			int result = SetClientInfo("{\"app_id\":\"TestClient\",\"d_uuid\":\"0000000\",\"a_bundle_id\":\"happ.exe\",\"a_ver_app\":\"1.0.0\",\"d_os\":\"windows\",\"d_model\":\"\"}", data, length);
			delete data;
			Assert::AreEqual(0, result);
		}

		TEST_METHOD(TestPerformance1)
		{
			int length = 0;
			int result = BuildPerformanceData(nullptr, nullptr, nullptr,length);
			Assert::AreEqual(-1, result);
		}

		TEST_METHOD(TestPerformance2)
		{
			int length = 0;
			int result = BuildPerformanceData("", "", nullptr, length);
			Assert::AreEqual(-1, result);
		}

		TEST_METHOD(TestPerformance3)
		{
			int length = 1;
			char* data = new char[length];
			int result = BuildPerformanceData("TestClient", "", data, length);
			delete data;
			Assert::AreEqual(-1, result);
		}

		TEST_METHOD(TestPerformance4)
		{
			int length = 1;
			char* data = new char[length];
			int result = BuildPerformanceData("TestClient", "some data", data, length);
			delete data;
			Assert::AreEqual(-1, result);
		}

		TEST_METHOD(TestPerformance5)
		{
			int length = 1024;
			char* data = new char[length];
			int result = BuildPerformanceData("TestClient", "some data", data, length);
			delete data;
			Assert::AreEqual(-1, result);
		}

		TEST_METHOD(TestPerformance6)
		{
			TestSetClientInfo6();

			int length = 1024;
			char* data = new char[length];
			int result = BuildPerformanceData("TestClient", "some data", data, length);
			delete data;
			Assert::AreEqual(0, result);
		}

		TEST_METHOD(TestPerformance7)
		{
			TestSetClientInfo6();

			int length = 1024;
			char* data = new char[length];
			int result = BuildPerformanceData("TestClient", "[{\"label\":{\"app_id\":\"HxPerfMonDemo\",\"a_ver_app\":\"H991.0.0.1\"},\"name\":\"apm_client_cpu_use_rate\",\"type\":1,\"count\":30,\"sum\":18.819576859474182,\"histogram\":{\"bucket\":[2,5,10,20,35,50,75],\"counts\":[29,0,0,1,0,0,0,0]}},{\"label\":{\"app_id\":\"HxPerfMonDemo\",\"a_ver_app\":\"H991.0.0.1\"},\"name\":\"apm_client_mem_use_amount\",\"type\":1,\"count\":30,\"sum\":149.5078125,\"histogram\":{\"bucket\":[75,100,200,400,800,1200],\"counts\":[30,0,0,0,0,0,0]}},{\"label\":{\"app_id\":\"HxPerfMonDemo\",\"a_ver_app\":\"H991.0.0.1\"},\"name\":\"apm_client_gdi_use_amount\",\"type\":1,\"count\":30,\"sum\":932.0,\"histogram\":{\"bucket\":[3,10,40,100,300,700],\"counts\":[0,0,30,0,0,0,0]}},{\"label\":{\"app_id\":\"HxPerfMonDemo\",\"a_ver_app\":\"H991.0.0.1\"},\"name\":\"apm_client_thread_use_amount\",\"type\":1,\"count\":30,\"sum\":150.0,\"histogram\":{\"bucket\":[3,10,20,40,80,130,200],\"counts\":[0,30,0,0,0,0,0,0]}},{\"label\":{\"app_id\":\"HxPerfMonDemo\",\"a_ver_app\":\"H991.0.0.1\"},\"name\":\"apm_client_handle_use_amount\",\"type\":1,\"count\":30,\"sum\":6480.0,\"histogram\":{\"bucket\":[30,80,150,300,600,1000,2000],\"counts\":[0,0,0,30,0,0,0,0]}}]", data, length);
			delete data;
			Assert::AreEqual(0, result);
		}
	};
}
