//Lineクラスの実装
//参考文献：http://www.prefield.com/algorithm/

#include "Line.h"
using namespace std;

//ベクトルuとベクトルvの外積の符号付きサイズ = u.real * v.imag - u.imag * v.realを返す. ちなみに、cross(u, v) = -cross(v, u)が成り立つ。
//conj(u)はuの共役複素数u.real() - u.imag(), imag(x)はx.imag()と同じ。
double Line::cross(Point &u, Point &v) {
	return imag(conj(u) * v);
}

//交点計算
Point Line::cross_point(Line &line) {
	double A = cross(e - s, line.e - line.s);	//線分の端点を頂点とする平行四辺形の面積(>=0)
	double B = cross(e - s, e - line.s);		//s, e, line.sを頂点とする三角形の面積(>=0)
	return line.s + B / A * (line.e - line.s);	//交点を返す
}

//s -> eが反時計回りの多角形の頂点列i, i + 1であるとき、多角形について「外向きな単位法線ベクトル」を返す。
//すなわち、Re軸を→, Im軸を↑に取ったとき、s -> eを時計回りに90°回転したベクトルを大きさ1にして返す。
Point Line::unit_norm() {
	Point norm = complex<double>(0, 1) * (s - e);
	return norm / abs(norm);
}

//2つのベクトルu, vの内積を返す
double Line::dot(Point &u, Point &v) {
	return u.real() * v.real() + u.imag() * v.imag();
}

//空の初期化
Line::Line() {
}

//まともな初期化
Line::Line(Point s, Point e) {
	this->s = s;
	this->e = e;
}




/*更新*/
//line1, line2との交点P, Qを求め、線分の範囲を[P, Q]にする。
void Line::cut(Line &line1, Line &line2) {
	s = cross_point(line1);
	e = cross_point(line2);
}

//線分を「外向きな単位法線ベクトル」の方向にdistだけ動かす。distが負の場合は逆方向に動く。
void Line::move(double dist) {
	Point trans = unit_norm();
	trans *= dist;
	s += trans;
	e += trans;
}

//線分を原点Oを中心として、angleだけ回転（angleを掛けるだけ）
void Line::rotate(Point angle) {
	s *= angle;
	e *= angle;
}

//transだけ平行移動
void Line::transrate(Point trans) {
	s += trans;
	e += trans;
}

//y = 0を軸として反転
void Line::turn() {
	s = conj(s);
	e = conj(e);
}




/*計算*/
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

//点pointから線分thisへの距離
double Line::dist(Point point) {
	if (dot(point - s, e - s) <= 0) { return abs(point - s); }
	if (dot(point - e, s - e) <= 0) { return abs(point - e); }
	return abs(cross(e - s, point - s) / abs(e - s));
}

//線分lineとの一致度
//0…一点も共有していない
//4…線分が一致
//2…向きが逆
//1…向きが同じ
//線分の方向を見る際にも用いているので、ここの戻り値を変えると、marge_poly()と角度の評価がバグります。
//線分の方向を返す関数とスコア関数を分け、適切な関数名を付けるの、誰かやってほしい。
//normは実部^2 + 虚部^2 (つまり長さ^2)を返す。
double Line::machi_score(Line line, double dist_error, double angle_error_deg) {
	double dist_error2 = dist_error * dist_error;
	if (norm(s - line.e) <= dist_error2 || norm(e - line.s) <= dist_error2) swap(line.s, line.e);
	if (norm(s - line.s) <= dist_error2 && norm(e - line.e) <= dist_error2) return 4;
	if (norm(s - line.s) > dist_error2  && norm(e - line.e) > dist_error2) return 0;

	Point angle = (e - s) / (line.e - line.s);
	angle /= abs(angle);
	if (abs(angle.imag()) > sin(angle_error_deg * 3.1415926 / 180.0)) return 0;
	if (angle.real() < 0) return 2;
	return 1;
}
