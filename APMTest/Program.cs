using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using Newtonsoft.Json;

namespace APMTest
{
    class Program
    {
        static void Main(string[] args)
        {
            TestGo();
        }

        private static void TestGo()
        {
            //1.获取SDK版本
            IntPtr ptr = APMDllImport.GetSDKVersion();
            var versionStr = Marshal.PtrToStringAnsi(ptr);

            //2.1 设置RSA密钥
            int rasResult = APMDllImport.SetRSAPubKey("6758ae5bcabf52bf1016a6803b846db5", "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDA4JuF4q8mtCSGcaqTTVkgLc2msyh81zFLrjtEYRrl7O+fQLtI/uV4GAgVSidtpD8vsV8km/Wc/QUB0PiOYl6zRyt7/clVaWd9XH+KwE/eDneZW18QwPOoyIqrnAzQpK2gKBF0EUbo5D/FR2HU6VmoD1Of0U0Q01aZRhn9068YvwIDAQAB");

            //2.2 设置阈值（包括RSA密钥）
            //string ss = "{\"status_code\":0,\"status_msg\":\"success\",\"data\":{\"pub_key_id\":\"6758ae5bcabf52bf1016a6803b846db5\",\"pub_key\":\"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDA4JuF4q8mtCSGcaqTTVkgLc2msyh81zFLrjtEYRrl7O+fQLtI/uV4GAgVSidtpD8vsV8km/Wc/QUB0PiOYl6zRyt7/clVaWd9XH+KwE/eDneZW18QwPOoyIqrnAzQpK2gKBF0EUbo5D/FR2HU6VmoD1Of0U0Q01aZRhn9068YvwIDAQAB\",\"configs\":[{\"module\":\"http\",\"config\":\"{\\\"sampling_rate\\\":\\\"1000\\\",\\\"interval\\\":\\\"10\\\",\\\"aggre_time\\\":\\\"600\\\",\\\"aggre_count\\\":\\\"100\\\"}\"}]}} ";
            //int configResult = APMDllImport.SetReportConfig(ss);

            //3.设置基础数据
            var baseInfo = new BaseInfo();
            var json = JsonConvert.SerializeObject(baseInfo);
            int length = 1024;
            //方式1：
            //StringBuilder sb = new StringBuilder(length);
            //int result = APMDllImport.SetClientInfo("PCTest", "00000000", json, sb, ref length);
            //string text = sb.ToString().Substring(0,length);
            //方式2：
            IntPtr intPtr = Marshal.AllocHGlobal(length);
            int result = APMDllImport.SetClientInfo(json, intPtr, ref length);
            string text = Marshal.PtrToStringAnsi(intPtr, length);
            Marshal.FreeHGlobal(intPtr);

            //4.性能信息压缩加密
            int len = 10240;
            IntPtr ptrPerfor = Marshal.AllocHGlobal(len);
            int perResult = APMDllImport.BuildPerformanceData("aaaadfasfasfafafadfasfafafasdfasdffffffffffffffffffffffffffffasfasdfasdfasdfasdfasdfasdfas", ptrPerfor, ref len);
            string text2 = Marshal.PtrToStringAnsi(ptrPerfor, len);
            Console.ReadLine();
        }

        /// <summary>
        /// 服务端定义的设备基础信息结构
        /// </summary>
        class BaseInfo
        {
            public string app_id { get; set; } = "TestApp";

            public string d_uuid { get; set; } = "00000000";

            public string a_bundle_id { get; set; } = "happ.exe";

            public string a_ver_app { get; set; } = "1.0.0";

            public string d_os { get; set; } = "windows";

            public string d_model { get; set; } = "";
        }
    }
}
