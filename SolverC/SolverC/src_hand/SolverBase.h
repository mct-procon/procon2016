//基本機能

#pragma once
#include "Line.h"
#include "Poly.h"
#include "DxLib.h"

class SolverBase {
protected:
	double dist;			//当たり判定の緩さ
	double angle_error_deg;	//角度の誤差範囲が (2～3cmの辺に囲まれた頂点において) ±angle_error_deg[°]くらい。角度の評価に用いる。

	Poly input_poly(ifstream &ifs, bool is_clockwise);
	Poly scaledown_poly(Poly &poly, double dist);		//当たり判定用の輪郭の取得

public:
	void input(string filename, double dist, double angle_error_deg);									//入力
	void 余分な頂点を取り除く(double dist_error);
};
