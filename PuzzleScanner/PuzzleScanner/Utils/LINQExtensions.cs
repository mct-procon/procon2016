using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PuzzleScanner.Utils {
    public static class LINQExtensions {
        public static IEnumerable<T> MergeAllList<T> ( List<List<T>> l) {
            foreach(var a in l) {
                foreach(var b in a) {
                    yield return b;
                }
            }
        }

        public static IEnumerable<T> MergeAllEnumerable<T>(IEnumerable<IEnumerable<T>> l) {
            foreach (var a in l) {
                foreach (var b in a) {
                    yield return b;
                }
            }
        }
    }
}
