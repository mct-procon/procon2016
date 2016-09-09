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
using System.Windows.Controls.Primitives;

namespace PuzzleScanner.Pages {
    /// <summary>
    /// Main_FilterWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class Main_FilterWindow : Page {
        UMat mm = new UMat();
        Mat readed = new Mat();
        UMat cc = new UMat();

        int ImageHeight = 0;
        int ImageWidth = 0;

        double Scale = 1;

        public Main_FilterWindow() {
            InitializeComponent();
        }

        public Main_FilterWindow(string ImagePath) {
            InitializeComponent();
            Init2(ImagePath);
        }

        private void Init2(string ImagePath) {
            readed = CvInvoke.Imread(ImagePath, Emgu.CV.CvEnum.LoadImageType.Color);
            ImageWidth = readed.Width;
            ImageHeight = readed.Height;
            CvInvoke.CvtColor(readed, mm, Emgu.CV.CvEnum.ColorConversion.Bgr2HsvFull);
            filter();
            Scale = 1;
            BaseImg.Source = new BitmapImage(new Uri(ImagePath, UriKind.Absolute));
            imgCanvas.Width = ImageWidth;
            imgCanvas.Height = ImageHeight;
        }

        private void filter() {
            cc = FilterMat(mm, (byte)(H_MIN.Value * 255 / 360), (byte)(H_MAX.Value * 255 / 360), (byte)(S_MIN.Value * 255 / 100), (byte)(S_MAX.Value * 255 / 100), (byte)(V_MIN.Value * 255 / 100), (byte)(V_MAX.Value * 255 / 100));
            img.Source = Main_ScannerWindow.ToWPFBitmap(cc.Bitmap);
            Thumbnail.Source = img.Source;
        }

        private void Slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e) {
            //if (!this.IsInitialized)
            //    return;
            //filter();
        }

        /// <summary>
        /// HSVでフィルタします．
        /// </summary>
        /// <param name="mm">HSVなイメージ</param>
        /// <param name="MIN_H">最小H</param>
        /// <param name="MAX_H">最大H</param>
        /// <param name="MIN_S">最小S</param>
        /// <param name="MAX_S">最大S</param>
        /// <param name="MIN_V">最小V</param>
        /// <param name="MAX_V">最大V</param>
        /// <returns></returns>
        private UMat FilterMat(UMat mm, byte MIN_H, byte MAX_H, byte MIN_S, byte MAX_S, byte MIN_V, byte MAX_V) {
            UMat res = new UMat(mm.Rows, mm.Cols, Emgu.CV.CvEnum.DepthType.Cv8U, 1);
            byte[] cache_in = mm.Bytes;
            byte[] filtercache = res.Bytes;
            for (int n = 0; n < filtercache.Length; ++n) {
                filtercache[n] = (cache_in[n * 3] >= MIN_H && cache_in[n * 3] <= MAX_H && cache_in[n * 3 + 1] <= MAX_S && cache_in[n * 3 + 1] >= MIN_S && cache_in[n * 3 + 2] <= MAX_V && cache_in[n * 3 + 2] >= MIN_V) ?
                    (byte)255 : (byte)0;
            }
            res.Bytes = filtercache;
            return res;
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
            this.Scroller.ScrollToHorizontalOffset(
                this.Scroller.HorizontalOffset + (e.HorizontalChange * this.Scroller.ExtentWidth / this.Thumbnail.ActualWidth));

            this.Scroller.ScrollToVerticalOffset(
                this.Scroller.VerticalOffset + (e.VerticalChange * this.Scroller.ExtentHeight / this.Thumbnail.ActualHeight));
        }

        private void Update_Click(object sender, RoutedEventArgs e) {
            filter();
        }
        private void Next_Click(object sender, RoutedEventArgs e) {
            filter();
            ((MainWindow)Window.GetWindow(this)).MainFrame.Navigate(new Pages.Main_ScannerWindow(cc, readed));
        }

        private void Scroller_PreviewMouseWheel(object sender, MouseWheelEventArgs e) {
            if ((Keyboard.Modifiers & ModifierKeys.Control) != ModifierKeys.None) {
                ChangeCanvasScale(e.Delta);
                e.Handled = true;
            }
        }

        private void ChangeCanvasScale(int delta) {
            double centerX = (Scroller.HorizontalOffset + Scroller.ViewportWidth / 2);
            double centerY = (Scroller.VerticalOffset + Scroller.ViewportHeight / 2);
            double prev = Scale;
            Scale += delta * 0.001;
            if (Scale < 0) {
                Scale = prev;
                return;
            }

            imgCanvas.Height = ImageHeight * Scale;
            imgCanvas.Width = ImageWidth * Scale;

            CanvasScale.ScaleX = Scale;
            CanvasScale.ScaleY = Scale;

            Scroller.ScrollToHorizontalOffset((centerX / prev * Scale) - (Scroller.ViewportWidth / 2));
            Scroller.ScrollToVerticalOffset((centerY / prev * Scale) - (Scroller.ViewportHeight / 2));
        }
    }
}
