using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using Emgu.CV;

namespace PuzzleScanner.Pages {
    public class ScannerWithPanorama : Scanner {
        public ScannerWithPanorama() {
            
        }

        protected override void Page_Loaded(object sender, RoutedEventArgs e) {
            CreatePanorama();
            base.Page_Loaded(sender, e);
        }

        protected async void CreatePanorama() {
            UMat Hsvmat = new UMat();
            await Task.Run(() => {
                Emgu.CV.Util.VectorOfMat ScannerReadedImages = new Emgu.CV.Util.VectorOfMat(App.ScannerImagePathes.Select(x => CvInvoke.Imread(x, Emgu.CV.CvEnum.LoadImageType.Color)).ToArray());
                using (Emgu.CV.Stitching.Stitcher stic = new Emgu.CV.Stitching.Stitcher(true)) {
                    stic.Stitch(ScannerReadedImages, Readed);
                }
                CvInvoke.CvtColor(Readed, Hsvmat, Emgu.CV.CvEnum.ColorConversion.Bgr2HsvFull);
            });
            Filter.FilterMat(ref Hsvmat, ref filtered,App.Scanner_Filter[0], App.Scanner_Filter[1], App.Scanner_Filter[2], App.Scanner_Filter[3], App.Scanner_Filter[4], App.Scanner_Filter[5]);
        }

    }
}
