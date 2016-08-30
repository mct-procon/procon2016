//Lineクラスの実装
//参考文献：http://www.prefield.com/algorithm/

#include "Line.h"

//ベクトルuとベクトルvの外積の符号付きサイズ = u.real * v.imag - u.imag * v.realを返す. ちなみに、cross(u, v) = -cross(v, u)が成り立つ。
//conj(u)はuの共役複素数u.real() - u.imag(), imag(x)はx.imag()と同じ。
double Line::cross(Point &u, Point &v) {
	return imag(conj(u) * v);
}

//交点計算
Point Line::get_cross_point(Line &line) {
	double A = cross(e - s, line.e - line.s);	//線分の端点を頂点とする平行四辺形の面積(>=0)
	double B = cross(e - s, e - line.s);		//s, e, line.sを頂点とする三角形の面積(>=0)
	return line.s + B / A * (line.e - line.s);			//交点を返す
}

//s -> eが反時計回りの多角形の頂点列i, i + 1であるとき、多角形について「外向きな単位法線ベクトル」を返す。
//すなわち、Re軸を→, Im軸を↑に取ったとき、s -> eを時計回りに90°回転したベクトルを大きさ1にして返す。
Point Line::get_unit_norm() {
	Point norm = complex<double>(0, 1) * (s - e);
	return norm / abs(norm);
}

//空の初期化
Line::Line() {
}

//まともな初期化
Line::Line(Point s, Point e) {
	this->s = s;
	this->e = e;
}

//当たり判定 (T字も当たっているとみなす)
bool Line::ishit(Line &line) {
	double u, v;

	u = cross(e - s, line.s - s);
	v = cross(e - s, line.e - s);
	if (max(u, v) < 0 || min(u, v) > 0) { return false; }
	u = cross(line.e - line.s, s - line.s);
	v = cross(line.e - line.s, e - line.s);
	if (max(u, v) < 0 || min(u, v) > 0) { return false; }
	return true;
}

//line1, line2との交点P, Qを求め、線分の範囲を[P, Q]にする。
void Line::cut_line(Line &line1, Line &line2) {
	s = get_cross_point(line1);
	e = get_cross_point(line2);
}

//線分を「外向きな単位法線ベクトル」の方向にdistだけ動かす。distが負の場合は逆方向に動く。
void Line::move_line(double dist) {
	Point trans = get_unit_norm();
	trans *= dist;
	s += trans;
	e += trans;
}

//半直線half_lineとの当たり判定
bool Line::ishit_half(Line &half_line) {
	double u, v;

	u = cross(half_line.e - half_line.s, s - half_line.s);
	v = cross(half_line.e - half_line.s, e - half_line.s);
	if (max(u, v) < 0 || min(u, v) > 0) { return false; }
	u = cross(e - s, half_line.s - s);
	v = cross(e - s, half_line.e - s);
	if ((max(u, v) < 0 || min(u, v) > 0) && abs(u) < abs(v)) { return false; }
	return true;
}