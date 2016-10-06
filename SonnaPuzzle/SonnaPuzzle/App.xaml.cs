using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

namespace SonnaPuzzle {
    /// <summary>
    /// App.xaml の相互作用ロジック
    /// </summary>
    public partial class App : Application {
        public static bool IsScannerScan = false;
        public static int ScannerScanCount = 0;
        public static List<string> ScannerImagePathes = new List<string>();

        public static byte[] Scanner_Filter;

        public static string SccannerScanCountString { get {
                return (ScannerScanCount == 0) ? "1st" : ((ScannerScanCount == 1) ? "2nd" : ((ScannerScanCount == 2) ? "3rd" : ($"{ScannerScanCount + 1}th")));
        } }
    }
}
