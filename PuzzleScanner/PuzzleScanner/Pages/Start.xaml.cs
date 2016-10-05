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
    /// Choose_Acceralator.xaml の相互作用ロジック
    /// </summary>
    public partial class Start : Page {
        public Start() {
            InitializeComponent();
        }

        private void OpenCLButtonClicked(object sender, RoutedEventArgs e) {
            CvInvoke.UseOpenCL = true;
            ((MainWindow)(Window.GetWindow(this))).MainFrame.Navigate(new Pages.LoadImageFile());
        }

        private void CPUButtonClicked(object sender, RoutedEventArgs e) {
            CvInvoke.UseOpenCL = false;
            ((MainWindow)(Window.GetWindow(this))).MainFrame.Navigate(new Pages.LoadImageFile());
        }

        private async void Page_Loaded(object sender, RoutedEventArgs e) {
            bool isOpenCLEnabled = await Task.Run(() => CvInvoke.HaveOpenCL); 
            OpenCL.IsEnabled = isOpenCLEnabled;
            ScanWithCameraButton.IsEnabled = true;
            ScanWithScannerButton.IsEnabled = true;
            BeginButton.IsEnabled = true;
            OpenCL.IsChecked = isOpenCLEnabled;
            App.ScannerImagePathes = new List<string>();
        }

        private void ScanWithScannerClicked(object sender, RoutedEventArgs e) {
            CvInvoke.UseOpenCL = OpenCL.IsChecked == true;
            App.IsScannerScan = true;
            App.ScannerScanCount = 0;
            ((MainWindow)(Window.GetWindow(this))).MainFrame.Navigate(new Pages.LoadImageFile());
        }

        private void ScanWithCameraClicked(object sender, RoutedEventArgs e) {
            CvInvoke.UseOpenCL = OpenCL.IsChecked == true;
            App.IsScannerScan = false;
            ((MainWindow)(Window.GetWindow(this))).MainFrame.Navigate(new Pages.LoadImageFile());
        }

        private void BeginButton_Click(object sender, RoutedEventArgs e) {
            ((MainWindow)(Window.GetWindow(this))).MainFrame.Navigate(new Pages.NewOverall());
        }
    }
}
