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
using Microsoft.Win32;
using Emgu.CV;

namespace PuzzleScanner.Pages {
    /// <summary>
    /// LoadImageFile.xaml の相互作用ロジック
    /// </summary>
    public partial class LoadImageFile : Page {
        string ReferencedImageFilePath = "";
        public LoadImageFile() {
            InitializeComponent();
        }




        private void ReferenceButtonClick(object sender,RoutedEventArgs e) {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "All(*)|*.*";
            if(ofd.ShowDialog() == true) {
                NextButton.Visibility = Visibility.Collapsed;
                try {
                    Previewer.Source = new BitmapImage(new Uri(ofd.FileName, UriKind.Absolute));
                    ReferencedImageFilePath = ofd.FileName;
                    NextButton.Visibility = Visibility.Visible;
                }catch { }
            }
        }

        private void NextButtonClick(object sender, RoutedEventArgs e) {
            MainWindow mw = (MainWindow)Window.GetWindow(this);
            mw.BackToImageChooseButton.Visibility = Visibility.Visible;
            mw.MainFrame.Navigate(new Pages.Filter(ReferencedImageFilePath));
        }

        private void CalibButton_Click(object sender, RoutedEventArgs e) {
            //CvInvoke.NamedWindow("debugwindow");
            //int HorizonalCrossCount = 39;
            //int VerticalCrossCount = 29;
            //Mat frame = CvInvoke.Imread(ReferencedImageFilePath);
            //Mat gray;
            //CvInvoke.Flip(frame, frame, Emgu.CV.CvEnum.FlipType.None);
            //CvInvoke.CvtColor(frame, gray, Emgu.CV.CvEnum.ColorConversion.Bgr2Gray);

        }
    }
}
