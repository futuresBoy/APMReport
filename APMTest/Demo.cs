using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace APMTestDemo
{
    /// <summary>
    /// 演示C#调用上报SDK的方式和流程
    /// </summary>
    class Demo
    {
        //日志打印回调（SDK调用过程中不能被释放，不然无法接受日志回调）
        static APMDllImport.APMLogFunc logFunc;

        public void Go()
        {
            //0.初始化日志
            logFunc = OnLogNotify;
            APMDllImport.InitLogger(logFunc);
            Console.WriteLine("InitLogger.");

            //1.获取SDK版本
            IntPtr ptr = APMDllImport.GetSDKVersion();
            var versionStr = Marshal.PtrToStringAnsi(ptr);
            Console.WriteLine("1.SDK Version: " + versionStr);

            //2.1 设置RSA密钥
            int rasResult = APMDllImport.SetRSAPubKey("6758ae5bcabf52bf1016a6803b846db5", "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDA4JuF4q8mtCSGcaqTTVkgLc2msyh81zFLrjtEYRrl7O+fQLtI/uV4GAgVSidtpD8vsV8km/Wc/QUB0PiOYl6zRyt7/clVaWd9XH+KwE/eDneZW18QwPOoyIqrnAzQpK2gKBF0EUbo5D/FR2HU6VmoD1Of0U0Q01aZRhn9068YvwIDAQAB");
            Console.WriteLine("SetRSAPubKey: " + rasResult);

            //2.2 设置阈值（包括RSA密钥）
            string ss = "{\"status_code\":0,\"status_msg\":\"success\",\"data\":{\"pub_key_id\":\"6758ae5bcabf52bf1016a6803b846db5\",\"pub_key\":\"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDA4JuF4q8mtCSGcaqTTVkgLc2msyh81zFLrjtEYRrl7O+fQLtI/uV4GAgVSidtpD8vsV8km/Wc/QUB0PiOYl6zRyt7/clVaWd9XH+KwE/eDneZW18QwPOoyIqrnAzQpK2gKBF0EUbo5D/FR2HU6VmoD1Of0U0Q01aZRhn9068YvwIDAQAB\",\"configs\":[{\"module\":\"http\",\"config\":\"{\\\"sampling_rate\\\":\\\"1000\\\",\\\"interval\\\":\\\"10\\\",\\\"aggre_time\\\":\\\"600\\\",\\\"aggre_count\\\":\\\"100\\\"}\"}]}} ";
            int configResult = APMDllImport.SetReportConfig(ss);
            Console.WriteLine("SetReportConfig: " + configResult);

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
            Console.WriteLine("SetClientInfo: " + text);

            //4.性能信息压缩加密
            int len = 10240;
            IntPtr ptrPerfor = Marshal.AllocHGlobal(len);
            int perResult = APMDllImport.BuildPerformanceData(baseInfo.app_id, "aaaadfasfasfafafadfasfafafasdfasdffffffffffffffffffffffffffffasfasdfasdfasdfasdfasdfasdfas", ptrPerfor, ref len);
            string text2 = Marshal.PtrToStringAnsi(ptrPerfor, len);
            Console.WriteLine("BuildPerformanceData: " + text2);
        }

        /// <summary>
        /// SDK内部日志输出
        /// </summary>
        /// <param name="message"></param>
        /// <param name="level"></param>
        public static void OnLogNotify(string message, int level)
        {
            Console.WriteLine($"level:[{level}] message:{message}");
        }

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
