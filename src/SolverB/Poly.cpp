//多角形クラスの実装

#include "Poly.h"

//初期化
Poly::Poly() {
}

Poly::Poly(Point point0, vector<Point> point, bool is_piece_) {
	this->point0 = point0;
	this->point = point;
	turn_cnt = 0;
	is_putted = false;
	this->is_piece_ = is_piece_;
}

//頂点数
int Poly::size() {
	return point.size();
}

//頂点idの絶対位置
Point Poly::get_point(int id) {
	return point0 + point[(id + size()) % size()];
}

//頂点idが点sとくっつき、頂点id -> id + 1をつなぐ有向線分と点s -> eをつなぐ有向線分が同じ向きになるように、多角形を移動する。
void Poly::move(int id, Point s, Point e) {
	Point a = get_point(id);
	Point b = get_point(id + 1);

	Point mul = (e - s) / (b - a);
	mul /= abs(mul);
	for (int i = 0; i < size(); i++) {
		point[i] *= mul;
	}

	point0 += s - get_point(id);
}

//回転ベクトル, 回転の中心, 平行移動量を指定して移動 (回転 -> 平行移動の順で実行)
void Poly::move2(Point mul, Point center, Point trans) {
	int i;
	for (i = 0; i < size(); i++) point[i] = get_point(i) - center;
	for (i = 0; i < size(); i++) point[i] *= mul;
	for (i = 0; i < size(); i++) point[i] += center - point0;
	point0 += trans;
}

//Polyを (頂点0のy座標を基準として) 反転し、頂点列の向きを修正する。
void Poly::turn() {
	int i;
	vector<Point> tmp(size());
	for (i = 0; i < point.size(); i++) tmp[i] = conj(point[i]);
	for (i = 0; i < point.size(); i++) point[i] = tmp[point.size() - 1 - i];
	turn_cnt++;
}

//軸を指定して反転
void Poly::turn2(double center_imag) {
	int i;
	vector<Point> tmp(size());

	for (i = 0; i < point.size(); i++) tmp[i] = get_point(i) - Point(0, center_imag);
	for (i = 0; i < point.size(); i++) tmp[i] = conj(tmp[i]);
	for (i = 0; i < point.size(); i++) tmp[i] += Point(0, center_imag) - point0;
	for (i = 0; i < point.size(); i++) point[i] = tmp[point.size() - 1 - i];
	turn_cnt++;
}

//接触判定
bool Poly::is_hit(Poly &poly) {
	int i, j;

	for (i = 0; i < size(); i++) {
		Line a = Line(get_point(i), get_point(i + 1));
		for (j = 0; j < poly.size(); j++) {
			Line b = Line(poly.get_point(j), poly.get_point(j + 1));
			if (a.ishit(b)) { return true; }
		}
	}
	return false;
}

//polyを含むか？(前提条件：接触していない)
bool Poly::is_contain(Poly &poly) {
	Point p = poly.get_point(0);
	Line hl = Line(p, Point(10000, 10000));
	int cnt = 0;

	for (int i = 0; i < size(); i++) {
		Line line = Line(get_point(i), get_point(i + 1));
		if (line.ishit_half(hl)) {
			cnt++;
		}
	}
	if (cnt % 2 == 1) return true;
	return false;
}

//polyに含まれるか？(前提条件：接触していない）
bool Poly::is_contained(Poly &poly) {
	Point p = get_point(0);
	Line hl = Line(p, Point(10000, 10000));
	int cnt = 0;

	for (int i = 0; i < poly.size(); i++) {
		Line line = Line(poly.get_point(i), poly.get_point(i + 1));
		if (line.ishit_half(hl)) {
			cnt++;
		}
	}
	if (cnt % 2 == 1) return true;
	return false;
}

//多角形の符号付き面積を返す.
//Im軸を↑Re軸を→に取った時、頂点列が時計回りなら負の値を返し、反時計回りなら正の値を返す。
double Poly::area() {
	double ret = 0;

	for (int i = 0; i < size(); i++) {
		double x1 = get_point(i).real();
		double y1 = get_point(i).imag();
		double x2 = get_point(i + 1).real();
		double y2 = get_point(i + 1).imag();
		ret += x1 * y2 - x2 * y1;
	}
	ret *= 0.5;
	return ret;
}

//反転状態ならtrueを返す. 使用例：枠を元の位置に戻す処理
bool Poly::is_turned() {
	return (turn_cnt % 2 == 1);
}

//頂点列の向きを強制的に変更する. 頂点0の位置は変更しない.
void Poly::point_reverse() {
	vector<Point> tmp;
	tmp = point;
	for (int i = 0; i < size(); i++) point[i] = tmp[(size() - i) % size()];
}

//多角形のタイプを返す
bool Poly::is_piece() {
	return is_piece_;
}
