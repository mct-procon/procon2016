using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using SonnaPuzzle.Utils.ExtClasses;

namespace SonnaPuzzle.Controls {
    public class ImagePage : PageInterface {
        public ImagePage(string ImagePath) {
            this.ImagePath = ImagePath;
            State = PageState.Filter;
            f = new FilterPage(ImagePath);
            Shower = new Page();
            Shower.Content = f;
            f.ScanButton.Click += (ss, ee) => {
                p = new Controls.ScannerPage(f.ReadedImgData, f.FilteredImgData);
                Shower.Content = p;
                State = PageState.Scanner;
                StateChanged?.Invoke();
            };
        }

        /// <summary>
        /// Please check the state before calling this method.
        /// </summary>
        /// <returns></returns>
        public override List<ResultPolygonData> GetResultData() {
            if (p.MeasureScale == 1) return p.result;
            else {
                for (int n = 0; n < p.result.Count; ++n)
                    for (int m = 0; m < p.result[n].Points.Length; ++m)
                        p.result[n].Points[m] = new System.Drawing.Point((int)(p.result[n].Points[m].X * p.MeasureScale), (int)(p.result[n].Points[m].Y * p.MeasureScale));
                return p.result;
            }
        }

        public override void Unload_All() {
            f?.Mats_Unload();
            p?.Mats_Unload();
            Shower = null;
            f = null;
            p = null;
        }
    }
}
