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
            imgCanvas.Height = ImageHeight * ZoomSlider.Value;
            imgCanvas.Width = ImageWidth * ZoomSlider.Value;
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

        private UMat FilterMat(UMat mm, byte MIN_H, byte MAX_H, byte MIN_S, byte MAX_S, byte MIN_V, byte MAX_V) {
            UMat res = new UMat(mm.Rows, mm.Cols, Emgu.CV.CvEnum.DepthType.Cv8U, 1);
            byte[] cache_in = mm.Bytes;
            byte[] filtercache = res.Bytes;
            Parallel.For(0, filtercache.Length, (n) => {
                filtercache[n] = (cache_in[n * 3] >= MIN_H && cache_in[n * 3] <= MAX_H && cache_in[n * 3 + 1] <= MAX_S && cache_in[n * 3 + 1] >= MIN_S && cache_in[n * 3 + 2] <= MAX_V && cache_in[n * 3 + 2] >= MIN_V) ?
                    (byte)255 : (byte)0;
            });
            res.Bytes = filtercache;
            return res;
        }

        //private UMat FilterMatWithGPU(UMat mm, byte MIN_H, byte MAX_H, byte MIN_S, byte MAX_S, byte MIN_V, byte MAX_V) {
        //    UMat res = new UMat(mm.Rows, mm.Cols, Emgu.CV.CvEnum.DepthType.Cv8U, 1);
        //    byte[] filtercache = res.Bytes;
        //    byte[] settingdata = { MIN_H, MAX_H, MIN_S, MAX_S, MIN_V, MAX_V };
        //    CudafyModule km = Cudafy.Translator.CudafyTranslator.Cudafy();
        //    GPGPU gpu = CudafyHost.GetDevice(CudafyModes.Target, CudafyModes.DeviceId);
        //    byte[] dev_filtercache = gpu.Allocate<byte>(filtercache);
        //    byte[] dev_cache_in = gpu.CopyToDevice(mm.Bytes);
        //    byte[] dev_settingdata = gpu.CopyToDevice(settingdata);
        //    gpu.Launch(mm.Bytes.Length, 1).FilterMatGPUKernel(dev_cache_in, dev_filtercache, dev_settingdata);
        //    gpu.CopyFromDevice(dev_filtercache, filtercache);
        //    gpu.FreeAll();
        //    res.Bytes = filtercache;
        //    return res;
        //}

        //[Cudafy]
        //private static void FilterMatGPUKernel(GThread gThread, byte[] dataIn, byte[] dataOut, byte[] settingdata) {
        //    int n = gThread.blockIdx.x;
        //    dataOut[n] = (dataIn[n * 3] >= settingdata[0] && dataIn[n * 3] <= settingdata[1] && dataIn[n * 3 + 1] <= settingdata[3] && dataIn[n * 3 + 1] >= settingdata[2] && dataIn[n * 3 + 2] <= settingdata[5] && dataIn[n * 3 + 2] >= settingdata[4]) ?
        //            (byte)255 : (byte)0;
        //}

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

        private void ZoomSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e) {
            if (!IsInitialized)
                return;
            imgCanvas.Height = ImageHeight * ZoomSlider.Value;
            imgCanvas.Width = ImageWidth * ZoomSlider.Value;
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
            ((MainWindow)(Window.GetWindow(this))).MainFrame.Navigate(new Pages.Main_ScannerWindow(cc, readed));
        }
    }
}
