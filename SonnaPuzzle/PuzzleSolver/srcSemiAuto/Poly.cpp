//多角形クラスの実装

#include "Poly.h"

//外接長方形の情報を更新
void Poly::update_rect() {
	lx = rx = points[0].real();
	ly = ry = points[0].imag();
	for (int i = 1; i < size(); i++) {
		Point p = points[i];
		lx = min(lx, p.real());
		ly = min(ly, p.imag());
		rx = max(rx, p.real());
		ry = max(ry, p.imag());
	}
}

//初期化
Poly::Poly() {
}

Poly::Poly(vector<Point> points, vector<Line> lines) {
	this->points = points;
	this->lines = lines;
	this->is_turn = false;
	update_rect();
}




/*更新*/
//頂点列の向きを強制的に変更する.
void Poly::point_reverse() {
	vector<Point> tmp;
	tmp = points;
	for (int i = 0; i < points.size(); i++) points[i] = tmp[points.size() - 1 - i];
}


#include "DxLib.h"

//移動（半直線points[id]->points[id+1]を半直線s->eにくっつける)
void Poly::move(int id, Point s, Point e) {
	Point mul = (e - s) / (points[id + 1] - points[id]);

	//頂点列の回転
	mul /= abs(mul);
	for (int i = 0; i < points.size(); i++) { points[i] *= mul; }

	//頂点列の平行移動
	Point trans = s - points[id];
	for (int i = 0; i < points.size(); i++) { points[i] += trans; }

	//表示辺の移動
	for (int i = 0; i < lines.size(); i++) { lines[i].rotate(mul); lines[i].transrate(trans); }

	//外接四角形の再計算
	update_rect();
}


//移動 (半直線point[id+1]->point[id]を半直線s->eにくっつける
void Poly::move_reverse(int id, Point s, Point e) {
	Point mul = (e - s) / (points[id] - points[id + 1]);
	
	//頂点列の回転
	mul /= abs(mul);
	for (int i = 0; i < points.size(); i++) { points[i] *= mul; }

	//頂点列の平行移動
	Point trans = s - points[id + 1];
	for (int i = 0; i < points.size(); i++) { points[i] += trans; }

	//表示辺の移動
	for (int i = 0; i < lines.size(); i++) { lines[i].rotate(mul); lines[i].transrate(trans); }

	//外接四角形の再計算
	update_rect();
}


//反転（y=points[0].imag()を基準として反転し、頂点列の向きを修正する。）
void Poly::turn() {
	int i;
	vector<Point> tmp(points.size());

	//頂点列の反転
	for (i = 0; i < points.size(); i++) tmp[i] = Point(points[i].real(), 2 * points[0].imag() - points[i].imag());
	for (i = 0; i < points.size(); i++) points[i] = tmp[points.size() - 1 - i];

	//表示辺の反転
	for (i = 0; i < lines.size(); i++) lines[i].turn(points[0].imag());

	//反転フラグの更新
	is_turn = !is_turn;

	//外接四角形の再計算
	update_rect();
}





/*取得*/
//頂点数
int Poly::size() {
	return (int)points.size() - 1;
}

//多角形の符号付き面積を返す.
//Im軸を↑Re軸を→に取った時、頂点列が時計回りなら負の値を返し、反時計回りなら正の値を返す。
double Poly::area() {
	double ret = 0;

	for (int i = 0; i < size(); i++) {
		double x1 = points[i].real();
		double y1 = points[i].imag();
		double x2 = points[i + 1].real();
		double y2 = points[i + 1].imag();
		ret += x1 * y2 - x2 * y1;
	}
	ret *= 0.5;
	return ret;
}

//辺が接触しているならtrue, 接触していないならfalseを返す.
bool Poly::is_hit(Poly &poly) {
	int i, j;

	if (lx > poly.rx || rx < poly.lx || ly > poly.ry || ry < poly.ly) { return false; }

	for (i = 0; i < size(); i++) {
		Line a = Line(points[i], points[i + 1]);
		for (j = 0; j < poly.size(); j++) {
			Line b = Line(poly.points[j], poly.points[j + 1]);
			if (a.ishit(b)) { return true; }
		}
	}
	return false;
}

//頂点pを含んでいればtrueを返す。
bool Poly::is_cover_point(Point &p) {
	Line hl = Line(p, Point(10000, 0));
	int cnt = 0;

	for (int i = 0; i < size(); i++) {
		Line line = Line(points[i], points[i + 1]);
		if (line.ishit_half(hl)) {
			cnt++;
		}
	}
	if (cnt % 2 == 1) return true;
	return false;
}

//polyを含んでいるならtrueを返す。（使用時の条件：接触していない)
bool Poly::is_cover(Poly &poly) {
	if (poly.lx < lx || poly.rx > rx || poly.ly < ly || poly.ry > ry) { return false; }
	return is_cover_point(poly.points[0]);
}
