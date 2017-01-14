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
using System.Diagnostics;

namespace PuzzleScanner.Pages {
    /// <summary>
    /// Main_FilterWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class Filter : Page {
        string ImagePath;

        UMat HsvImgData = new UMat();
        Mat ReadedImgData = new Mat();
        UMat FilteredImgData = new UMat();
        System.Drawing.Bitmap bmp;

        int ImageHeight = 0;
        int ImageWidth = 0;

        double Scale = 1;

        Task afterTask = Task.Run(() => { });
        Task filterTask = Task.Run(() => { });

        (byte H_MIN, byte H_MAX, byte S_MIN, byte S_MAX, byte V_MIN, byte V_MAX) lastInfo = (0, 0, 0, 0, 0, 0);
        bool changed = false;

        public Filter() {
            InitializeComponent();
        }

        public Filter(string imgPath) {
            InitializeComponent();
            ImagePath = imgPath;
        }

        private async void Page_Loaded(object sender, RoutedEventArgs e) {
            if (string.IsNullOrWhiteSpace(ImagePath))
                return;
            await Task.Run(() => {
                ReadedImgData = CvInvoke.Imread(ImagePath, Emgu.CV.CvEnum.LoadImageType.Color);
                ImageWidth = ReadedImgData.Width;
                ImageHeight = ReadedImgData.Height;
                CvInvoke.CvtColor(ReadedImgData, HsvImgData, Emgu.CV.CvEnum.ColorConversion.Bgr2HsvFull);
                FilteredImgData = new UMat(HsvImgData.Rows, HsvImgData.Cols, Emgu.CV.CvEnum.DepthType.Cv8U, 1);
            });
            imgCanvas.Width = ImageWidth;
            imgCanvas.Height = ImageHeight;
            filter();
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

            NextButton.IsEnabled = true;
        }

        private void filter() {
            var filterinfo = GetFilterInfo();
            if (filterTask.Status == TaskStatus.Running || afterTask.Status == TaskStatus.Running) {
                lastInfo = filterinfo;
                changed = true;
                Debug.WriteLine("Changed!");
            } else {
                TaskScheduler t_s = TaskScheduler.FromCurrentSynchronizationContext();

                void afterAct(Task t)
                {
                    bmp = FilteredImgData.Bitmap;
                    img.Source = Controls.FilterPage.ToWPFBitmap(bmp);
                    bmp.Dispose();
                    Thumbnail.Source = img.Source;
                    if (changed) {
                        Debug.WriteLine("AfterAct");

                        changed = false;
                        filterTask = Task.Run(() => FilterMat(ref HsvImgData, ref FilteredImgData, lastInfo.H_MIN, lastInfo.H_MAX, lastInfo.S_MIN, lastInfo.S_MAX, lastInfo.V_MIN, lastInfo.V_MAX));
                        afterTask = filterTask.ContinueWith(afterAct, t_s);
                    }
                };

                filterTask = Task.Run(() => FilterMat(ref HsvImgData, ref FilteredImgData, filterinfo.H_MIN, filterinfo.H_MAX, filterinfo.S_MIN, filterinfo.S_MAX, filterinfo.V_MIN, filterinfo.V_MAX));
                afterTask = filterTask.ContinueWith(afterAct, t_s);
            }
        }

        private void Slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e) {
            if (!this.IsInitialized || !this.IsLoaded)
                return;
            filter();
        }

        private (byte H_MIN, byte H_MAX, byte S_MIN, byte S_MAX, byte V_MIN, byte V_MAX) GetFilterInfo() =>
            ((byte)(H_min.Value * 255 / 360), (byte)(H_max.Value * 255 / 360), (byte)(S_min.Value * 255 / 100), (byte)(S_max.Value * 255 / 100), (byte)(V_min.Value * 255 / 100), (byte)(V_max.Value * 255 / 100));

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
            if(cc.Rows != mm.Rows || cc.Cols != mm.Cols)
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
            //cc.Bytes = PuzzleScanner.Utils.ParallelExtensions.Merge(tasks).ToArray();
            byte[] filtercache = cc.Bytes;
            int cache_at = 0;
            for (int n = 0; n < filtercache.Length; ++n) {
                filtercache[n] = (cache_in[cache_at] >= MIN_H && cache_in[cache_at] <= MAX_H && cache_in[cache_at + 1] <= MAX_S && cache_in[cache_at + 1] >= MIN_S && cache_in[cache_at + 2] <= MAX_V && cache_in[cache_at + 2] >= MIN_V) ?
                    (byte)255 : (byte)0;
                cache_at += 3;
            }
            cc.Bytes = filtercache;
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
        private void Next_Click(object sender, RoutedEventArgs e) {
            filter();
            if (App.IsScannerScan) {
                App.Scanner_Filter = new byte[] { (byte)(H_min.Value * 255 / 360), (byte)(H_max.Value * 255 / 360), (byte)(S_min.Value * 255 / 100),
                    (byte)(S_max.Value * 255 / 100), (byte)(V_min.Value * 255 / 100), (byte)(V_max.Value * 255 / 100) };
                App.ScannerScanCount++;
                ((MainWindow)Window.GetWindow(this)).MainFrame.Navigate(new Pages.LoadImageFile());
            } else {
                ((MainWindow)Window.GetWindow(this)).MainFrame.Navigate(new Pages.Scanner(FilteredImgData, ReadedImgData));
            }
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

        private void Page_Unloaded(object sender, RoutedEventArgs e) {
            HsvImgData.Dispose();
        }
    }
}
