using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace APMTestDemo
{
    class HxPerformanceImport
    {
        const string AppID = "hevo-client-future";

        const string dllPerfMonName = "HxPerfMonApi.dll";

        /// <summary>
        /// 启动监控进程
        /// </summary>
        /// <param name="monitorPath">监控进程路径</param>
        /// <param name="productName">产品名称（公司业务树上登记的名称）</param>
        /// <param name="productBuild">产品编译名（C#客户端可不填）</param>
        /// <param name="productVersion">产品版本号</param>
        /// <returns>返回值大于0为监控进程ID，小于0报错</returns>
        [DllImport(dllPerfMonName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern int APMPerfStartMonitorCS(string monitorPath, string productName, string productBuild, string productVersion);


        /// <summary>
        /// 设置用户信息
        /// </summary>
        /// <param name="userInfo"></param>
        [DllImport(dllPerfMonName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void ApmInit(string userInfo);

        [DllImport(dllPerfMonName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void ApmPostLog(string appID, string module, string logType, string bussiness, string subname, string errCode, string data, string extData);

        /// <summary>
        /// 启动hxperformance性能监控进程
        /// </summary>
        /// <returns></returns>
        public static bool StartPerformance()
        {
            string path = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "hxperformance.exe");
            int pid = APMPerfStartMonitorCS(path, AppID, "happ", "4.2.880.0");
            return pid > 0;
        }

        public static void AddApmLog()
        {

        }

        public static void AddApmHttpCountLog(string url)
        {
            var json = new JObject();
            json.Add("proxyVersion", "1.0");
            json.Add("url", url);
            var msg = json.ToString(Newtonsoft.Json.Formatting.None);
            ApmPostLog(AppID, "pc-http", "apm_count", "Http", "Count", "0", msg, "");
        }

        public static void AddApmHttpTimeoutLog(string url, string bussiness)
        {
            var json = new JObject();
            json.Add("proxyVersion", "1.0");
            json.Add("url", url);
            var msg = json.ToString(Newtonsoft.Json.Formatting.None);
            ApmPostLog(AppID, "pc-http", "apm_http_timeout", bussiness, "", "", msg, "");
        }

        public static void AddApmHttpTimeCostLog(string url, int timeCost, string bussiness)
        {
            var json = new JObject();
            json.Add("proxyVersion", "1.0");
            json.Add("url", url);
            json.Add("timeCost", timeCost);
            var msg = json.ToString(Newtonsoft.Json.Formatting.None);
            ApmPostLog(AppID, "pc-http", "apm_http_ok", bussiness, "", "0", msg, "");
        }

        public static void AddApmHttpErrorLog(string url, string errorCode, string bussiness, string extData = "")
        {
            var json = new JObject();
            json.Add("proxyVersion", "1.0");
            json.Add("url", url);
            var msg = json.ToString(Newtonsoft.Json.Formatting.None);
            ApmPostLog(AppID, "pc-http", "apm_http_error_request", bussiness, "", errorCode, msg, extData);
        }

        public static void AddApmErrorLog(string bussiness,string subName,string errorCode, string msg)
        {
            var json = new JObject();
            json.Add("msg", msg);
            var data = json.ToString(Newtonsoft.Json.Formatting.None);
            ApmPostLog(AppID, "pc-bussiness", "error", bussiness, subName, errorCode, data, "");
        }
    }
}
