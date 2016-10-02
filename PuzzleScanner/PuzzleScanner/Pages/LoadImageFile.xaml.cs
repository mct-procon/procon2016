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
        int Scanner_Count = 0;
        public LoadImageFile() {
            InitializeComponent();
            if (App.IsScannerScan) {
                ActionTitle.Text = $"Choose {App.SccannerScanCountString} Image";
                Scanner_Count = App.ScannerScanCount;
                if (App.ScannerScanCount > 0)
                    GoScanButtonInScanner.Visibility = Visibility.Visible;
            }
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
            if (App.IsScannerScan) {
                if (App.ScannerScanCount <= Scanner_Count + 1)
                    App.ScannerImagePathes.Add(ReferencedImageFilePath);
                else
                    App.ScannerImagePathes[Scanner_Count] = ReferencedImageFilePath;
                if (App.ScannerScanCount == 0)
                    mw.MainFrame.Navigate(new Pages.Filter(ReferencedImageFilePath));
                else {
                    App.ScannerScanCount++;
                    mw.MainFrame.Navigate(new Pages.LoadImageFile());
                }
            } else
                mw.MainFrame.Navigate(new Pages.Filter(ReferencedImageFilePath));
        }

        private void GoScanButtonInScannerClick(object sender, RoutedEventArgs e) {
            if (App.IsScannerScan) {
                if (App.ScannerScanCount <= Scanner_Count + 1)
                    App.ScannerImagePathes.Add(ReferencedImageFilePath);
                else
                    App.ScannerImagePathes[Scanner_Count] = ReferencedImageFilePath;
            }
            MainWindow mw = (MainWindow)Window.GetWindow(this);
            mw.MainFrame.Navigate(new Pages.Scanner());
        }
    }
}
