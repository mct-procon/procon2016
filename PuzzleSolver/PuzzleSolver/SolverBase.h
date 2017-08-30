//基本機能

#pragma once
#include "Line.h"
#include "Poly.h"
#include "DxLib.h"
#include "SolverParameter.h"

class SolverBase {
protected:
	Poly input_poly(ifstream &ifs, bool is_clockwise, int defaultTag);
	void print_poly(ofstream &ofs, Poly poly);

public:
	SolverParameter parameter;							//当たり判定, 評価関数, マージ処理で用いるパラメータをまとめたもの
	void input(string filename);					//入力, 誤差パラメータの調整も行う
	void 余分な多角形を取り除く(double min_area);	//小さな多角形を取り除く
	void 余分な頂点を取り除く(double dist_error);	//input直後にのみ呼んでよい
	void print(string filename);
};
