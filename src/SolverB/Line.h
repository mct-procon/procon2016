//線分クラス
 
#pragma once
#include "Point.h"
#include "STL.h"

class Line {
private:
	double cross(Point &u, Point &v);			//ベクトルu, vの外積の大きさを返す
	Point get_cross_point(Line &line);			//交点計算
	Point get_unit_norm();						//s -> eが反時計回りの多角形の頂点列i, i + 1であるとき、多角形について「外向きな単位法線ベクトル」を返す。

public:
	Point s, e;									//始点, 終点。以下の関数群は、始点と終点の座標を交換しても、正しく動作する。

	Line ();
	Line (Point s, Point e);
	bool ishit(Line &line);						//他の線分との交差判定. 境界上は当たっている(true)とみなす。
	void cut_line(Line &line1, Line &line2);	//line1, line2との交点P, Qを求め、線分の範囲を[P, Q]にする。
	void move_line(double dist);				//線分を「外向きの単位法線ベクトル」の方向にdistだけ動かす。distが負の場合は逆方向に動く。
	bool ishit_half(Line &half_line);			//半直線half_lineとの交差判定, 境界上は当たっている(true)とみなす。
};