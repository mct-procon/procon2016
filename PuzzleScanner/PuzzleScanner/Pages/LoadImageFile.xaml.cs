﻿using System;
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
        MainWindow mw;
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
            mw.BackToImageChooseButton.Visibility = Visibility.Visible;
            mw.MainFrame.Navigate(new Pages.Main_FilterWindow(ReferencedImageFilePath));
        }

        private void Page_Loaded(object sender, RoutedEventArgs e) {
            mw = (MainWindow)(Window.GetWindow(this));
            ActionTitle.Text = mw.Frames == null ? "Choose the Frame's Image" : "Choose the Pieces' Image";
        }
    }
}
