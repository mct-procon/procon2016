//線分クラス
 
#pragma once
#include "Point.h"

class Line {
private:
	double cross(Point &u, Point &v);			//ベクトルu, vの外積の大きさを返す
	Point get_cross_point(Line &line);			//交点計算
	Point get_unit_norm();						//s -> e方向から見て外向きの、単位法線ベクトルを返す。

public:
	Point s, e;

	Line (Point s, Point e);
	bool ishit(Line &line);						//他の線分との交差判定. 境界上は当たっている(true)とみなす。
	void cut_line(Line &line1, Line &line2);	//line1, line2との交点P, Qを求め、線分の範囲を[P, Q]にする。
	void move_line(double dist);				//線分を「単位法線ベクトル」の方向にdistだけ動かす。distが負の場合は逆方向に動く。
};