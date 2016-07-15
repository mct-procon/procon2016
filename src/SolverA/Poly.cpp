//多角形クラスの実装

#include "Poly.h"

//初期化
Poly::Poly() {
}

Poly::Poly(Point point0, vector<Point> point) {
	this->point0 = point0;
	this->point = point;
}

//頂点数を返す
int Poly::size() {
	return point.size();
}

//頂点idの絶対座標を返す
Point Poly::get_point(int id) {
	return point0 + point[(id + size()) % size()];
}

//頂点idの角度[°]を返す. 値域は[0, 360)
double Poly::get_angle_deg(int id) {
	Point a = get_point(id - 1) - get_point(id);
	Point b = get_point(id + 1) - get_point(id);

	double rad = atan2((b / a).imag(), (b / a).real());
	double deg = rad * 180.0 / 3.14159265358979;
	while (deg < 0) { deg += 360; }
	while (deg >= 360) { deg -= 360; }
	return deg;
}

//頂点idが点sとくっつき、頂点id -> id + 1をつなぐ有向線分と点s -> eをつなぐ有向線分が同じ向きになるように、多角形を移動する。
void Poly::move(int id, Point s, Point e) {
	Point a = get_point(id);
	Point b = get_point(id + 1);

	//頂点0を中心に回転. ベクトルa -> bがベクトルs -> eと同じ向きになるように「全頂点を」回転する。
	Point mul = (e - s) / (b - a);
	mul /= abs(mul);
	for (int i = 0; i < size(); i++) {
		point[i] *= mul;
	}

	//平行移動
	point0 += s - get_point(id);
}