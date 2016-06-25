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
using System.Collections;
using Emgu.CV;
using System.Windows.Controls.Primitives;
using System.Diagnostics;
using Point = System.Drawing.Point;

namespace PuzzleScanner.Pages {
    /// <summary>
    /// Main_ScannerWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class Main_ScannerWindow : Page {
        private double NormalCircleSize = 0;
        private double NormalLineTickness = 0;
        private double Offset = 0;

        //private int MAX_S = 220;
        //private int MIN_S = 70;

        //private int MAX_V = 160;
        //private int MIN_V = 10;

        //private int MAX_H = 50;
        //private int MIN_H = 0;
        string ImagePath = "";
        int ImageWidth = 0;
        int ImageHeight = 0;

        List<ResultPolygonData> result = new List<ResultPolygonData>();

        public Main_ScannerWindow() {
            InitializeComponent();
        }

        //public Main_ScannerWindow(string imagepath) {
        //    ImagePath = imagepath;
        //    InitializeComponent();
        //    UpdateContent();
        //}

        public Main_ScannerWindow(UMat filteredImg, Mat ReadedImg) {
            InitializeComponent();
            UpdateContent_WithoutFilter(filteredImg, ReadedImg);
        }
        //#region old
        //private async void UpdateContent() {
        //    throw new Exception("Don't use!!!!");
        //    Waiter.Opacity = 0;
        //    Waiter.Visibility = Visibility.Visible;
        //    var anim = new System.Windows.Media.Animation.DoubleAnimation(1, TimeSpan.FromSeconds(1));
        //    Waiter.BeginAnimation(UIElement.OpacityProperty, anim);

        //    ImageList.Items.Clear();
        //    ResultCanvas.Children.Clear();
        //    result.Clear();
        //    Mat readedImg = null;
        //    Emgu.CV.Util.VectorOfVectorOfPoint contours = new Emgu.CV.Util.VectorOfVectorOfPoint();
        //    UMat resultImg = new UMat();
        //    UMat filteredImg = new UMat();
        //    await Task.Run(() => {
        //        readedImg = CvInvoke.Imread(ImagePath, Emgu.CV.CvEnum.LoadImageType.Color);
        //        UMat hsv = new UMat();
        //        CvInvoke.CvtColor(readedImg, hsv, Emgu.CV.CvEnum.ColorConversion.Bgr2HsvFull);
        //        filteredImg = FilterMat(hsv);
        //        filteredImg.CopyTo(hsv);
        //        CvInvoke.FindContours(hsv, contours, null, Emgu.CV.CvEnum.RetrType.List, Emgu.CV.CvEnum.ChainApproxMethod.ChainApproxSimple);
        //        hsv.Dispose();
        //    });
        //    var res = contours.ToArrayOfArray().Select(x => new Emgu.CV.Util.VectorOfPoint(x));
        //    var n = 0;
        //    Emgu.CV.Util.VectorOfPoint polyStorage = null;
        //    Image BASE_IMG = new Image() { Source = ToWPFBitmap(readedImg.Bitmap) };
        //    CheckBox BASE_CHECKBOX = new CheckBox { Content = "Base", IsChecked = true };
        //    BASE_CHECKBOX.Checked += (ss, ee) => BASE_IMG.Visibility = Visibility.Visible;
        //    BASE_CHECKBOX.Unchecked += (ss, ee) => BASE_IMG.Visibility = Visibility.Hidden;
        //    ImageList.Items.Add(BASE_CHECKBOX);

        //    Image TEST_IMG = new Image() { Source = ToWPFBitmap(filteredImg.Bitmap) };
        //    CheckBox TEST_CHECKBOX = new CheckBox { Content = "Filtered", IsChecked = true };
        //    TEST_CHECKBOX.Checked += (ss, ee) => TEST_IMG.Visibility = Visibility.Visible;
        //    TEST_CHECKBOX.Unchecked += (ss, ee) => TEST_IMG.Visibility = Visibility.Hidden;
        //    ImageList.Items.Add(TEST_CHECKBOX);


        //    ResultCanvas.Children.Add(BASE_IMG);
        //    ResultCanvas.Children.Add(TEST_IMG);
        //    ResultCanvas.Height = readedImg.Height;
        //    ResultCanvas.Width = readedImg.Width;

        //    NormalCircleSize = Math.Min(readedImg.Width, readedImg.Height) / 128;
        //    NormalLineTickness = Math.Min(readedImg.Width, readedImg.Height) / 320;
        //    Offset = NormalCircleSize / 2;

        //    Canvas.SetTop(BASE_IMG, Offset);
        //    Canvas.SetLeft(BASE_IMG, Offset);
        //    Canvas.SetZIndex(BASE_IMG, 0);
        //    Canvas.SetTop(TEST_IMG, Offset);
        //    Canvas.SetLeft(TEST_IMG, Offset);
        //    Canvas.SetZIndex(TEST_IMG, 1);

        //    foreach (var parray in res.Where((x) => CvInvoke.ContourArea(x) > 100)) {
        //        polyStorage = new Emgu.CV.Util.VectorOfPoint();
        //        await Task.Run(() => CvInvoke.ApproxPolyDP(parray, polyStorage, 10, true));
        //        System.Drawing.Point? prevPoint = null;
        //        Stack<UIElement> drawers = new Stack<UIElement>(polyStorage.Size * 2);
        //        CheckBox chbox = new CheckBox() { Content = n.ToString(), IsChecked = true };
        //        var cacheArray = polyStorage.ToArray();
        //        foreach (var p in cacheArray) {
        //            Ellipse esp = new Ellipse() { Fill = Brushes.Red, Stroke = Brushes.Red, Width = NormalCircleSize, Height = NormalCircleSize };
        //            Canvas.SetZIndex(esp, 3);
        //            Canvas.SetTop(esp, p.Y);
        //            Canvas.SetLeft(esp, p.X);
        //            ResultCanvas.Children.Add(esp);
        //            drawers.Push(esp);
        //            if (prevPoint.HasValue) {
        //                Line l = new Line() { Stroke = Brushes.ForestGreen, StrokeThickness = NormalLineTickness, X1 = prevPoint.Value.X, X2 = p.X, Y1 = prevPoint.Value.Y, Y2 = p.Y };
        //                Canvas.SetZIndex(l, 2);
        //                Canvas.SetTop(l, Offset);
        //                Canvas.SetLeft(l, Offset);
        //                drawers.Push(l);
        //                ResultCanvas.Children.Add(l);
        //            }
        //            prevPoint = p;
        //        }

        //        //汚い
        //        Line _l = new Line() { Stroke = Brushes.ForestGreen, StrokeThickness = NormalLineTickness, X1 = prevPoint.Value.X, X2 = cacheArray[0].X, Y1 = prevPoint.Value.Y, Y2 = cacheArray[0].Y };
        //        Canvas.SetZIndex(_l, 2);
        //        Canvas.SetTop(_l, Offset);
        //        Canvas.SetLeft(_l, Offset);
        //        drawers.Push(_l);
        //        ResultCanvas.Children.Add(_l);

        //        chbox.Checked += (ee, ss) => { foreach (var uu in drawers) { uu.Visibility = Visibility.Visible; } };
        //        chbox.Unchecked += (ee, ss) => { foreach (var uu in drawers) { uu.Visibility = Visibility.Hidden; } };
        //        ImageList.Items.Add(chbox);
        //        n++;
        //    }
        //    ImageWidth = readedImg.Width;
        //    ImageHeight = readedImg.Height;

        //    polyStorage.Dispose();
        //    contours.Dispose();
        //    readedImg.Dispose();
        //    resultImg.Dispose();

        //    anim = new System.Windows.Media.Animation.DoubleAnimation(0, TimeSpan.FromSeconds(1));
        //    Waiter.BeginAnimation(UIElement.OpacityProperty, anim);
        //    Waiter.Visibility = Visibility.Hidden;
        //}
        //#endregion

        private async void UpdateContent_WithoutFilter(UMat FilteredImg, Mat ReadedImg) {
            Waiter.Opacity = 0;
            Waiter.Visibility = Visibility.Visible;
            var anim = new System.Windows.Media.Animation.DoubleAnimation(1, TimeSpan.FromSeconds(1));
            Waiter.BeginAnimation(UIElement.OpacityProperty, anim);

            ImageList.Items.Clear();
            ResultCanvas.Children.Clear();
            Emgu.CV.Util.VectorOfVectorOfPoint contours = new Emgu.CV.Util.VectorOfVectorOfPoint();
            UMat resultImg = new UMat();
            Image BASE_IMG = new Image() { Source = ToWPFBitmap(ReadedImg.Bitmap) };
            Thumbnail.Source = BASE_IMG.Source;
            CheckBox BASE_CHECKBOX = new CheckBox { Content = "Base", IsChecked = true };
            BASE_CHECKBOX.Checked += (ss, ee) => BASE_IMG.Visibility = Visibility.Visible;
            BASE_CHECKBOX.Unchecked += (ss, ee) => BASE_IMG.Visibility = Visibility.Hidden;
            ImageList.Items.Add(BASE_CHECKBOX);

            Image TEST_IMG = new Image() { Source = ToWPFBitmap(FilteredImg.Bitmap) };
            CheckBox TEST_CHECKBOX = new CheckBox { Content = "Filtered", IsChecked = true };
            TEST_CHECKBOX.Checked += (ss, ee) => TEST_IMG.Visibility = Visibility.Visible;
            TEST_CHECKBOX.Unchecked += (ss, ee) => TEST_IMG.Visibility = Visibility.Hidden;
            ImageList.Items.Add(TEST_CHECKBOX);

            await Task.Run(() => CvInvoke.FindContours(FilteredImg, contours, null, Emgu.CV.CvEnum.RetrType.List, Emgu.CV.CvEnum.ChainApproxMethod.ChainApproxSimple));
            var res = contours.ToArrayOfArray().Select(x => new Emgu.CV.Util.VectorOfPoint(x));
            var n = 0;
            int m = 0;
            ResultPolygonData rpd;
            Emgu.CV.Util.VectorOfPoint polyStorage = null;

            ResultCanvas.Children.Add(BASE_IMG);
            ResultCanvas.Children.Add(TEST_IMG);
            ResultCanvas.Height = ReadedImg.Height;
            ResultCanvas.Width = ReadedImg.Width;

            NormalCircleSize = Math.Min(ReadedImg.Width, ReadedImg.Height) / 256;
            NormalLineTickness = Math.Min(ReadedImg.Width, ReadedImg.Height) / 640;
            Offset = NormalCircleSize / 2;

            Canvas.SetTop(BASE_IMG, Offset);
            Canvas.SetLeft(BASE_IMG, Offset);
            Canvas.SetZIndex(BASE_IMG, 0);
            Canvas.SetTop(TEST_IMG, Offset);
            Canvas.SetLeft(TEST_IMG, Offset);
            Canvas.SetZIndex(TEST_IMG, 1);

            foreach (var parray in res.Where((x) => CvInvoke.ContourArea(x) > 100)) {
                int cacheIndex = ImageList.Items.Count;
                polyStorage = new Emgu.CV.Util.VectorOfPoint();
                await Task.Run(() => CvInvoke.ApproxPolyDP(parray, polyStorage, 10, true));
                System.Drawing.Point? prevPoint = null;
                CheckBox chbox = new CheckBox() { Content = n.ToString(), IsChecked = true };
                var cacheArray = polyStorage.ToArray();
                rpd = GetPolygonInformation(new RingBuffer<System.Drawing.Point>( cacheArray));
                Stack<UIElement> drawers = new Stack<UIElement>((cacheArray.Length + 1) * 4);
                foreach (var p in cacheArray) {
                    Ellipse esp = new Ellipse() { Fill = Brushes.Red, Stroke = Brushes.Red, Width = NormalCircleSize, Height = NormalCircleSize };
                    esp.ToolTip = rpd.Angles[m].ToString() + "°";
                    esp.MouseLeftButtonUp += (ss, ee) => ImageList.SelectedIndex = cacheIndex;
                    Canvas.SetZIndex(esp, 3);
                    Canvas.SetTop(esp, p.Y);
                    Canvas.SetLeft(esp, p.X);
                    TextBlock tb = new TextBlock() { FontSize= 24, Foreground = Brushes.DarkRed, Text=rpd.Angles[m].ToString("F") + "°" };
                    Canvas.SetZIndex(tb, 4);
                    Canvas.SetTop(tb, p.Y);
                    Canvas.SetLeft(tb, p.X);
                    drawers.Push(esp);
                    drawers.Push(tb);

                    ResultCanvas.Children.Add(tb);
                    ResultCanvas.Children.Add(esp);
                    if (prevPoint.HasValue) {
                        Line l = new Line() { Stroke = Brushes.ForestGreen, StrokeThickness = NormalLineTickness, X1 = prevPoint.Value.X, X2 = p.X, Y1 = prevPoint.Value.Y, Y2 = p.Y };
                        l.ToolTip = rpd.Lines[m - 1].ToString() + "px";
                        Canvas.SetZIndex(l, 2);
                        Canvas.SetTop(l, Offset);
                        Canvas.SetLeft(l, Offset);
                        TextBlock tb2 = new TextBlock() { FontSize = 24, Foreground = Brushes.DarkGreen, Text = rpd.Lines[m - 1].ToString("F") + "px" };
                        Canvas.SetZIndex(tb2, 4);
                        Canvas.SetTop(tb2, (p.Y + prevPoint.Value.Y) / 2);
                        Canvas.SetLeft(tb2, (p.X + prevPoint.Value.X) / 2);
                        ResultCanvas.Children.Add(tb2);
                        ResultCanvas.Children.Add(l);
                        drawers.Push(l);
                        drawers.Push(tb2);
                    }
                    prevPoint = p;
                    ++m;
                }

                //汚い
                Line _l = new Line() { Stroke = Brushes.ForestGreen, StrokeThickness = NormalLineTickness, X1 = prevPoint.Value.X, X2 = cacheArray[0].X, Y1 = prevPoint.Value.Y, Y2 = cacheArray[0].Y };
                _l.ToolTip = rpd.Lines[m - 2].ToString() + "px";
                Canvas.SetZIndex(_l, 2);
                Canvas.SetTop(_l, Offset);
                Canvas.SetLeft(_l, Offset);
                TextBlock tb3 = new TextBlock() { FontSize = 24, Foreground = Brushes.DarkGreen, Text = rpd.Lines[m - 1].ToString("F") + "px" };
                Canvas.SetZIndex(tb3, 4);
                Canvas.SetTop(tb3, (cacheArray[0].Y + prevPoint.Value.Y) / 2);
                Canvas.SetLeft(tb3, (cacheArray[0].X + prevPoint.Value.X) / 2);
                ResultCanvas.Children.Add(tb3);

                ResultCanvas.Children.Add(_l);
                drawers.Push(_l);
                drawers.Push(tb3);

                chbox.Checked += (ee, ss) => { foreach (var uu in drawers) { uu.Visibility = Visibility.Visible; } };
                chbox.Unchecked += (ee, ss) => { foreach (var uu in drawers) { uu.Visibility = Visibility.Hidden; } };
                ImageList.Items.Add(chbox);
                result.Add(rpd);
                n++;
                m = 0;
            }
            ImageWidth = ReadedImg.Width;
            ImageHeight = ReadedImg.Height;

            polyStorage.Dispose();
            contours.Dispose();
            ReadedImg.Dispose();
            resultImg.Dispose();

            anim = new System.Windows.Media.Animation.DoubleAnimation(0, TimeSpan.FromSeconds(1));
            Waiter.BeginAnimation(UIElement.OpacityProperty, anim);
            Waiter.Visibility = Visibility.Hidden;
        }

        private void Next_Button_Clicked(object sender, RoutedEventArgs e) => Next();

        private async void Next() {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine("Line,,Angle");
            foreach(var r in result) {
                if (r.Angles.Length == r.Lines.Length)
                    for(int n = 0; n < r.Angles.Length; ++n) {
                        sb.Append(r.Lines[n]);
                        sb.Append(",,");
                        sb.Append(r.Angles[n]);
                        sb.AppendLine();
                    }
            }
            System.IO.StreamWriter sw = new System.IO.StreamWriter("result.csv", false, Encoding.UTF8);
            await sw.WriteAsync(sb.ToString());
            sw.Close();
            sw.Dispose();
        }

        private void Shot_Button_Clicked(object sender, RoutedEventArgs e) => ResultImageShot(ResultCanvas);

        private void ResultImageShot(UIElement source) {
            try {
                double actualHeight = source.RenderSize.Height;
                double actualWidth = source.RenderSize.Width;

                double renderHeight = actualHeight / 2;
                double renderWidth = actualWidth / 2;

                RenderTargetBitmap renderTarget = new RenderTargetBitmap((int)renderWidth, (int)renderHeight, 96, 96, PixelFormats.Pbgra32);
                VisualBrush sourceBrush = new VisualBrush(source);

                DrawingVisual drawingVisual = new DrawingVisual();
                DrawingContext drawingContext = drawingVisual.RenderOpen();

                using (drawingContext) {
                    drawingContext.PushTransform(new ScaleTransform(0.5, 0.5));
                    drawingContext.DrawRectangle(sourceBrush, null, new Rect(new System.Windows.Point(0, 0), new System.Windows.Point(actualWidth, actualHeight)));
                }
                renderTarget.Render(drawingVisual);

                PngBitmapEncoder encoder = new PngBitmapEncoder();
                encoder.Frames.Add(BitmapFrame.Create(renderTarget));
                using (System.IO.FileStream stream = new System.IO.FileStream("ResultImage.png", System.IO.FileMode.Create, System.IO.FileAccess.Write)) {
                    encoder.Save(stream);
                }
            } catch {  }
        }

        //private UMat FilterMat(UMat mm) {
        //    UMat res = new UMat(mm.Rows, mm.Cols, Emgu.CV.CvEnum.DepthType.Cv8U, 1);
        //    byte[] cache_in = mm.Bytes;
        //    byte[] cache = res.Bytes;
        //    Parallel.For(0, cache.Length, (n) => {
        //        cache[n] = (cache_in[n * 3] < MAX_H && cache_in[n * 3 + 1] < MAX_S && cache_in[n * 3 + 1] > MIN_S && cache_in[n * 3 + 2] < MAX_V && cache_in[n * 3 + 2] > MIN_V) ?
        //            (byte)255 : (byte)0;
        //    });
        //    res.Bytes = cache;
        //    return res;
        //}

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

        private void ZoomSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e) {
            if (!IsInitialized)
                return;
            ResultCanvas.Height = ImageHeight * ZoomSlider.Value;
            ResultCanvas.Width = ImageWidth * ZoomSlider.Value;
            
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

        private static ResultPolygonData GetPolygonInformation(RingBuffer<System.Drawing.Point> Poly) {
            double[] Lines = new double[Poly.Length];
            double[] Points = new double[Poly.Length];
            Parallel.For(0, Poly.Length, (count) => {
                double x1 = Poly[count].X - Poly[count - 1].X;
                double y1 = Poly[count].Y - Poly[count - 1].Y;
                double x2 = Poly[count + 1].X - Poly[count].X;
                double y2 = Poly[count + 1].Y - Poly[count].Y;
                double theta1 = Math.Atan2(y1, x1) * 180 / 3.1415926358979323;
                double theta2 = Math.Atan2(y2, x2) * 180 / 3.1415926358979323;
                Points[count] = (180 + theta1 - theta2 + 360);
                while (Points[count] > 360) Points[count] -= 360;
                Points[count] = 360 - Points[count];
                Lines[count] = Math.Sqrt(Math.Abs(Math.Pow(Poly[count].X - Poly[count + 1].X, 2) + Math.Pow(Poly[count].Y - Poly[count + 1].Y, 2)));
            });
            return new ResultPolygonData(Points, Lines, Poly.Get_Array);
        }

    }

    public class RingBuffer<T> {
        T[] data;
        public T this[int i] {
            get {
                return i < 0 ? Math.Abs(i) % data.Length == 0 ? data[0] : data[data.Length - (Math.Abs(i) % data.Length)] : data[i % data.Length];
            }
            set{
                data[i % data.Length] = value;
            }
        }
        public  RingBuffer(int capacity) {
            data = new T[capacity];
        }
        public RingBuffer(T[] ary) {
            data = ary;
        }
        public int Length
        {
            get { return data.Length; }
        }
        public T[] Get_Array
        {
            get { return data; }
        }
        public static implicit operator T[](RingBuffer<T> val) => val.data;
        public static implicit operator RingBuffer<T>(T[] val) => new RingBuffer<T>(val);
    }

    public struct ResultPolygonData {
        public RingBuffer<double> Angles;
        public RingBuffer<double> Lines;
        public RingBuffer<Point> Points;
        public ResultPolygonData(double[] a, double[] l, Point[] p) {
            Angles = a;
            Lines = l;
            Points = p;
        }
    }

    public class ImageCheckboxPair {
        public CheckBox cb;
        public int index;
        
        public ImageCheckboxPair(CheckBox ch, int ind, Grid gg) {
            cb = ch;
            index = ind;
            ch.Checked += (ss, ee) => gg.Children[index + 1].Visibility = Visibility.Visible;
            ch.Unchecked += (ss, ee) => gg.Children[index + 1].Visibility = Visibility.Hidden;
        }
    }
}
