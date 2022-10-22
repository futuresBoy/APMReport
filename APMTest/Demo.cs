using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
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

        static APMDllImport.PostErrorLogFunc postErrorLogFunc;
        static APMDllImport.PostPerformanceFunc postPerformanceFunc;

        static HttpClient _httpClient = new HttpClient();

        public async void Go()
        {
            //0.初始化日志
            logFunc = OnLogNotify;
            //APMDllImport.InitLogger(logFunc);
            //Console.WriteLine("InitLogger.");

            //0.初始化SDK
            postErrorLogFunc = OnPostErrorLogNotify;
            postPerformanceFunc = OnPostPerformanceNotify;
            int init = APMDllImport.APMInit(postErrorLogFunc, postPerformanceFunc, logFunc);

            //1.获取SDK版本
            IntPtr ptr = APMDllImport.GetSDKVersion();
            var versionStr = Marshal.PtrToStringAnsi(ptr);
            Console.WriteLine("SDK Version: " + versionStr);

            var baseInfo = new BaseInfo();

            //2.1 设置RSA密钥
            //int rasResult = APMDllImport.SetRSAPubKey("6758ae5bcabf52bf1016a6803b846db5", "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDA4JuF4q8mtCSGcaqTTVkgLc2msyh81zFLrjtEYRrl7O+fQLtI/uV4GAgVSidtpD8vsV8km/Wc/QUB0PiOYl6zRyt7/clVaWd9XH+KwE/eDneZW18QwPOoyIqrnAzQpK2gKBF0EUbo5D/FR2HU6VmoD1Of0U0Q01aZRhn9068YvwIDAQAB");

            //2.2 设置阈值（包括RSA密钥）
            //测试地址
            string configUrl = $"https://khtest.10jqka.com.cn/apm-nginx/apm-api/apm/v1/get_threshold_config?app_id={baseInfo.app_id}&a_ver_app={baseInfo.a_ver_app}&s_ver={versionStr}&d_uuid={baseInfo.d_uuid}";
            string ss = "{\"status_code\":0,\"status_msg\":\"success\",\"data\":{\"pub_key_id\":\"1b891e80c0b4cd56a08e2d394a8e31c8\",\"pub_key\":\"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQD04nj3/4ynPzu28kRh7q83SReo34wKwgaffUm/PfjemtYdcB0LvOwQl74tC2i8pDhvSHMf5mx0USSbr9hQLHp30ubn18oVVyVK2dFsmj1pJylGw2Yw6TnFR/qUfJTA7YteFPVh2ADNc+G9tsA/7SbRiTB3P72zkTB1rnrT+hdILQIDAQAB\",\"configs\":[{\"module\":\"http\",\"config\":\"{\\\"sampling_rate\\\":\\\"500\\\",\\\"type\\\":\\\"2\\\",\\\"aggre_time\\\":\\\"60\\\",\\\"aggre_count\\\":\\\"100\\\",\\\"slow_load_threshold\\\":\\\"1\\\"}\"},{\"module\":\"base\",\"config\":\"{\\\"sampling_rate\\\":\\\"1000\\\",\\\"interval\\\":\\\"10\\\",\\\"aggre_time\\\":\\\"60\\\",\\\"aggre_count\\\":\\\"100\\\"}\"},{\"module\":\"web\",\"config\":\"{\\\"sampling_rate\\\":\\\"1000\\\",\\\"interval\\\":\\\"10\\\",\\\"aggre_time\\\":\\\"60\\\",\\\"aggre_count\\\":\\\"100\\\",\\\"type\\\":\\\"3\\\",\\\"white_screen_threshold\\\":\\\"95\\\",\\\"slow_load_threshold\\\":\\\"3\\\"}\"}]}}";
            int configResult = APMDllImport.SetReportConfig(ss);
            Console.WriteLine("SetReportConfig: " + configResult);

            //2.3 设置开关
            //测试地址：
            string switchUrl = $"https://khtest.10jqka.com.cn/apm-nginx/apm-api/apm/v1/get_switch_config?app_id={baseInfo.app_id}&a_ver_app={baseInfo.a_ver_app}&s_ver={versionStr}&d_uuid={baseInfo.d_uuid}";
            string switchMsg = "{\"status_code\":0,\"status_msg\":\"success\",\"data\":{\"app_id\":\"all\",\"a_ver_app\":\"all\",\"s_ver\":\"all\",\"d_uuid\":\"all\",\"switch\":1,\"gather_switch\":127,\"up_switch\":127}}";
            int switchResult = APMDllImport.SetReportSwitch(switchMsg);
            Console.WriteLine("SetReportSwitch: " + switchResult);

            //3.设置基础数据
            
            var json = JsonConvert.SerializeObject(baseInfo);
            int length = 1024;
            IntPtr intPtr = Marshal.AllocHGlobal(length);
            int result = APMDllImport.SetClientInfo(json, intPtr, ref length);
            Console.WriteLine("SetClientInfo: " + result);
            string text = Marshal.PtrToStringAnsi(intPtr, length);
            Marshal.FreeHGlobal(intPtr);
            using (HttpContent content = new StringContent(text))
            {
                var httpSetBaseInfo = await _httpClient.PostAsync("https://khtest.10jqka.com.cn/apm-nginx/apm-api/apm/v1/base_info", content);
                Console.WriteLine("基础信息完成上报：" + httpSetBaseInfo.IsSuccessStatusCode);
            }

            //4.性能信息压缩加密
            int len = 10240;
            IntPtr ptrPerfor = Marshal.AllocHGlobal(len);
            int perResult = APMDllImport.BuildPerformanceData(baseInfo.app_id, "[{\"label\":{\"app_id\":\"HxPerfMonDemo\",\"a_ver_app\":\"H991.0.0.1\"},\"name\":\"apm_client_cpu_use_rate\",\"type\":1,\"count\":30,\"sum\":18.819576859474182,\"histogram\":{\"bucket\":[2,5,10,20,35,50,75],\"counts\":[29,0,0,1,0,0,0,0]}},{\"label\":{\"app_id\":\"HxPerfMonDemo\",\"a_ver_app\":\"H991.0.0.1\"},\"name\":\"apm_client_mem_use_amount\",\"type\":1,\"count\":30,\"sum\":149.5078125,\"histogram\":{\"bucket\":[75,100,200,400,800,1200],\"counts\":[30,0,0,0,0,0,0]}},{\"label\":{\"app_id\":\"HxPerfMonDemo\",\"a_ver_app\":\"H991.0.0.1\"},\"name\":\"apm_client_gdi_use_amount\",\"type\":1,\"count\":30,\"sum\":932.0,\"histogram\":{\"bucket\":[3,10,40,100,300,700],\"counts\":[0,0,30,0,0,0,0]}},{\"label\":{\"app_id\":\"HxPerfMonDemo\",\"a_ver_app\":\"H991.0.0.1\"},\"name\":\"apm_client_thread_use_amount\",\"type\":1,\"count\":30,\"sum\":150.0,\"histogram\":{\"bucket\":[3,10,20,40,80,130,200],\"counts\":[0,30,0,0,0,0,0,0]}},{\"label\":{\"app_id\":\"HxPerfMonDemo\",\"a_ver_app\":\"H991.0.0.1\"},\"name\":\"apm_client_handle_use_amount\",\"type\":1,\"count\":30,\"sum\":6480.0,\"histogram\":{\"bucket\":[30,80,150,300,600,1000,2000],\"counts\":[0,0,0,30,0,0,0,0]}}]", ptrPerfor, ref len);
            string text2 = Marshal.PtrToStringAnsi(ptrPerfor, len);
            Marshal.FreeHGlobal(ptrPerfor);
            Console.WriteLine("BuildPerformanceData: " + text2);

            //设置用户信息（上传异常日志需要）
            int userInfo = APMDllImport.SetUserInfo("1234567", "xukan", "xukan2");
            Console.WriteLine("SetUserInfo: " + userInfo);

            //异常日志上报
            string errorMsg = "1.0测试异常数据日志上报文本demo";
            //var bytes = Encoding.UTF8.GetBytes(errorMsg);
            //char[] charArray = Encoding.UTF8.GetChars(bytes);
            int addLog = APMDllImport.AddTraceLog(baseInfo.app_id, "ClassDemo", "GoMethod", "-1", 0, false, errorMsg);
            Console.WriteLine("AddTraceLog: " + addLog);

            string errorMsg2 = "2.0测试异常数据日志上报文本demo";
            int addErrorLog = APMDllImport.AddErrorLog(baseInfo.app_id, "ClassDemo", "GoMethod", "-1", errorMsg2);
            Console.WriteLine("AddErrorLog: " + addErrorLog);

            int httpLog = APMDllImport.AddHTTPLog(baseInfo.app_id, "ClassDemo", "http://ftapi.10jqka.com.cn/ljapi/futuresactivity/simulatetradematch/join/?userid=1111111111", "404", 500, "Page Not Fund");
             httpLog = APMDllImport.AddHTTPLog(baseInfo.app_id, "ClassDemo", "https://ftapi.10jqka.com.cn/ljapi/default/appconfig/getall/?app_key=qhtpc", "", 5000, "");
            for (int i = 0; i < 5; i++)
            {
                httpLog = APMDllImport.AddHTTPLog(baseInfo.app_id, "ClassDemo", "http://ftapi.10jqka.com.cn/ljapi/futuresactivity/simulatetradematch/join/?userid=xd9HniY2%2BUuUz%2FMvBG%2ByFA%3D%3D", "", 300, "模拟交易用户参与的大赛列表");
                httpLog = APMDllImport.AddHTTPLog(baseInfo.app_id, "ClassDemo", "https://ftapi.10jqka.com.cn/ljapi/futures/contract/basic", "", 300, "期货品种接口");
            }
            Console.WriteLine("AddHTTPLog: " + httpLog);
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

        public static async void OnPostErrorLogNotify(string message, int msgLength, string url, int urlLength)
        {
            Console.WriteLine($"ErrorMsg:{message}");
            try
            {
                var value = Encoding.UTF8.GetBytes(message);
                using (HttpContent content = new ByteArrayContent(value))
                {
                    //测试地址
                    var result = await _httpClient.PostAsync("https://khtest.10jqka.com.cn/apm-nginx/apm-api/apm/v1/error_log", content);
                    if (result.IsSuccessStatusCode)
                    {
                        Console.WriteLine("异常信息上报成功！");
                    }
                    else
                    {
                        Console.WriteLine($"异常信息上报失败：{result.StatusCode} {result.Content}");
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"异常信息上报异常：{ex.ToString()}");
            }
        }

        public static async void OnPostPerformanceNotify(string message, int msgLength, string url, int urlLength)
        {
            Console.WriteLine($"PerformanceMsg:{message}");
            try
            {
                var value = Encoding.UTF8.GetBytes(message);
                using (HttpContent content = new ByteArrayContent(value))
                {
                    //测试地址
                    var result = await _httpClient.PostAsync("https://khtest.10jqka.com.cn/apm-nginx/apm-api/apm/v1/perf_metric", content);
                    if (result.IsSuccessStatusCode)
                    {
                        Console.WriteLine("性能信息上报成功！");
                    }
                    else
                    {
                        Console.WriteLine($"性能信息上报失败：{result.StatusCode} {result.Content}");
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"异常信息上报异常：{ex.ToString()}");
            }
        }
    }

    /// <summary>
    /// 服务端定义的设备基础信息结构
    /// </summary>
    class BaseInfo
    {
        public string app_id { get; set; } = "hevo-client-future";

        public string d_uuid { get; set; } = "FA-80-02-0A-0C-A8";

        public string a_bundle_id { get; set; } = "happ.exe";

        public string a_ver_app { get; set; } = "1.0.0";

        public string d_os { get; set; } = "windows 7";

        public string d_model { get; set; } = "";
    }
}
