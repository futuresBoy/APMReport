using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace APMTestDemo
{
    class HxPerformanceImport
    {
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
        /// 启动hxperformance性能监控进程
        /// </summary>
        /// <returns></returns>
        public static bool StartPerformance()
        {
            string path = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "performance", "hxperformance.exe");
            int pid = APMPerfStartMonitorCS(path, "hevo-client-future", "happ", "4.2.880.0");
            return pid > 0;
        }
    }
}
