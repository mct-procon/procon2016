using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Emgu.CV;

namespace PuzzleScanner.Pages {
    /// <summary>
    /// Main_FilterWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class Main_FilterWindow : Page {
        UMat mm = new UMat();
        Mat readed = new Mat();
        UMat cc = new UMat();
        

        public Main_FilterWindow() {
            InitializeComponent();
        }

        public Main_FilterWindow(string ImagePath) {
            InitializeComponent();
            readed = CvInvoke.Imread(ImagePath, Emgu.CV.CvEnum.LoadImageType.Color);
            CvInvoke.CvtColor(readed, mm, Emgu.CV.CvEnum.ColorConversion.Bgr2HsvFull);
            filter();
        }

        private void filter() {
            cc = FilterMat(mm, (byte)(H_MIN.Value * 255 / 360), (byte)(H_MAX.Value * 255 / 360), (byte)(S_MIN.Value * 255 / 100), (byte)(S_MAX.Value * 255 / 100), (byte)(V_MIN.Value * 255 / 100), (byte)(V_MAX.Value * 255 / 100));
            img.Source = Main_ScannerWindow.ToWPFBitmap(cc.Bitmap);
        }

        private void Slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e) {
            //if (!this.IsInitialized)
            //    return;
            //filter();
        }

        private UMat FilterMat(UMat mm, byte MIN_H, byte MAX_H, byte MIN_S, byte MAX_S, byte MIN_V, byte MAX_V) {
            UMat res = new UMat(mm.Rows, mm.Cols, Emgu.CV.CvEnum.DepthType.Cv8U, 1);
            byte[] cache_in = mm.Bytes;
            byte[] filtercache = res.Bytes;
            Parallel.For(0, filtercache.Length, (n) => {
                filtercache[n] = (cache_in[n * 3] > MIN_H && cache_in[n * 3] < MAX_H && cache_in[n * 3 + 1] < MAX_S && cache_in[n * 3 + 1] > MIN_S && cache_in[n * 3 + 2] < MAX_V && cache_in[n * 3 + 2] > MIN_V) ?
                    (byte)255 : (byte)0;
            });
            res.Bytes = filtercache;
            return res;
        }


        private void Update_Click(object sender, RoutedEventArgs e) {
            filter();
        }
        private void Next_Click(object sender, RoutedEventArgs e) {
            ((MainWindow)(Window.GetWindow(this))).MainFrame.Navigate(new Pages.Main_ScannerWindow(cc, readed));
        }
    }
}
