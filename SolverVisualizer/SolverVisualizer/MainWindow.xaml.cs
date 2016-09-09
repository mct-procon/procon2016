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
using System.Diagnostics;
using WPFShape = System.Windows.Shapes;
using Microsoft.Win32;
using System.Windows.Controls.Primitives;

namespace SolverVisualizer {
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window {
        DataReader Data = new DataReader();
        double Scale = 1;

        public MainWindow() {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e) {
            Load();
        }

        internal async void Load() {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Text Files|*.txt|All Files|*.*";
            ofd.FilterIndex = 0;
            ofd.Multiselect = false;
            if (ofd.ShowDialog() != true)
                return;
            await Data.LoadAsync(ofd.FileName);
            PolygonCanvas.Height = Data.MaxSize.Y;
            PolygonCanvas.Width = Data.MaxSize.X;
            foreach(var p in Data.Polygons) {
                PolygonCanvas.Children.Add(new WPFShape.Polygon() { Points = p.Points, Stroke = new SolidColorBrush(p.FrameColor) });
            }
            Thumbnail.Width = 400;
            Thumbnail.Height = Data.MaxSize.Y * (400 / Data.MaxSize.X);
        }

        private void UpdateThumbnailViewport(object sender, ScrollChangedEventArgs e) {
            // ExtentWidth/Height が ScrollViewer 内の広さ
            // ViewportWidth/Height が ScrollViewer で実際に表示されているサイズ

            var xfactor = this.Thumbnail.ActualWidth / e.ExtentWidth;
            var yfactor = this.Thumbnail.ActualHeight / e.ExtentHeight;

            var left = e.HorizontalOffset * xfactor;
            var top = e.VerticalOffset * yfactor;

            var width = e.ViewportWidth * xfactor;
            if (width > this.Thumbnail.ActualWidth) width = this.Thumbnail.ActualWidth;

            var height = e.ViewportHeight * yfactor;
            if (height > this.Thumbnail.ActualHeight) height = this.Thumbnail.ActualHeight;

            // Canvas (親パネル) 上での Viewport の位置を、Left/Top 添付プロパティで設定
            // (XAML で言う <Border Canvas.Left="0" ... \> みたいなやつ)
            Canvas.SetLeft(this.Viewport, left);
            Canvas.SetTop(this.Viewport, top);

            this.Viewport.Width = width;
            this.Viewport.Height = height;
        }

        private void OnDragDelta(object sender, DragDeltaEventArgs e) {
            this.ScrollViewer.ScrollToHorizontalOffset(
                this.ScrollViewer.HorizontalOffset + (e.HorizontalChange * this.ScrollViewer.ExtentWidth / this.Thumbnail.ActualWidth));

            this.ScrollViewer.ScrollToVerticalOffset(
                this.ScrollViewer.VerticalOffset + (e.VerticalChange * this.ScrollViewer.ExtentHeight / this.Thumbnail.ActualHeight));
        }

        private void ScrollViewer_PreviewMouseWheel(object sender, MouseWheelEventArgs e) {
            if ((Keyboard.Modifiers & ModifierKeys.Control) != ModifierKeys.None) {
                ChangeCanvasScale(e.Delta);
                e.Handled = true;
            }
        }

        private void ChangeCanvasScale(int delta) {
            double centerX = (ScrollViewer.HorizontalOffset + ScrollViewer.ViewportWidth / 2);
            double centerY = (ScrollViewer.VerticalOffset + ScrollViewer.ViewportHeight / 2);
            double prev = Scale;
            Scale += delta * 0.001;
            if (Scale < 0) {
                Scale = prev;
                return;
            }

            PolygonCanvas.Height = Data.MaxSize.Y * Scale;
            PolygonCanvas.Width = Data.MaxSize.X * Scale;

            CanvasScale.ScaleX = Scale;
            CanvasScale.ScaleY = Scale;

            ScrollViewer.ScrollToHorizontalOffset((centerX / prev * Scale) - (ScrollViewer.ViewportWidth / 2));
            ScrollViewer.ScrollToVerticalOffset((centerY / prev * Scale) - (ScrollViewer.ViewportHeight / 2));
        }
    }
}
