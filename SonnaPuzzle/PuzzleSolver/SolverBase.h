//基本機能

#pragma once
#include "Line.h"
#include "Poly.h"
#include "DxLib.h"
#include "WinAPIExt.h"

class SolverBase {
protected:
	double dist;			//当たり判定の緩さ
	vector<Poly> pieces;	//反時計回りの頂点列
	vector<Poly> wakus;		//時計回りの頂点列

	Poly input_poly(ifstream &ifs, bool is_clockwise);
	Poly scaledown_poly(Poly &poly);		//当たり判定用の輪郭の取得

	bool inputSerialize(ifstream*);
public:
	void input(string filename, double dist);											//入力
	bool inputTstr(tstring filename, double dist);
	void 余分な頂点を取り除く(double dist_error);
	void draw(Point center, double scale, double windowSizeX, double windowSizeY);		//表示
};
