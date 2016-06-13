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
        UMat cache = new UMat();
        
        public Main_FilterWindow() {
            InitializeComponent();
        }

        public Main_FilterWindow(string ImagePath) {
            InitializeComponent();
            Mat cache = CvInvoke.Imread(ImagePath, Emgu.CV.CvEnum.LoadImageType.Color);
            CvInvoke.CvtColor(cache, mm, Emgu.CV.CvEnum.ColorConversion.Bgr2HsvFull);
            filter();
        }

        private void filter() {
            CvInvoke.InRange(mm, new Emgu.CV.ScalarArray(new Emgu.CV.Structure.MCvScalar(H_MIN.Value * 255 / 360,S_MIN.Value * 255 / 100 , V_MIN.Value * 255 / 100)), new Emgu.CV.ScalarArray(new Emgu.CV.Structure.MCvScalar(H_MAX.Value * 255 / 360, S_MAX.Value * 255 / 100, V_MAX.Value * 255 / 100)), cache);
            img.Source = Main_ScannerWindow.ToWPFBitmap(cache.Bitmap);
        }

        private void Slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e) {
            //if (!this.IsInitialized)
            //    return;
            //filter();
        }

        private void Update_Click(object sender, RoutedEventArgs e) {
            filter();
        }
    }
}
