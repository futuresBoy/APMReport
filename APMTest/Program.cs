using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using Newtonsoft.Json;

namespace APMTestDemo
{
    class Program
    {
        static void Main(string[] args)
        {
            //new Demo().Go();
            new HxPerformanceTest().Test();
            

            Console.ReadLine();
        }

    }
}
