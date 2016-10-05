using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using PuzzleScanner.Utils.ExtClasses;

namespace PuzzleScanner.Controls {
    public class PageInterface {
        protected string ImagePath;

        protected FilterPage f;
        protected ScannerPage p;
        public Page Shower;

        public PageState State;
        public Action StateChanged;

        public virtual List<ResultPolygonData> GetResultData() { throw new NotImplementedException(); }
        public virtual void Unload_All() { throw new NotImplementedException(); }
    }

    public enum PageState {
        Filter, PanoramaProgress, Scanner
    }
}
