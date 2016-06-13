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
    public partial class Choose_Acceralator : Page {
        public Choose_Acceralator() {
            InitializeComponent();
            OpenCLButton.IsEnabled = CvInvoke.HaveOpenCL;
        }

        private void OpenCLButtonClicked(object sender, RoutedEventArgs e) {
            CvInvoke.UseOpenCL = true;
            ((MainWindow)(Window.GetWindow(this))).MainFrame.Navigate(new Pages.LoadImageFile());
        }

        private void CPUButtonClicked(object sender, RoutedEventArgs e) {
            CvInvoke.UseOpenCL = false;
            ((MainWindow)(Window.GetWindow(this))).MainFrame.Navigate(new Pages.LoadImageFile());
        }
    }
}
