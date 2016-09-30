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

namespace PuzzleScanner {
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window {
        public MainWindow() {
            InitializeComponent();
        }

        private void Window_StateChanged(object sender, EventArgs e) =>
            WindowNorMaxChangeButton.Content = WindowState == WindowState.Normal ? "1" : "2";


        private void CloseButton_Clicked(object sender, RoutedEventArgs e) =>
            Environment.Exit(0);

        private void NorMaxButton_Clicked(object sender, RoutedEventArgs e) =>
            WindowState = WindowState == WindowState.Normal ? WindowState.Maximized : WindowState.Normal;

        private void MiniButton_Clicked(object sender, RoutedEventArgs e) =>
            WindowState = WindowState.Minimized;

        private void Window_Loaded(object sender, RoutedEventArgs e) {
            MainFrame.Navigate(new Pages.Start());
        }

        private void BackToImageChooseButton_Click(object sender, RoutedEventArgs e) {
            MainFrame.Navigate(new Pages.Start());
        }
    }
}
