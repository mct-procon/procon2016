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

namespace PuzzleScanner.CustomControll {
    /// <summary>
    /// PolygonListedItem.xaml の相互作用ロジック
    /// </summary>
    public partial class PolygonListedItem : Grid {
        public PolygonListedItem() {
            InitializeComponent();
        }

        public PolygonListedItem(string description) {
            InitializeComponent();
            DescriptionLabel.Text = description;
        }

        public bool IsChosen {
            get { return EnableButton.IsChecked == true; }
            set { EnableButton.IsChecked = value; }
        }
        
        public bool IsFrame {
            get { return FrameButton.IsChecked == true; }
            set { FrameButton.IsChecked = value; }
        }
    }
}
