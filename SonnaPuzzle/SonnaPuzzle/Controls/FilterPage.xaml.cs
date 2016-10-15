using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
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

namespace SonnaPuzzle.Controls {
    /// <summary>
    /// FilterPage.xaml の相互作用ロジック
    /// </summary>
    public partial class FilterPage : Grid {
        string ImagePath;

        UMat HsvImgData = new UMat();
        public Mat ReadedImgData = new Mat();
        public UMat FilteredImgData = new UMat();

        System.Drawing.Bitmap bmp;

        int ImageHeight = 0;
        int ImageWidth = 0;

        double Scale = 1;

        private bool SecondaryLoad = false;

        Task filterTask;
        CancellationTokenSource filter_cancelToken = new CancellationTokenSource();

        public FilterPage() {
            InitializeComponent();
        }

        public FilterPage(string imgPath) {
            InitializeComponent();
            ImagePath = imgPath;
        }

        private void Grid_Loaded(object sender, RoutedEventArgs e) {
            if (SecondaryLoad)
                return;
            if (string.IsNullOrWhiteSpace(ImagePath))
                return;
            TaskScheduler t_s = TaskScheduler.FromCurrentSynchronizationContext();
            Task.Run(() => {
                ReadedImgData = CvInvoke.Imread(ImagePath, Emgu.CV.CvEnum.LoadImageType.Color);
                ImageWidth = ReadedImgData.Width;
                ImageHeight = ReadedImgData.Height;
                CvInvoke.CvtColor(ReadedImgData, HsvImgData, Emgu.CV.CvEnum.ColorConversion.Bgr2HsvFull);
                FilteredImgData = new UMat(HsvImgData.Rows, HsvImgData.Cols, Emgu.CV.CvEnum.DepthType.Cv8U, 1);
            }).ContinueWith((_) => {
                //Dispatcher.BeginInvoke((Action)(() => {
                    filter();
                    imgCanvas.Width = ImageWidth;
                    imgCanvas.Height = ImageHeight;
                    Scale = 1;
                    BaseImg.Source = new BitmapImage(new Uri(ImagePath, UriKind.Absolute));


                    // ExtentWidth/Height が ScrollViewer 内の広さ
                    // ViewportWidth/Height が ScrollViewer で実際に表示されているサイズ

                    var xfactor = this.Thumbnail.ActualWidth / Scroller.ExtentWidth;
                    var yfactor = this.Thumbnail.ActualHeight / Scroller.ExtentHeight;

                    var width = Scroller.ViewportWidth * xfactor;
                    if (width > this.Thumbnail.ActualWidth) width = this.Thumbnail.ActualWidth;

                    var height = Scroller.ViewportHeight * yfactor;
                    if (height > this.Thumbnail.ActualHeight) height = this.Thumbnail.ActualHeight;

                    // Canvas (親パネル) 上での Viewport の位置を、Left/Top 添付プロパティで設定
                    // (XAML で言う <Border Canvas.Left="0" ... \> みたいなやつ)
                    Canvas.SetLeft(this.Viewport, 0);
                    Canvas.SetTop(this.Viewport, 0);

                    this.Viewport.Width = width;
                    this.Viewport.Height = height;

                    ScanButton.IsEnabled = true;
                    SecondaryLoad = true;
                //}));
            }, t_s);
        }

        private void filter() {
            byte _h_min = (byte)(H_min.Value * 255 / 360);
            byte _h_max = (byte)(H_max.Value * 255 / 360);
            byte _s_min = (byte)(S_min.Value * 255 / 100);
            byte _s_max = (byte)(S_max.Value * 255 / 100);
            byte _v_min = (byte)(V_min.Value * 255 / 100);
            byte _v_max = (byte)(V_max.Value * 255 / 100);
            TaskScheduler t_s = TaskScheduler.FromCurrentSynchronizationContext();
            filterTask = Task.Run(() => FilterMat(ref HsvImgData, ref FilteredImgData, _h_min, _h_max, _s_min, _s_max, _v_min, _v_max), filter_cancelToken.Token);
            filterTask.ContinueWith((t) => {
                if (!filterTask.IsCanceled) {
                    bmp = FilteredImgData.Bitmap;
                    img.Source = ToWPFBitmap(bmp);
                    bmp.Dispose();
                    Thumbnail.Source = img.Source;
                }
            }, t_s);
        }

        private void Slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e) {
            if (!this.IsInitialized || !this.IsLoaded)
                return;
            byte _h_min = (byte)(H_min.Value * 255 / 360);
            byte _h_max = (byte)(H_max.Value * 255 / 360);
            byte _s_min = (byte)(S_min.Value * 255 / 100);
            byte _s_max = (byte)(S_max.Value * 255 / 100);
            byte _v_min = (byte)(V_min.Value * 255 / 100);
            byte _v_max = (byte)(V_max.Value * 255 / 100);
            if (filterTask.IsCompleted) {
                TaskScheduler t_s = TaskScheduler.FromCurrentSynchronizationContext();
                filterTask = Task.Run(() => FilterMat(ref HsvImgData, ref FilteredImgData, _h_min, _h_max, _s_min, _s_max, _v_min, _v_max), filter_cancelToken.Token);
                filterTask.ContinueWith((t) => {
                    if (!filterTask.IsCanceled) {
                        bmp = FilteredImgData.Bitmap;
                        img.Source = ToWPFBitmap(bmp);
                        bmp.Dispose();
                        Thumbnail.Source = img.Source;
                    }
                }, t_s);
            }

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
        internal static void FilterMat(ref UMat mm, ref UMat cc, byte MIN_H, byte MAX_H, byte MIN_S, byte MAX_S, byte MIN_V, byte MAX_V) {
            if (cc.Rows != mm.Rows || cc.Cols != mm.Cols)
                cc = new UMat(mm.Rows, mm.Cols, Emgu.CV.CvEnum.DepthType.Cv8U, 1);
            byte[] cache_in = mm.Bytes;

            //--------------------------
            //　　並列化した方が遅かった…
            //--------------------------
            //int thread_job = cc.Bytes.Length / Environment.ProcessorCount;
            //int surplus = cc.Bytes.Length - (thread_job * Environment.ProcessorCount);
            //Task<byte[]>[] tasks = new Task<byte[]>[Environment.ProcessorCount];
            //int cache_iter = 0;
            //for(int n = 0; n < tasks.Length; ++n) {
            //    int m = n < surplus ? thread_job + 1 : thread_job;
            //    _data_Vector3_filterMat[] cache_data = new _data_Vector3_filterMat[m];
            //    for (int p = 0; p < m; ++p) {
            //        cache_data[p] = new _data_Vector3_filterMat();
            //        cache_data[p].H = cache_in[cache_iter];
            //        cache_iter++;
            //        cache_data[p].S = cache_in[cache_iter];
            //        cache_iter++;
            //        cache_data[p].V = cache_in[cache_iter];
            //        cache_iter++;
            //    }

            //    tasks[n] = Task.Run(() => {
            //        byte[] _return_cache = new byte[m];
            //        for (int p = 0; p < m; ++p) {
            //            _return_cache[p] = (cache_data[p].H >= MIN_H && cache_data[p].H <= MAX_H && 
            //            cache_data[p].S <= MAX_S && cache_data[p].S >= MIN_S && cache_data[p].V <= MAX_V && cache_data[p].V >= MIN_V) ?
            //                (byte)255 : (byte)0;
            //        }
            //        return _return_cache;
            //    });
            //}
            //Task.WaitAll(tasks);
            //cc.Bytes = SonnaPuzzle.Utils.ParallelExtensions.Merge(tasks).ToArray();
            byte[] filtercache = cc.Bytes;
            int cache_at = 0;
            for (int n = 0; n < filtercache.Length; ++n) {
                filtercache[n] = (cache_in[cache_at] >= MIN_H && cache_in[cache_at] <= MAX_H && cache_in[cache_at + 1] <= MAX_S && cache_in[cache_at + 1] >= MIN_S && cache_in[cache_at + 2] <= MAX_V && cache_in[cache_at + 2] >= MIN_V) ?
                    (byte)255 : (byte)0;
                cache_at += 3;
            }
            cc.Bytes = filtercache;
        }

        //private struct _data_Vector3_filterMat {
        //    public byte H, S, V;
        //    public _data_Vector3_filterMat(byte _h, byte _s, byte _v) {
        //        H = _h;
        //        S = _s;
        //        V = _v;
        //    }
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

        private void OnDragDelta(object sender, DragDeltaEventArgs e) {
            if (!this.IsLoaded)
                return;
            this.Scroller.ScrollToHorizontalOffset(
                this.Scroller.HorizontalOffset + (e.HorizontalChange * this.Scroller.ExtentWidth / this.Thumbnail.ActualWidth));

            this.Scroller.ScrollToVerticalOffset(
                this.Scroller.VerticalOffset + (e.VerticalChange * this.Scroller.ExtentHeight / this.Thumbnail.ActualHeight));
        }

        private void Update_Click(object sender, RoutedEventArgs e) {
            filter();
        }
        //private void Next_Click(object sender, RoutedEventArgs e) {
        //    filter();
        //    if (App.IsScannerScan) {
        //        App.Scanner_Filter = new byte[] { (byte)(H_MIN.Value * 255 / 360), (byte)(H_MAX.Value * 255 / 360), (byte)(S_MIN.Value * 255 / 100),
        //            (byte)(S_MAX.Value * 255 / 100), (byte)(V_MIN.Value * 255 / 100), (byte)(V_MAX.Value * 255 / 100) };
        //        App.ScannerScanCount++;
        //        ((MainWindow)Window.GetWindow(this)).MainFrame.Navigate(new Pages.LoadImageFile());
        //    } else {
        //        ((MainWindow)Window.GetWindow(this)).MainFrame.Navigate(new Pages.Scanner(cc, readed));
        //    }
        //}

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
            Scale += delta * 0.0001;
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

        public void Mats_Unload() {
            HsvImgData.Dispose();
        }


        /// <summary>
        /// BitmapからWPFのBitmapSourceに変換するメソッド
        /// </summary>
        /// <param name="bitmap">変換もとBitmap</param>
        /// <returns>変換したBitmapSource</returns>
        public static BitmapSource ToWPFBitmap(System.Drawing.Bitmap bitmap) {
            var hBitmap = bitmap.GetHbitmap();

            BitmapSource source;
            try {
                source = System.Windows.Interop.Imaging.CreateBitmapSourceFromHBitmap(
                    hBitmap, IntPtr.Zero, Int32Rect.Empty,
                    BitmapSizeOptions.FromEmptyOptions());
            } finally {
                DeleteObject(hBitmap);
            }
            return source;
        }

        [System.Runtime.InteropServices.DllImport("gdi32.dll")]
        public static extern bool DeleteObject(IntPtr hObject);

        private void CameraHonbanButton_Click(Object sender, RoutedEventArgs e) {
            H_max.Value = 360;
            H_min.Value = 0;
            S_max.Value = 100;
            S_min.Value = 63;
            V_min.Value = 27;
            V_max.Value = 100;
        }

        private void ForceFilter_Click(Object sender, RoutedEventArgs e) {
            filter();
        }
    }
}
