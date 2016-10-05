using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using PuzzleScanner.Utils.ExtClasses;

namespace PuzzleScanner.Controls {
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
            return p.result;
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
