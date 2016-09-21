using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;
using System.Windows;
using System.IO;

namespace SolverVisualizer {
    internal class DataReader {
        internal List<Polygon> Polygons = new List<Polygon>();
        internal Point MaxSize = new Point(0,0);

        internal async Task LoadAsync(string path) {
            string line;
            double[] ns;
            int polys;
            int[] poly_info;
            StreamReader sr = new StreamReader(path, Encoding.UTF8);
            line = await sr.ReadLineAsync();
            polys = int.Parse(line.Trim());
            for (int n = 0; n < polys; ++n) {
                line = await sr.ReadLineAsync();
                poly_info = new int[1];
                poly_info[0] = int.Parse(line);
                Polygon p = new SolverVisualizer.Polygon();
                p.FrameColor = Colors.Red;
                p.Points = new PointCollection(poly_info[0]);
                for (int m = 0; m < poly_info[1]; ++m) {
                    line = await sr.ReadLineAsync();
                    ns = line.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries).Select(x => double.Parse(x)).ToArray();
                    p.Points.Add(new Point(ns[0], ns[1]));
                    if (MaxSize.X < ns[0])
                        MaxSize.X = ns[0];
                    if (MaxSize.Y < ns[1])
                        MaxSize.Y = ns[1];
                }
                Polygons.Add(p);
                await sr.ReadLineAsync();
            }

            line = await sr.ReadLineAsync();
            polys = int.Parse(line.Trim());
            for (int n = 0; n < polys; ++n) {
                line = await sr.ReadLineAsync();
                poly_info = line.Split(new char[] { ':' }, StringSplitOptions.RemoveEmptyEntries).Select(x => int.Parse(x)).ToArray();
                Polygon p = new SolverVisualizer.Polygon();
                p.Points = new PointCollection(poly_info[1]);
                p.Tag = (sbyte)poly_info[0];
                for (int m = 0; m < poly_info[1]; ++m) {
                    line = await sr.ReadLineAsync();
                    ns = line.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries).Select(x => double.Parse(x)).ToArray();
                    p.Points.Add(new Point(ns[0], ns[1]));
                    if (MaxSize.X < ns[0])
                        MaxSize.X = ns[0];
                    if (MaxSize.Y < ns[1])
                        MaxSize.Y = ns[1];
                }
                Polygons.Add(p);
                await sr.ReadLineAsync();
            }
        }

        //internal void Load(string path) {
        //    string line;
        //    double[] ns_x;
        //    double[] ns_y;
        //    int polys;
        //    int cache_capacity;
        //    StreamReader sr = new StreamReader(path, Encoding.UTF8);
        //    line = sr.ReadLine();
        //    polys = int.Parse(line.Trim());
        //    for (int n = 0; n < polys; ++n) {
        //        line = sr.ReadLine();
        //        cache_capacity = int.Parse(line);
        //        line = sr.ReadLine();
        //        ns_x = line.Split(new char[] {' '},StringSplitOptions.RemoveEmptyEntries).Select(x => double.Parse(x)).ToArray();
        //        line = sr.ReadLine();
        //        ns_y = line.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries).Select(y => double.Parse(y)).ToArray();
        //        Polygon p = new SolverVisualizer.Polygon();
        //        p.FrameColor = Colors.Red;
        //        for (int m = 0; m < cache_capacity; ++m)
        //            p.Points.Add(new Point(ns_x[m], ns_y[m]));
        //        Polygons.Add(p);
        //        sr.ReadLine();
        //    }

        //    line = sr.ReadLine();
        //    polys = int.Parse(line.Trim());
        //    for (int n = 0; n < polys; ++n) {
        //        line = sr.ReadLine();
        //        cache_capacity = int.Parse(line);
        //        line = sr.ReadLine();
        //        ns_x = line.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries).Select(x => double.Parse(x)).ToArray();
        //        line = sr.ReadLine();
        //        ns_y = line.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries).Select(y => double.Parse(y)).ToArray();
        //        Polygon p = new SolverVisualizer.Polygon();
        //        for (int m = 0; m < cache_capacity; ++m)
        //            p.Points.Add(new Point(ns_x[m], ns_y[m]));
        //        Polygons.Add(p);
        //        sr.ReadLine();
        //    }
        //}


    }

    internal class Polygon {
        internal Color FrameColor = Colors.Black;
        internal PointCollection Points = new PointCollection();
        internal sbyte Tag = -1;
    }

}
