using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using PuzzleScanner.Utils.ExtClasses;

namespace PuzzleScanner.Controls {
    public abstract class PageInterface {
        protected string ImagePath;

        protected FilterPage f;
        protected ScannerPage p;
        public Page Shower;

        public PageState State;
        public Action StateChanged;

        public abstract int Width { get; }
        public abstract List<ResultPolygonData> GetResultData();
        public abstract void Unload_All();
    }

    public enum PageState {
        Filter, PanoramaProgress, Scanner
    }
}
