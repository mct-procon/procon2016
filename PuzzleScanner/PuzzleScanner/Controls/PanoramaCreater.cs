using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OpenCvSharp;

namespace PuzzleScanner.Controls {
    public static class PanoramaCreater {
        public static async Task<Emgu.CV.Mat> CreatePanorama(params Emgu.CV.Mat[] imgs) {
            return await INTERNAL_CreatePanorama(imgs.Select(x => new Mat(x.Ptr)).ToArray());
        }

        public static async Task<Emgu.CV.Mat> CreatePanorama(IEnumerable<Emgu.CV.Mat> imgs) {
            return await INTERNAL_CreatePanorama(imgs.Select(x => new Mat(x.Ptr)).ToArray());
        }

        private static async Task<Emgu.CV.Mat> INTERNAL_CreatePanorama(params Mat[] imgs){
            Mat Result = new Mat();
            using (Stitcher stitch = Stitcher.Create(true)) {
                stitch.Stitch(imgs, Result);
            }
            return new Emgu.CV.Mat(Result.Rows, Result.Cols, (Emgu.CV.CvEnum.DepthType)((int)Result.Type()), Result.Channels(),Result.DataStart,0);
        }
    }
}
