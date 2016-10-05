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
using PuzzleScanner.Utils.ExtClasses;

namespace PuzzleScanner.Controls {
    /// <summary>
    /// ImageListItemControl.xaml の相互作用ロジック
    /// </summary>
    public partial class ImageListItemControl : Grid {
        public PageInterface p;

        public ImageListItemControl() {
            InitializeComponent();
        }

        public ImageListItemControl(string imgPath, bool IsPanorama) {
            InitializeComponent();
            if(IsPanorama) {

            } else {
                p = new Controls.ImagePage(imgPath);
                ItemLabel.Text = System.IO.Path.GetFileNameWithoutExtension(imgPath);
                p.StateChanged = () => 
                    StatusCircle.Fill = p.State == PageState.Filter ? Brushes.Red : (p.State == PageState.Scanner ? Brushes.Green : Brushes.Blue);
            }
        }

        public List<ResultPolygonData>  GetResultData() {
            return p.GetResultData();
        }

    }
}
