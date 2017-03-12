using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace PuzzleScanner.Controls {
    //public class PanoramaPage : PageInterface {
    //    protected List<string> Pathes;

    //    public PanoramaPage(params string[] pathes) {
    //        this.Pathes = pathes.ToList();
    //        Shower = new Page();
    //    }

    //    public PanoramaPage(string Imagepath) {
    //        this.Pathes.Add(ImagePath);
    //        f = new FilterPage(ImagePath);
    //        Shower.Content = f;
    //        f.ScanButton.Click += async(ss, ee) => {
    //            f.IsEnabled = false;
    //            Emgu.CV.Mat[] imgs = await Task.Run( () =>  Pathes.Select(x => Emgu.CV.CvInvoke.Imread(x, Emgu.CV.CvEnum.LoadImageType.Color)).ToArray());
    //            Emgu.CV.Mat result = await PanoramaCreater.CreatePanorama(imgs);
    //            byte _h_min = (byte)(f.H_min.Value * 255 / 360);
    //            byte _h_max = (byte)(f.H_max.Value * 255 / 360);
    //            byte _s_min = (byte)(f.S_min.Value * 255 / 100);
    //            byte _s_max = (byte)(f.S_max.Value * 255 / 100);
    //            byte _v_min = (byte)(f.V_min.Value * 255 / 100);
    //            byte _v_max = (byte)(f.V_max.Value * 255 / 100);
    //            Emgu.CV.UMat filter_result = await Task.Run(() => {
    //                for (int i = 0; i < imgs.Length; ++i)
    //                    imgs[i].Dispose();
    //                Emgu.CV.UMat filter = new Emgu.CV.UMat();
    //                Emgu.CV.UMat _result = result.ToUMat(Emgu.CV.CvEnum.AccessType.Read);
    //                FilterPage.FilterMat(ref _result, ref filter, _h_min, _h_max, _s_min, _s_max, _v_min, _v_max);
    //                return filter;
    //            });
    //            p = new ScannerPage(result, filter_result);
    //            Shower.Content = p;
    //            f.IsEnabled = true;
    //        };
    //    }

    //}
}
