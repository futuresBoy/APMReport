using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json.Linq;

namespace APMTestDemo
{
    /// <summary>
    /// 调用HxPerformance的Demo
    /// </summary>
    class HxPerformanceTest
    {
        public void Test()
        {
            HxPerformanceImport.StartPerformance();

            var userInfo = new JObject();
            userInfo.Add("appID", "hevo-client-future");
            userInfo.Add("userId", "1111111");
            userInfo.Add("userName", "徐侃");
            userInfo.Add("productVersion", "1.0.0.0");
            var josn = userInfo.ToString(Newtonsoft.Json.Formatting.None);
            
            HxPerformanceImport.ApmInit(josn);
            TestHttpLog();
        }

        public void TestHttpLog()
        {
            HxPerformanceImport.AddApmHttpCountLog("https://ftapi.10jqka.com.cn/ljapi/futures/contract/basic");

            HxPerformanceImport.AddApmHttpTimeCostLog("https://ftapi.10jqka.com.cn/ljapi/futures/contract/basic", 1000,"Demo");
            
            HxPerformanceImport.AddApmHttpErrorLog("https://testm.10jqka.com.cn/mobile/info/ftapi/ljapi/futuresactivity/simulatetradematch/join/?userid=111", "-33", "Demo", "decode failed");

            HxPerformanceImport.AddApmHttpTimeoutLog("https://ftapi.10jqka.com.cn/futgwapi/api/market/v1/options/market/info", "Demo");

            HxPerformanceImport.AddApmErrorLog("Demo", "TestAddErrorLog", "-1", "test English");
            HxPerformanceImport.AddApmErrorLog("Demo", "TestAddErrorLog", "-1", "测试中文");
        }
    }
}
