using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Emgu.CV;
using System.Windows.Controls.Primitives;
using System.Numerics;
using Point = System.Drawing.Point;
using Rectangle = System.Drawing.Rectangle;
using WpfRect = System.Windows.Shapes.Rectangle;
using static PuzzleScanner.Utils.ParallelExtensions;

namespace PuzzleScanner.Pages {
    /// <summary>
    /// Main_ScannerWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class Scanner : Page {

        private double NormalCircleSize = 0;
        private double NormalLineTickness = 0;
        private double Offset = 0;

        private double Scale = 1;
        private Mat InputImage;

        private UMat filtered;
        private Mat Readed;

        Stack<Polygon> UIPolygons = null;

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

        char[] cascades = new char[] {
            'あ','い','う','え','お','か','き','く','け','こ','さ','し','す','せ','そ','た','ち','つ','て','と','な','に','ぬ','ね','の','は','ひ','ふ','へ','も','や','ゆ','よ','ら','り','る','れ','ろ','ゐ','ゑ','を','ん','四','A','B','C','D','E','F','G','H','I'
        };

        public Scanner() {
            InitializeComponent();
        }

        //public Main_ScannerWindow(string imagepath) {
        //    ImagePath = imagepath;
        //    InitializeComponent();
        //    UpdateContent();
        //}

        public Scanner(UMat filteredImg, Mat ReadedImg) {
            InitializeComponent();
            filtered = filteredImg;
            Readed = ReadedImg;
        }

        /// <summary>
        /// 処理全容
        /// </summary>
        /// <param name="FilteredImg">フィルタしたイメージ</param>
        /// <param name="ReadedImg">原画像</param>
        private async void UpdateContent_WithoutFilter(UMat FilteredImg, Mat ReadedImg) {
            //GUI ANIMATION ホワイトさせるほど処理長くないけど…
            Waiter.Opacity = 0;
            Waiter.Visibility = Visibility.Visible;
            var anim = new System.Windows.Media.Animation.DoubleAnimation(1, TimeSpan.FromSeconds(1));
            Waiter.BeginAnimation(UIElement.OpacityProperty, anim);
            InputImage = ReadedImg.Clone();
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

            ResultCanvas.Children.Add(BASE_IMG);
            ResultCanvas.Children.Add(TEST_IMG);
            ResultCanvas.Height = ReadedImg.Height;
            ResultCanvas.Width = ReadedImg.Width;

            NormalCircleSize = Math.Min(ReadedImg.Width, ReadedImg.Height) / 256;
            NormalLineTickness = NormalCircleSize * 0.4;
            //readable:: NormalLineTickness = NormalChircleSize * 256 / 640;
            Offset = NormalCircleSize / 2;

            Canvas.SetTop(BASE_IMG, Offset);
            Canvas.SetLeft(BASE_IMG, Offset);
            Canvas.SetZIndex(BASE_IMG, 0);
            Canvas.SetTop(TEST_IMG, Offset);
            Canvas.SetLeft(TEST_IMG, Offset);
            Canvas.SetZIndex(TEST_IMG, 1);
            ImageWidth = ReadedImg.Width;
            ImageHeight = ReadedImg.Height;

            Queue<Emgu.CV.Util.VectorOfPoint> polyStorage = new Queue<Emgu.CV.Util.VectorOfPoint>(res.Count());
            await Task.Run(() => {
                Emgu.CV.Util.VectorOfPoint polyCache = null;
                int elp = (int)(0.003 * Math.Max(ImageWidth, ImageHeight));
                foreach (var parray in res.Where((x) => CvInvoke.ContourArea(x) > 1000)) {
                    polyCache = new Emgu.CV.Util.VectorOfPoint();
                    CvInvoke.ApproxPolyDP(parray, polyCache, elp, true);
                    polyStorage.Enqueue(polyCache);
                }
                result = polyStorage.ParallelSelect((x) => GetPolygonInformation(x.ToArray()), Environment.ProcessorCount).ToList();
                polyStorage.ParallelForAll(x => x.Dispose(), Environment.ProcessorCount);
            });
            Stack<UIElement> chardrawers = new Stack<UIElement>(result.Count * 7);
            UIPolygons = new Stack<Polygon>(result.Count);
            foreach (var poly in result) {
                int cacheIndex = ImageList.Items.Count;
                CustomControll.PolygonListedItem chbox = new CustomControll.PolygonListedItem(n.ToString());
                Stack<UIElement> drawers = new Stack<UIElement>((poly.Points.Length + 1) * 4);

                Polygon po = new Polygon() { Stroke = Brushes.Green, StrokeThickness = 4 };
                //po.MouseLeftButtonUp += (ss, ee) => ImageList.SelectedIndex = cacheIndex;
                po.MouseLeftButtonUp += (ss, ee) => {
                    if (chbox.FrameButton.IsChecked == true) {
                        chbox.FrameButton.IsChecked = false;
                        po.Stroke = Brushes.Green;
                    } else {
                        chbox.FrameButton.IsChecked = true;
                        po.Stroke = Brushes.Magenta;
                    }
                };
                po.MouseRightButtonUp += (ss, ee) => chbox.EnableButton.IsChecked = chbox.EnableButton.IsChecked == true ? false : true;
                Canvas.SetZIndex(po, 2);
                Canvas.SetTop(po, Offset);
                Canvas.SetLeft(po, Offset);
                po.Points = new System.Windows.Media.PointCollection(poly.Points.Get_Array.ParallelSelect((p) => new System.Windows.Point(p.X, p.Y), Environment.ProcessorCount));
                ResultCanvas.Children.Add(po);
                UIPolygons.Push(po);
                drawers.Push(po);
                for (int m = 0; m < poly.Angles.Length; ++m) {
                    bool esp_isDragging = false;
                    int esp_index = m;
                    System.Windows.Point esp_DragOffset = new System.Windows.Point();
                    Ellipse esp = new Ellipse() { Fill = Brushes.Red, Stroke = Brushes.Red, Width = NormalCircleSize, Height = NormalCircleSize };
                    //esp.MouseLeftButtonUp += (ss, ee) => ImageList.SelectedIndex = cacheIndex;
                    esp.MouseLeftButtonDown += (ss, ee) => {
                        esp_isDragging = true;
                        esp_DragOffset = ee.GetPosition(esp);
                        esp.CaptureMouse();
                    };
                    esp.MouseMove += (ss, ee) => {
                        if(esp_isDragging) {
                            System.Windows.Point pt = Mouse.GetPosition(ResultCanvas);
                            Canvas.SetLeft(esp, pt.X - esp_DragOffset.X);
                            Canvas.SetTop(esp, pt.Y - esp_DragOffset.Y);
                            po.Points[esp_index] = new System.Windows.Point(pt.X - esp_DragOffset.X, pt.Y - esp_DragOffset.Y);
                        }
                    };
                    Canvas.SetZIndex(esp, 4);
                    Canvas.SetTop(esp, poly.Points[m].Y);
                    Canvas.SetLeft(esp, poly.Points[m].X);
                    TextBlock tb = new TextBlock() { FontSize = 24, Foreground = Brushes.DarkRed, Background = new SolidColorBrush(Color.FromArgb(125, 255, 255, 255)), Text = poly.Angles[m].ToString("F") + "°" };
                    chardrawers.Push(tb);
                    Canvas.SetZIndex(tb, 3);
                    Canvas.SetTop(tb, poly.Points[m].Y);
                    Canvas.SetLeft(tb, poly.Points[m].X);
                    drawers.Push(esp);
                    drawers.Push(tb);

                    ResultCanvas.Children.Add(tb);
                    ResultCanvas.Children.Add(esp);
                    //    Line l = new Line() { Stroke = Brushes.ForestGreen, StrokeThickness = NormalLineTickness, X1 = poly.Points[m-1].X, X2 = poly.Points[m].X, Y1 = poly.Points[m-1].Y, Y2 = poly.Points[m].Y };
                    //    l.MouseLeftButtonUp += (ss, ee) => chbox.EnableButton.IsChecked = chbox.EnableButton.IsChecked == true ? false : true;
                    //    l.MouseRightButtonUp += (ss, ee) => chbox.FrameButton.IsChecked = chbox.FrameButton.IsChecked == true ? false : true;
                    //    Canvas.SetZIndex(l, 2);
                    //    Canvas.SetTop(l, Offset);
                    //    Canvas.SetLeft(l, Offset);
                    TextBlock tb2 = new TextBlock() { FontSize = 24, Foreground = Brushes.DarkGreen,Background = new SolidColorBrush(Color.FromArgb(125,255,255,255)), Text = poly.Lines[m - 1].ToString("F") + "px" };
                    chardrawers.Push(tb2);
                    Canvas.SetZIndex(tb2, 3);
                    Canvas.SetTop(tb2, (poly.Points[m].Y + poly.Points[m-1].Y) / 2);
                    Canvas.SetLeft(tb2, (poly.Points[m].X + poly.Points[m-1].X) / 2);
                    ResultCanvas.Children.Add(tb2);
                    //    ResultCanvas.Children.Add(l);
                    //    drawers.Push(l);
                    drawers.Push(tb2);

                    esp.MouseLeftButtonUp += (ss, ee) => {
                        esp.ReleaseMouseCapture();
                        esp_isDragging = false;
                        System.Windows.Point pt = Mouse.GetPosition(ResultCanvas);
                        Canvas.SetLeft(esp, pt.X - Math.Floor(esp_DragOffset.X));
                        Canvas.SetTop(esp, pt.Y - Math.Floor(esp_DragOffset.Y));
                        poly.Points[esp_index] = new Point(poly.Points[esp_index].X - (int)Math.Floor(esp_DragOffset.X), poly.Points[esp_index].Y - (int)Math.Floor(esp_DragOffset.Y));
                        po.Points[esp_index] = new System.Windows.Point(pt.X - Math.Floor(esp_DragOffset.X), pt.Y - Math.Floor(esp_DragOffset.Y));
                    };
                }

                chbox.EnableButton.Checked += (ee, ss) => { foreach (var uu in drawers) { uu.Visibility = Visibility.Visible; } };
                chbox.EnableButton.Unchecked += (ee, ss) => { foreach (var uu in drawers) { uu.Visibility = Visibility.Hidden; } };
                ImageList.Items.Add(chbox);
                n++;
            }
            ShowSizesButton.Checked += (ee, ss) => { foreach (var uu in chardrawers) { uu.Visibility = Visibility.Visible; } };
            ShowSizesButton.Unchecked += (ee, ss) => { foreach (var uu in chardrawers) { uu.Visibility = Visibility.Hidden; } };
            contours.Dispose();
            ReadedImg.Dispose();
            resultImg.Dispose();

            anim = new System.Windows.Media.Animation.DoubleAnimation(0, TimeSpan.FromSeconds(1));
            Waiter.BeginAnimation(UIElement.OpacityProperty, anim);
            Waiter.Visibility = Visibility.Hidden;
        }

        /// <summary>
        /// 未完成．タグ付けを行う．
        /// </summary>
        private async void DetectTags() {
            CascadeClassifier cascade;
            Rectangle[] detects;
            for(sbyte n = 0; n < cascades.Length; ++n) {
                cascade = new CascadeClassifier("なんだろね～");
                detects = await Task.Run( () => cascade.DetectMultiScale(InputImage));
                foreach(Rectangle r in detects) {
                    WpfRect wr = new WpfRect() { Stroke = Brushes.Blue, StrokeThickness = 1.0, Width = r.Width, Height = r.Height };
                    Canvas.SetLeft(wr, r.X);
                    Canvas.SetTop(wr, r.Y);
                    ResultCanvas.Children.Add(wr);
                    for(int m = 0; m < result.Count; ++m)
                        if(judgeInclusion(result[m], r.Location)) {
                            result[m] = new ResultPolygonData(result[m].Angles, result[m].Lines, result[m].Points) { Tag = n };
                            break;
                        }
                }
            }
        }

        private bool judgeInclusion(ResultPolygonData d, Point p) {
            double deg = 0;
            int px = p.X;
            int py = p.Y;

            for (int index = 0; index < d.Points.Length; index++) {
                int p2x = d.Points[index].X;
                int p2y = d.Points[index].Y;
                int p3x;
                int p3y;
                if (index < d.Points.Length - 1) {
                    p3x = d.Points[index + 1].X;
                    p3y = d.Points[index + 1].Y;
                } else {
                    p3x = d.Points[0].X;
                    p3y = d.Points[0].Y;
                }

                var ax = p2x - px;
                var ay = p2y - py;
                var bx = p3x - px;
                var by = p3y - py;

                var cos = (ax * bx + ay * by) / (Math.Sqrt(ax * ax + ay * ay) * Math.Sqrt(bx * bx + by * by));
                deg += Math.Acos(cos) * 180 / Math.PI;
            }

            if (Math.Round(deg) == 360) {
                return true;
            } else {
                return false;
            }
        }

        private void Next_Button_Clicked(object sender, RoutedEventArgs e) => Next();

        /// <summary>
        /// データを渡すメソッド
        /// </summary>
        private async void Next() {
            int cache = result.Count;
            Queue<ResultPolygonData> q = new Queue<ResultPolygonData>();
            Queue<ResultPolygonData> q_frm = new Queue<ResultPolygonData>();
            CustomControll.PolygonListedItem pli = null;
            for (int co = 0; co < cache; ++co) {
                pli = ((CustomControll.PolygonListedItem)(ImageList.Items[co + 2]));
                if (pli.IsChosen == true)
                    if (pli.IsFrame == true)
                        q_frm.Enqueue(result[co]);
                    else
                        q.Enqueue(result[co]);
            }

            StringBuilder sb = new StringBuilder();
            sb.AppendLine(q_frm.Count.ToString());
            foreach(var f in q_frm) {
                sb.AppendLine($"-1:{f.Points.Length.ToString()}");
                foreach(var pts in f.Points.Get_Array) {
                    sb.Append(pts.X);
                    sb.Append(" ");
                    sb.AppendLine(pts.Y.ToString());
                }
                sb.AppendLine();
            }
            sb.AppendLine(q.Count.ToString());
            foreach (var f in q) {
                sb.AppendLine($"{f.Tag}:{f.Points.Length.ToString()}");
                foreach (var pts in f.Points.Get_Array) {
                    sb.Append(pts.X);
                    sb.Append(" ");
                    sb.AppendLine(pts.Y.ToString());
                }
                sb.AppendLine();
            }
            System.IO.StreamWriter sw = new System.IO.StreamWriter("result.txt", false, Encoding.UTF8);
            await sw.WriteAsync(sb.ToString());
            sw.Close();
            sw.Dispose();
        }

        private void Shot_Button_Clicked(object sender, RoutedEventArgs e) => ResultImageShot(ResultCanvas);

        /// <summary>
        /// 視覚化したデータをPNGファイルに50%のサイズで保存する．サイズは表示倍率も影響する．
        /// </summary>
        /// <param name="source">保存するUIエレメント</param>
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

        /// <summary>
        /// ポリゴン(頂点の集合)に対して角度，長さなどの詳細な情報を取得します．
        /// </summary>
        /// <param name="Poly">ポリゴン(頂点の集合)</param>
        /// <returns>詳細な情報のあるポリゴン</returns>
        private static ResultPolygonData GetPolygonInformation(RingBuffer<System.Drawing.Point> Poly) {
            //double[] Lines = new double[Poly.Length];
            //double[] Points = new double[Poly.Length];
            switch(Vector<double>.Count) {
                case 1: case 0:
                    return GetPolygonInformation_NoSIMD(Poly);
                case 2: case 3:
                    return GetPolygonInformation_SSE_AVX(Poly);
                case 4:
                    return GetPolygonInformation_AVX2(Poly);
                default:
                    throw new NotImplementedException();
            }
            /*
             * Old Code;
             */
//            Parallel.For(0, Poly.Length, (count) => {
//#if NotSIMD
//                // Not SIMD Using Codes
//                double x1 = Poly[count].X - Poly[count - 1].X;
//                double y1 = Poly[count].Y - Poly[count - 1].Y;
//                double x2 = Poly[count + 1].X - Poly[count].X;
//                double y2 = Poly[count + 1].Y - Poly[count].Y;
//                double theta1 = Math.Atan2(y1, x1) * 180 / 3.1415926358979323;
//                double theta2 = Math.Atan2(y2, x2) * 180 / 3.1415926358979323;
//                Points[count] = (180 + theta1 - theta2 + 360);
//#else
//                Vector<double> v1 = new Vector<double>(new double[]{ Poly[count].X, Poly[count].Y , Poly[count + 1].X, Poly[count + 1].Y });
//                Vector<double> v2 = new Vector<double>(new double[] { Poly[count - 1].X, Poly[count - 1].Y, Poly[count].X, Poly[count].Y });
//                v1 -= v2;
//                Vector<double> v3 = new Vector<double>(new double[] { Math.Atan2(v1[1], v1[0]), Math.Atan2(v1[3], v1[2]),0,0});
//                v3 *= new Vector<double>(new double[] { 180 / 3.1415926358979323, 180 / 3.1415926358979323,0,0 });
//                Points[count] = 180 + v3[0] - v3[1] + 360;
//#endif
//                while (Points[count] > 360) Points[count] -= 360;
//                Points[count] = 360 - Points[count];
//                Lines[count] = Math.Sqrt(Math.Abs(Math.Pow(Poly[count].X - Poly[count + 1].X, 2) + Math.Pow(Poly[count].Y - Poly[count + 1].Y, 2)));
//            });
//            return new ResultPolygonData(Points, Lines, Poly.Get_Array);
        }

        private static ResultPolygonData GetPolygonInformation_NoSIMD(RingBuffer<Point> Poly) {
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

        private static ResultPolygonData GetPolygonInformation_SSE_AVX(RingBuffer<Point> Poly) {
            double[] Lines = new double[Poly.Length];
            double[] Points = new double[Poly.Length];
            Parallel.For(0, Poly.Length, (count) => {
                Vector<double> v1 = new Vector<double>(new double[] { Poly[count].X, Poly[count].Y });
                Vector<double> v1_2 = new Vector<double>(new double[] { Poly[count + 1].X, Poly[count + 1].Y });
                Vector<double> v2 = new Vector<double>(new double[] { Poly[count - 1].X, Poly[count - 1].Y });
                Vector<double> v2_2 = new Vector<double>(new double[] { Poly[count].X, Poly[count].Y });
                v1 -= v2;
                v1_2 -= v2_2;
                Vector<double> v3 = new Vector<double>(new double[] { Math.Atan2(v1[1], v1[0]), Math.Atan2(v1_2[1], v1_2[0]) });
                v3 *= new Vector<double>(new double[] { 180 / 3.1415926358979323, 180 / 3.1415926358979323 });
                Points[count] = 180 + v3[0] - v3[1] + 360;
                while (Points[count] > 360) Points[count] -= 360;
                Points[count] = 360 - Points[count];
                Lines[count] = Math.Sqrt(Math.Abs(Math.Pow(Poly[count].X - Poly[count + 1].X, 2) + Math.Pow(Poly[count].Y - Poly[count + 1].Y, 2)));
            });
            return new ResultPolygonData(Points, Lines, Poly.Get_Array);
        }

        private static ResultPolygonData GetPolygonInformation_AVX2(RingBuffer<Point> Poly) {
            double[] Lines = new double[Poly.Length];
            double[] Points = new double[Poly.Length];
            Parallel.For(0, Poly.Length, (count) => {
                Vector<double> v1 = new Vector<double>(new double[] { Poly[count].X, Poly[count].Y, Poly[count + 1].X, Poly[count + 1].Y });
                Vector<double> v2 = new Vector<double>(new double[] { Poly[count - 1].X, Poly[count - 1].Y, Poly[count].X, Poly[count].Y });
                v1 -= v2;
                Vector<double> v3 = new Vector<double>(new double[] { Math.Atan2(v1[1], v1[0]), Math.Atan2(v1[3], v1[2]), 0, 0 });
                v3 *= new Vector<double>(new double[] { 180 / 3.1415926358979323, 180 / 3.1415926358979323, 0, 0 });
                Points[count] = 180 + v3[0] - v3[1] + 360;
                while (Points[count] > 360) Points[count] -= 360;
                Points[count] = 360 - Points[count];
                Lines[count] = Math.Sqrt(Math.Abs(Math.Pow(Poly[count].X - Poly[count + 1].X, 2) + Math.Pow(Poly[count].Y - Poly[count + 1].Y, 2)));
            });
            return new ResultPolygonData(Points, Lines, Poly.Get_Array);
        }

        private static Tuple<double, double, double> UpdatePolygonInformation (Point Prev, Point Current, Point Next) {
            Vector<double> v1 = new Vector<double>(new double[] { Current.X, Current.Y, Next.X, Next.Y });
            Vector<double> v2 = new Vector<double>(new double[] { Prev.X, Prev.Y, Current.X, Current.Y });
            v1 -= v2;
            Vector<double> v3 = new Vector<double>(new double[] { Math.Atan2(v1[1], v1[0]), Math.Atan2(v1[3], v1[2]), 0, 0 });
            v3 *= new Vector<double>(new double[] { 180 / 3.1415926358979323, 180 / 3.1415926358979323, 0, 0 });
            double second = 180 + v3[0] - v3[1] + 360;
            while (second > 360) second -= 360;
            second = 360 - second;
            double first = Math.Sqrt(Math.Abs(Math.Pow(Prev.X - Current.X, 2) + Math.Pow(Prev.Y - Current.Y, 2)));
            double third = Math.Sqrt(Math.Abs(Math.Pow(Current.X - Next.X, 2) + Math.Pow(Current.Y - Next.Y, 2)));
            return Tuple.Create(first, second, third);
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

            ResultCanvas.Height = ImageHeight * Scale;
            ResultCanvas.Width = ImageWidth * Scale;

            CanvasScale.ScaleX = Scale;
            CanvasScale.ScaleY = Scale;

            Scroller.ScrollToHorizontalOffset((centerX / prev * Scale) - (Scroller.ViewportWidth / 2));
            Scroller.ScrollToVerticalOffset((centerY / prev * Scale) - (Scroller.ViewportHeight / 2));

            if (UIPolygons != null) {
                double strokeThickness = 4 / Scale;
                foreach (Polygon po in UIPolygons) {
                    po.StrokeThickness = strokeThickness;
                }
            }
        }

        private void Page_Loaded(object sender, RoutedEventArgs e) {
            UpdateContent_WithoutFilter(filtered, Readed);
        }
    }

    /// <summary>
    /// リングバッファ(輪配列)のクラス
    /// </summary>
    /// <typeparam name="T">格納する型</typeparam>
    public struct RingBuffer<T> {
        T[] data;
        public T this[int i] {
            get {
                return i == -1 ? data[Length - 1] :( i < 0 ? Math.Abs(i) % data.Length == 0 ? data[0] : data[data.Length - (Math.Abs(i) % data.Length)] : data[i % data.Length]);
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

    public class ResultPolygonData {
        public sbyte Tag;
        public RingBuffer<double> Angles;
        public RingBuffer<double> Lines;
        public RingBuffer<Point> Points;
        public ResultPolygonData(double[] a, double[] l, Point[] p) {
            Tag = -1;
            Angles = a;
            Lines = l;
            Points = p;
        }
    }

}
