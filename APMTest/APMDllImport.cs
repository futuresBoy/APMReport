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
        public delegate void PostErrorLogFunc(string message, int msgLength, string url, int urlLength);


        /// <summary>
        /// 上报性能信息回调函数
        /// </summary>
        /// <param name="message"></param>
        /// <param name="level"></param>
        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public delegate void PostPerformanceFunc(string message, int msgLength, string url, int urlLength);

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
        public static extern int APMInit(PostErrorLogFunc postLogFunc, PostPerformanceFunc postPerformanceFunc, APMLogFunc logFunc);

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
        /// 设置用户信息
        /// </summary>
        /// <param name="msg"></param>
        /// <returns></returns>
        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int SetUserInfo(string userID, string userName, string userAccount);


        /// <summary>
        /// 设置扩展用户信息
        /// </summary>
        /// <param name="msg"></param>
        /// <returns></returns>
        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int SetUserInfoEx(string appID, string userInfo);


        /// <summary>
        /// 添加日志
        /// </summary>
        /// <param name="msg"></param>
        /// <returns></returns>
        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int AddTraceLog(string appID, string moduleName, string subName, string errorCode, int monitorType, bool isSucceed, [MarshalAs(UnmanagedType.LPWStr)]string msg);


        ///// <summary>
        ///// 添加错误日志
        ///// </summary>
        ///// <param name="msg"></param>
        ///// <returns></returns>
        //[DllImport(dllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        //public static extern int AddErrorLog(string appID, string moduleName, string subName, string errorCode, [MarshalAs(UnmanagedType.LPWStr)]string msg);

        /// <summary>
        /// 添加错误日志
        /// </summary>
        /// <param name="msg"></param>
        /// <returns></returns>
        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int AddErrorLog(string appID, string module, string logType, string bussiness, string subName, string errorCode, [MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(UTF8Marshaler))]string data, [MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(UTF8Marshaler))]string extData);

        /// <summary>
        /// 添加HTTP日志
        /// </summary>
        /// <param name="msg"></param>
        /// <returns></returns>
        [DllImport(dllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int AddHTTPLog(string appID, string logType, string moduleName, string url, string errorCode, int costTime, [MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(UTF8Marshaler))]string data, [MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(UTF8Marshaler))]string extData);
    }

    //接口数据为utf-8编码所设置
    public class UTF8Marshaler : ICustomMarshaler
    {
        public void CleanUpManagedData(object managedObj)
        {
        }

        public void CleanUpNativeData(IntPtr pNativeData)
        {
            Marshal.FreeHGlobal(pNativeData);
        }

        public int GetNativeDataSize()
        {
            return -1;
        }

        public IntPtr MarshalManagedToNative(object managedObj)
        {
            if (object.ReferenceEquals(managedObj, null))
                return IntPtr.Zero;
            if (!(managedObj is string))
                throw new InvalidOperationException();

            byte[] utf8bytes = Encoding.UTF8.GetBytes(managedObj as string);
            IntPtr ptr = Marshal.AllocHGlobal(utf8bytes.Length + 1);
            Marshal.Copy(utf8bytes, 0, ptr, utf8bytes.Length);
            Marshal.WriteByte(ptr, utf8bytes.Length, 0);
            return ptr;
        }

        public object MarshalNativeToManaged(IntPtr pNativeData)
        {
            if (pNativeData == IntPtr.Zero)
                return null;

            List<byte> bytes = new List<byte>();
            for (int offset = 0; ; offset++)
            {
                byte b = Marshal.ReadByte(pNativeData, offset);
                if (b == 0) break;
                else bytes.Add(b);
            }
            return Encoding.UTF8.GetString(bytes.ToArray(), 0, bytes.Count);
        }

        private static UTF8Marshaler instance = new UTF8Marshaler();
        public static ICustomMarshaler GetInstance(string cookie)
        {
            return instance;
        }
    }
}
