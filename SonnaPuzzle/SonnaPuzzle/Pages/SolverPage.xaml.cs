using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Runtime.InteropServices;
using Microsoft.Wpf.Interop.DirectX;
using System.Windows.Interop;

namespace SonnaPuzzle.Pages {
    /// <summary>
    /// SolverPage.xaml の相互作用ロジック
    /// </summary>
    public partial class SolverPage : Page {
        D3DImage D3DOutputer = null;
        int ticks = 0;

        public SolverPage() {
            D3DOutputer = new D3DImage();
            D3DOutputer.IsFrontBufferAvailableChanged += D3DOutputer_IsFrontBufferAvailableChanged;
            D3DOutputer.Lock();
            //D3DOutputer.SetPixelSize(1000, 800);
            //D3DOutputer.OnRender += (ee, b) => SolverLoop();
            HwndSource hwnd = new HwndSource(0, 0, 0, 0, 0, "SolverDxLib", IntPtr.Zero);
            Init_PuzzleSolver(hwnd.Handle);
            D3DOutputer.SetBackBuffer(System.Windows.Interop.D3DResourceType.IDirect3DSurface9, GetSurfacePointer());
            App.IsSoolverInitied = true;
            D3DOutputer.Unlock();
            CompositionTarget.Rendering += CompositionTarget_Rendering;
            InitializeComponent();
            out_img.Source = D3DOutputer;
        }

        private void CompositionTarget_Rendering(Object sender, EventArgs e) {
            if(D3DOutputer.IsFrontBufferAvailable && App.IsSoolverInitied) {
                D3DOutputer.Lock();
                SolverLoop();
                D3DOutputer.Unlock();
            }
        }

        private void D3DOutputer_IsFrontBufferAvailableChanged(Object sender, DependencyPropertyChangedEventArgs e) {
            if (!D3DOutputer.IsFrontBufferAvailable ) { 
                if (App.IsSoolverInitied)
                    Pages.SolverPage.UnloadSolver();
                CompositionTarget.Rendering -= CompositionTarget_Rendering;
                App.IsSoolverInitied = false;
            }
        }

        [DllImport("PuzzleSolver.vs2015.dll", EntryPoint = "Init_DynamicInvoke")]
        static extern void Init_PuzzleSolver(IntPtr i);

        [DllImport("PuzzleSolver.vs2015.dll", EntryPoint = "GetSurfacePointer_DynamicInvoke")]
        static extern IntPtr GetSurfacePointer();

        [DllImport("PuzzleSolver.vs2015.dll", EntryPoint = "Loop_DynamicInvoke")]
        static extern bool SolverLoop();

        [DllImport("PuzzleSolver.vs2015.dll", EntryPoint = "End_DynamicInvoke")]
        internal static extern void UnloadSolver();
    }
}
