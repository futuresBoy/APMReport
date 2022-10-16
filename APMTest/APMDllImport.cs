using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace APMTestDemo
{
    public class APMDllImport
    {
        const string dllName = "APMReportEngine.dll";

        /// <summary>
        /// 接收日志打印回调函数
        /// </summary>
        /// <param name="message"></param>
        /// <param name="level"></param>
        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public delegate void APMLogFunc(string message, int level);

        /// <summary>
        /// 上报异常日志回调函数
        /// </summary>
        /// <param name="message"></param>
        /// <param name="level"></param>
        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public delegate void PostErrorLogFunc(string message, int length, string url);

        /// <summary>
        /// 初始化SDK日志
        /// </summary>
        /// <returns></returns>
        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int InitLogger(APMLogFunc logFunc);

        /// <summary>
        /// SDK初始化
        /// </summary>
        /// <param name="logFunc"></param>
        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int APMInit(PostErrorLogFunc postLogFunc, APMLogFunc logFunc);

        /// <summary>
        /// 获取SDK版本
        /// </summary>
        /// <returns></returns>
        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern IntPtr GetSDKVersion();

        /// <summary>
        /// 给SDK设置RSA公钥
        /// </summary>
        /// <param name="pubKeyID"></param>
        /// <param name="pubKey"></param>
        /// <returns></returns>
        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int SetRSAPubKey(string pubKeyID, string pubKey);

        /// <summary>
        /// 设置客户端基础信息
        /// </summary>
        /// <param name="baseInfo"></param>
        /// <param name="outText"></param>
        /// <param name="length"></param>
        /// <returns></returns>
        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int SetClientInfo(string baseInfo, IntPtr outText, ref int length);

        /// <summary>
        /// 组装性能数据
        /// </summary>
        /// <param name="msg"></param>
        /// <param name="outText"></param>
        /// <param name="length"></param>
        /// <returns></returns>
        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int BuildPerformanceData(string appID, string msg, IntPtr outText, ref int length);

        /// <summary>
        /// 设置阈值配置
        /// </summary>
        /// <param name="json"></param>
        /// <returns></returns>
        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int SetReportConfig(string json);

        /// <summary>
        /// 设置开关配置
        /// </summary>
        /// <param name="json"></param>
        /// <returns></returns>
        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int SetReportSwitch(string json);

        /// <summary>
        /// 添加错误日志
        /// </summary>
        /// <param name="msg"></param>
        /// <returns></returns>
        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int AddErrorLog(string appID, string msg);

        /// <summary>
        /// 添加错误日志
        /// </summary>
        /// <param name="msg"></param>
        /// <returns></returns>
        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int AddTraceLog(string appID, string moduleName, string subName, string errorCode, int monitorType, bool isSucceed, char[] msgArray, int[] msgLengthArray, int arrayCount);

    }
}
