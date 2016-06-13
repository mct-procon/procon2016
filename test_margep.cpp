
//Gpu
public bool CanMarge (vector<Point> shape1, vector<Point> shape2) {
  shape1.push_back(shape1[0]);
  shape2.push_back(shape2[0]);
  array<Point, 1> shape1_arr(shape1.size());
  array<Point, 1> shape2_arr(shape2.size());
  copy(shape1.begin(), shape1.end(), shape1_arr);
  copy(shape2.begin(), shape2.end(), shape2_arr);
  array_view<Point> shape1_av(shape1_arr.size(), shape1_arr);
  array_view<Point> shape2_av(shape2_arr.size(), shape2_arr);
  array_view<bool> res_av(1, {true});
  bool res = true;
  int shape1_max = shape1_arr.size()
  int shape2_max = shape2_arr.size();
  parallel_for_each(extent<2>(shape1_arr.size() - 1, shape2_arr.size() - 1), [shape1_av, shape2_av, res](index<2> idx) restrict(amp) {
     if(res_av[0])
       res_av[0] ^= ((shape1_av[idx[0]].X - shape1_av[idx[0] + 1].X) * (shape2_av[idx[1]].Y - shape1_av[idx[0]].Y) + (shape1_av[idx[0]].Y - by) * (ax - cx)) * ((ax - bx) * (dy - ay) + (ay - by) * (ax - dx)) < 0 && ((cx - dx) * (ay - cy) + (cy - dy) * (cx - ax)) * ((cx - dx) * (by - cy) + (cy - dy) * (cx - bx)) < 0;
  });
}

//Cpu
public bool CanMarge (vector<Point> shape1, vector<Point> shape2) {
  shape1.push_back(shape1[0]);
  shape2.push_back(shape2[0]);
  bool res = true;
#ifdef _OPENMP
  #pragma omp parallel for
#endif
  for(int n = 0;n < shape1.size() - 1; ++n) {
    for(int m = 0; m < shape2.size() - 1; ++m) {
      res ^= margep(shape1[n].X, shape1[n].Y, shape1[n + 1].X, shape1[n + 1].Y, shape2[m].X, shape2[m].Y, shape2[m + 1].X, shape2[m + 1].Y);
#ifdef _OPENMP
      #pragma omp cancel for if (!res)
#endif
    }
  }
  return res;
}



private bool margep(short ax, short ay, short bx, short by, short cx, short cy, short dx, short dy) {
  return ((ax - bx) * (cy - ay) + (ay - by) * (ax - cx)) * ((ax - bx) * (dy - ay) + (ay - by) * (ax - dx)) < 0 && ((cx - dx) * (ay - cy) + (cy - dy) * (cx - ax)) * ((cx - dx) * (by - cy) + (cy - dy) * (cx - bx)) < 0;
}

//this is readable code
//Please read this homepage : http://www5d.biglobe.ne.jp/%7Etomoya03/shtml/algorithm/Intersection.htm
private bool margep(short ax, short ay, short bx, short by, short cx, short cy, short dx, short dy) {
  var ta = (cx - dx) * (ay - cy) + (cy - dy) * (cx - ax);
  var tb = (cx - dx) * (by - cy) + (cy - dy) * (cx - bx);
  var tc = (ax - bx) * (cy - ay) + (ay - by) * (ax - cx);
  var td = (ax - bx) * (dy - ay) + (ay - by) * (ax - dx);

  return tc * td < 0 && ta * tb < 0;
}