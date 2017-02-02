//線分クラス
 
#pragma once
#include "Point.h"
#include "stdafx.h"

class Line {
private:
	//ライブラリ
	double cross(Point &u, Point &v);			//ベクトルu, vの外積の大きさを返す
	Point unit_norm();							//s -> eが反時計回りの多角形の頂点列i, i + 1であるとき、多角形について「外向きな単位法線ベクトル」を返す。
	double dot(Point &u, Point &v);				//ベクトルu, vの内積を返す
	int tag;									//[タグが存在するか(1ビット), ピースに貼られたシールの値(7ビット), ラインの番号(7ビット)]

public:
	Point s, e;									//始点, 終点。以下の関数群は、始点と終点の座標を交換しても、正しく動作する。

	Line ();
	Line (Point s, Point e);

	//更新
	void cut(Line &line1, Line &line2);			//line1, line2との交点P, Qを求め、線分の範囲を[P, Q]にする。
	void move(double dist);						//線分をunit_norm()の方向にdistだけ動かす。distが負の場合は逆方向に動く.
	void rotate(Point angle);					//線分を原点Oを中心として、angleだけ回転（angleを掛けるだけ）
	void transrate(Point trans);				//transだけ平行移動
	void turn(double center_y);					//y = center_yを中心に反転

	//計算
	double length();
	Point cross_point(Line &line);				//交点計算
	bool ishit(Line &line);														//他の線分との交差判定.
	bool ishit_half(Line &half_line);											//半直線half_lineとの交差判定.
	double dist(Point point);													//点pointから線分thisまでの距離.
	double straight_dist(Point point);											//点pointから直線thisまでの距離
	double match_score(Line line, double dist_error, double angle_error_deg);	//線分lineとの一致度

	//演算子
	bool operator==(const Line &r) { return s == r.s && e == r.e; }

	//入出力用
	void set_tag(int poly_id, int line_id);
	bool is_exist_tag();
	int poly_id();
	int line_id();
};
