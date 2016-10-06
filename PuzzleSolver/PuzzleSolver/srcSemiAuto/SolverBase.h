//基本機能

#pragma once
#include "Line.h"
#include "Poly.h"
#include "DxLib.h"
#include "SolverParameter.h"

class SolverBase {
protected:
	Poly input_poly(ifstream &ifs, bool is_clockwise);
	Poly scaledown_poly(Poly &poly, double dist);		//当たり判定用の輪郭の取得
	SolverParameter parameter;							//当たり判定, 評価関数, マージ処理で用いるパラメータをまとめたもの

public:
	void input(string filename, double dist_error, double angle_error);		//入力, 誤差パラメータの調整も行う
	void 余分な頂点を取り除く(double dist_error);
	void 多角形を拡大(double dist_piece, double dist_waku);
};
