﻿using System;
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
using Microsoft.Win32;
using PuzzleScanner.Controls;
using PuzzleScanner.Utils.ExtClasses;
using static PuzzleScanner.Utils.LINQExtensions;

namespace PuzzleScanner.Pages {
    /// <summary>
    /// NewOverall.xaml の相互作用ロジック
    /// </summary>
    public partial class NewOverall : Page {
        public NewOverall() {
            InitializeComponent();
        }

        private void AddButton_Click(object sender, RoutedEventArgs e) {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "All(*)|*.*";
            if (ofd.ShowDialog() == true) {
                Controls.ImageListItemControl cont = new Controls.ImageListItemControl(ofd.FileName, false);
                cont.DeleteButton.Click += (ss, ee) => { PagesItemList.Items.Remove(cont); cont.p.Unload_All(); };
                PagesItemList.Items.Add(cont);
            }
        }

        private void PagesItemList_SelectionChanged(object sender, SelectionChangedEventArgs e) {
            if (PagesItemList.SelectedIndex == -1)
                return;
            PageContent.Navigate(((Controls.ImageListItemControl)PagesItemList.Items[PagesItemList.SelectedIndex]).p.Shower);
        }

        private async void TextWriteOut_Click(object sender, RoutedEventArgs e) {
            var result_cache = PagesItemList.Items.Cast<ImageListItemControl>().ToList();
            IEnumerable<ResultPolygonData> result = MergeAllEnumerable(result_cache.Select((x, idx) => {
                if (idx == 0)
                    return x.GetResultData();
                return x.GetResultData().Select(y => {
                    for (int n = 0; n < y.Points.Length; ++n) {
                        y.Points.data[n].X += result_cache[idx - 1].p.Width;
                    }
                    return y;
                });
            }));
            Stack<ResultPolygonData> s = new Stack<ResultPolygonData>();
            Stack<ResultPolygonData> s_frm = new Stack<ResultPolygonData>();

            foreach(var v in result) {
                if (v.IsFrame)
                    s_frm.Push(v);
                else
                    s.Push(v);
            }

            StringBuilder sb = new StringBuilder();
            sb.AppendLine(s_frm.Count.ToString());
            foreach (var f in s_frm) {
                sb.AppendLine($"-1:{f.Points.Length.ToString()}");
                foreach (var pts in f.Points.Get_Array) {
                    sb.Append(pts.X);
                    sb.Append(" ");
                    sb.AppendLine(pts.Y.ToString());
                }
                sb.AppendLine();
            }
            sb.AppendLine(s.Count.ToString());
            foreach (var f in s) {
                sb.AppendLine($"{f.Tag}:{f.Points.Length.ToString()}");
                foreach (var pts in f.Points.Get_Array) {
                    sb.Append(pts.X);
                    sb.Append(" ");
                    sb.AppendLine(pts.Y.ToString());
                }
                sb.AppendLine();
            }
            System.IO.StreamWriter sw = new System.IO.StreamWriter("result.txt", false, Encoding.UTF8);
            await sw.WriteAsync(sb.ToString());
            sw.Close();
            sw.Dispose();
        }
    }
}