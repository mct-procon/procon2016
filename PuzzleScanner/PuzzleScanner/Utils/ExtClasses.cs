using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;

namespace PuzzleScanner.Utils.ExtClasses {
        /// <summary>
        /// リングバッファ(輪配列)のクラス
        /// </summary>
        /// <typeparam name="T">格納する型</typeparam>
        public class RingBuffer<T> {
            T[] data;
            public T this[int i] {
                get {
                    return i == -1 ? data[Length - 1] : (i < 0 ? Math.Abs(i) % data.Length == 0 ? data[0] : data[data.Length - (Math.Abs(i) % data.Length)] : data[i % data.Length]);
                }
                set {
                    data[i % data.Length] = value;
                }
            }
            public RingBuffer(int capacity) {
                data = new T[capacity];
            }
            public RingBuffer(T[] ary) {
                data = ary;
            }
            public int Length {
                get { return data.Length; }
            }
            public T[] Get_Array {
                get { return data; }
            }
            public static implicit operator T[] (RingBuffer<T> val) => val.data;
            public static implicit operator RingBuffer<T>(T[] val) => new RingBuffer<T>(val);
        }

    public class ResultPolygonData {
        public sbyte Tag;
        public RingBuffer<double> Angles;
        public RingBuffer<double> Lines;
        public RingBuffer<Point> Points;
        public bool IsFrame = false;
        public ResultPolygonData(double[] a, double[] l, Point[] p) {
            Tag = -1;
            Angles = a;
            Lines = l;
            Points = p;
        }
    }
}
