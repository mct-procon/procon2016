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
using System.Collections;
using Emgu.CV;

namespace PuzzleScanner.Pages {
    /// <summary>
    /// Main_ScannerWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class Main_ScannerWindow : Page {


        private const int MAX_S = 220;
        private const int MIN_S = 70;

        private const int MAX_V = 160;
        private const int MIN_V = 10;

        private const byte MAX_H = 50;

        string ImagePath = "";
        int ImageWidth = 0;
        int ImageHeight = 0;

        public Main_ScannerWindow() {
            InitializeComponent();
        }

        public Main_ScannerWindow(string imagepath) {
            ImagePath = imagepath;
            InitializeComponent();
            UpdateContent();
        }

        private async void UpdateContent() {
            Waiter.Opacity = 0;
            Waiter.Visibility = Visibility.Visible;
            var anim = new System.Windows.Media.Animation.DoubleAnimation(1, TimeSpan.FromSeconds(1));
            Waiter.BeginAnimation(UIElement.OpacityProperty, anim);

            ImageList.Items.Clear();
            ResultCanvas.Children.Clear();
            Mat readedImg = null;
            Emgu.CV.Util.VectorOfVectorOfPoint contours = new Emgu.CV.Util.VectorOfVectorOfPoint();
            UMat resultImg = new UMat();
            UMat filteredImg = new UMat();
            await Task.Run(() => {
                readedImg = CvInvoke.Imread(ImagePath, Emgu.CV.CvEnum.LoadImageType.Color);
                UMat hsv = new UMat();
                CvInvoke.CvtColor(readedImg, hsv, Emgu.CV.CvEnum.ColorConversion.Bgr2HsvFull);
                filteredImg = FilterMat(hsv);
                filteredImg.CopyTo(hsv);
                CvInvoke.FindContours(hsv, contours, null, Emgu.CV.CvEnum.RetrType.List, Emgu.CV.CvEnum.ChainApproxMethod.ChainApproxSimple);
                hsv.Dispose();
            });
            var res = contours.ToArrayOfArray().Select(x => new Emgu.CV.Util.VectorOfPoint(x));
            var n = 0;
            Emgu.CV.Util.VectorOfPoint polyStorage = null;
            Image BASE_IMG = new Image() { Source = ToWPFBitmap(readedImg.Bitmap) };
            CheckBox BASE_CHECKBOX = new CheckBox { Content = "Base", IsChecked = true };
            BASE_CHECKBOX.Checked += (ss, ee) => BASE_IMG.Visibility = Visibility.Visible;
            BASE_CHECKBOX.Unchecked += (ss, ee) => BASE_IMG.Visibility = Visibility.Hidden;
            ImageList.Items.Add(BASE_CHECKBOX);

            Image TEST_IMG = new Image() { Source = ToWPFBitmap(filteredImg.Bitmap) };
            CheckBox TEST_CHECKBOX = new CheckBox { Content = "Filtered", IsChecked = true };
            TEST_CHECKBOX.Checked += (ss, ee) => TEST_IMG.Visibility = Visibility.Visible;
            TEST_CHECKBOX.Unchecked += (ss, ee) => TEST_IMG.Visibility = Visibility.Hidden;
            ImageList.Items.Add(TEST_CHECKBOX);


            ResultCanvas.Children.Add(BASE_IMG);
            ResultCanvas.Children.Add(TEST_IMG);
            ResultCanvas.Height = readedImg.Height;
            ResultCanvas.Width = readedImg.Width;
            Canvas.SetTop(BASE_IMG, 2.5);
            Canvas.SetLeft(BASE_IMG, 2.5);
            Canvas.SetZIndex(BASE_IMG, 0);
            Canvas.SetTop(TEST_IMG, 2.5);
            Canvas.SetLeft(TEST_IMG, 2.5);
            Canvas.SetZIndex(TEST_IMG, 1);

            foreach (var parray in res.Where((x) => CvInvoke.ContourArea(x) > 100)) {
                polyStorage = new Emgu.CV.Util.VectorOfPoint();
                await Task.Run(() => CvInvoke.ApproxPolyDP(parray, polyStorage, 10, true));
                System.Drawing.Point? prevPoint = null;
                Stack<UIElement> drawers = new Stack<UIElement>(polyStorage.Size * 2);
                CheckBox chbox = new CheckBox() { Content = n.ToString(), IsChecked = true };
                var cacheArray = polyStorage.ToArray();
                foreach (var p in cacheArray) {
                    Ellipse esp = new Ellipse() { Fill = Brushes.Red, Stroke = Brushes.Red, Width = 5, Height = 5 };
                    Canvas.SetZIndex(esp, 3);
                    Canvas.SetTop(esp, p.Y);
                    Canvas.SetLeft(esp, p.X);
                    ResultCanvas.Children.Add(esp);
                    drawers.Push(esp);
                    if (prevPoint.HasValue) {
                        Line l = new Line() { Stroke = Brushes.ForestGreen, StrokeThickness = 1, X1 = prevPoint.Value.X, X2 = p.X, Y1 = prevPoint.Value.Y, Y2 = p.Y };
                        Canvas.SetZIndex(l, 2);
                        Canvas.SetTop(l, 2.5);
                        Canvas.SetLeft(l, 2.5);
                        drawers.Push(l);
                        ResultCanvas.Children.Add(l);
                    }
                    prevPoint = p;
                }

                //汚い
                Line _l = new Line() { Stroke = Brushes.ForestGreen, StrokeThickness = 1, X1 = prevPoint.Value.X, X2 = cacheArray[0].X, Y1 = prevPoint.Value.Y, Y2 = cacheArray[0].Y };
                Canvas.SetZIndex(_l, 2);
                Canvas.SetTop(_l, 2.5);
                Canvas.SetLeft(_l, 2.5);
                drawers.Push(_l);
                ResultCanvas.Children.Add(_l);

                chbox.Checked += (ee, ss) => { foreach (var uu in drawers) { uu.Visibility = Visibility.Visible; } };
                chbox.Unchecked += (ee, ss) => { foreach (var uu in drawers) { uu.Visibility = Visibility.Hidden; } };
                ImageList.Items.Add(chbox);
                n++;
            }
            ImageWidth = readedImg.Width;
            ImageHeight = readedImg.Height;

            polyStorage.Dispose();
            contours.Dispose();
            readedImg.Dispose();
            resultImg.Dispose();

            anim = new System.Windows.Media.Animation.DoubleAnimation(0, TimeSpan.FromSeconds(1));
            Waiter.BeginAnimation(UIElement.OpacityProperty, anim);
            Waiter.Visibility = Visibility.Hidden;
        }

        private UMat FilterMat(UMat mm) {
            UMat res = new UMat(mm.Rows, mm.Cols, Emgu.CV.CvEnum.DepthType.Cv8U, 1);
            byte[] cache_in = mm.Bytes;
            byte[] cache = res.Bytes;
            Parallel.For(0, cache.Length, (n) => {
                cache[n] = (cache_in[n * 3] < MAX_H && cache_in[n * 3 + 1] < MAX_S && cache_in[n * 3 + 1] > MIN_S && cache_in[n * 3 + 2] < MAX_V && cache_in[n * 3 + 2] > MIN_V) ?
                    (byte)255 : (byte)0;
            });
            res.Bytes = cache;
            return res;
        }

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
