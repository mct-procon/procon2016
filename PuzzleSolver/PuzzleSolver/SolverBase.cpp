#include "SolverBase.h"

//多角形を読み込む
Poly SolverBase::input_poly(ifstream &ifs, bool is_clockwise) {
	int n;
	vector<Point> points;	//多角形の頂点列
	vector<Line> lines;		//表示する線分

	ifs >> n;
	for (int i = 0; i < n; i++) {
		double x, y;
		ifs >> x >> y;
		points.push_back(Point(x, y));
	}
	points.push_back(points[0]);

	for (int i = 0; i < n; i++) {
		lines.push_back(Line(points[i], points[i + 1]));
	}
	Poly ret = Poly(points, lines);

	if (is_clockwise && ret.area() > 0) ret.point_reverse();
	if (!is_clockwise && ret.area() < 0) ret.point_reverse();
	return ret;
}


//当たり判定用の輪郭を返す。
//dist > 0のとき…ピースの場合は当たり判定枠が内側にできる。枠の場合は当たり判定枠が外側にできる。
Poly SolverBase::scaledown_poly(Poly &poly) {
	int i;
	int n = poly.size();
	vector<Line> lines(n);
	vector<Point> points(n + 1);

	//多角形内部に線分を引く
	for (i = 0; i < n; i++) {
		lines[i] = Line(poly.points[i], poly.points[i + 1]);
		lines[i].move(-dist);
	}
	for (i = 0; i < n; i++) {
		int id1 = i - 1; if (id1 < 0) id1 += n;
		int id2 = i + 1; if (id2 >= n) id2 -= n;
		lines[i].cut(lines[id1], lines[id2]);	//lines[i].s <- lines[id1]との交点, lines[i].e <- lines[id2]との交点
	}

	//多角形を構成
	for (i = 0; i < n; i++) {
		points[i] = lines[i].s;
	}
	points[n] = points[0];
	return Poly(points, lines);		//第２引数は何でも良い
}

bool SolverBase::inputSerialize(ifstream* ifs) {
	if (ifs->fail())
		return false;
	int poly_num;

	*ifs >> poly_num;
	for (int i = 0; i < poly_num; i++) { wakus.push_back(input_poly(*ifs, true)); }
	*ifs >> poly_num;

	for (int i = 0; i < poly_num; i++) { pieces.push_back(input_poly(*ifs, false)); }
	ifs->close();

	return true;
}

//入力 (新しいフォーマットで読む)
void SolverBase::input(string filename, double dist) {
	this->dist = dist;

	ifstream ifs(filename); 
	inputSerialize(&ifs);
}

bool SolverBase::inputTstr(tstring filename, double dist) {
	this->dist = dist;

	ifstream ifs(filename);
	return inputSerialize(&ifs);
}


//ここでは例外的に、メンバー変数を直接操作しています。
void SolverBase::余分な頂点を取り除く(double dist_error) {
	int i, j;

	for (i = 0; i < pieces.size(); i++) {
		while (true) {
			for (j = 0; j < pieces[i].size(); j++) {
				int a = j - 1 + ((j == 0) * pieces[i].size());
				int b = j;
				int c = j + 1;
				Line line = Line(pieces[i].points[a], pieces[i].points[c]);

				if (line.dist(pieces[i].points[b]) <= dist_error) {
					break;
				}
			}
			if (j == pieces[i].size()) { break; }	//終了
			//頂点jを取り除いて, どうぞ
			vector<Point> cycle;
			int id = j;
			for (j = 0; j < pieces[i].size(); j++) if (j != id) cycle.push_back(pieces[i].points[j]);
			cycle.push_back(cycle[0]);
			pieces[i].points = cycle;
		}
		vector<Line> lines;
		for (j = 0; j < pieces[i].size(); j++) { lines.push_back(Line(pieces[i].points[j], pieces[i].points[j + 1])); }
		pieces[i] = Poly(pieces[i].points, lines);
	}

	for (i = 0; i < wakus.size(); i++) {
		while (true) {
			for (j = 0; j < wakus[i].size(); j++) {
				int a = j - 1 + ((j == 0) * wakus[i].size());
				int b = j;
				int c = j + 1;
				Line line = Line(wakus[i].points[a], wakus[i].points[c]);

				if (line.dist(wakus[i].points[b]) <= dist_error) {
					break;
				}
			}
			if (j == wakus[i].size()) { break; }	//終了
			//頂点jを取り除いて, どうぞ
			vector<Point> cycle;
			int id = j;
			for (j = 0; j < wakus[i].size(); j++) if (j != id) cycle.push_back(wakus[i].points[j]);
			cycle.push_back(cycle[0]);
			wakus[i].points = cycle;
		}
		vector<Line> lines;
		for (j = 0; j < wakus[i].size(); j++) { lines.push_back(Line(wakus[i].points[j], wakus[i].points[j + 1])); }
		wakus[i] = Poly(wakus[i].points, lines);
	}
}


//表示 (中心座標, 拡大率, 画面サイズX, 画面サイズY)
void SolverBase::draw(Point center, double scale, double windowSizeX, double windowSizeY) {
	int i, j;
	Point s, e;
	Point trans = Point(windowSizeX / 2, windowSizeY / 2);

	//辺 (ゾンビは無視)
	for (i = 0; i < wakus.size(); i++) {
		if (wakus[i].size() <= 0) continue;
		for (j = 0; j < wakus[i].lines.size(); j++) {
			s = wakus[i].lines[j].s;
			e = wakus[i].lines[j].e;
			//変換
			s -= center; s *= scale; s += trans;
			e -= center; e *= scale; e += trans;
			//表示
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 255, 0), 2);
		}
	}
	for (i = 0; i < pieces.size(); i++) {
		if (pieces[i].size() <= 0) continue;
		for (j = 0; j < pieces[i].lines.size(); j++) {
			s = pieces[i].lines[j].s;
			e = pieces[i].lines[j].e;
			//変換
			s -= center; s *= scale; s += trans;
			e -= center; e *= scale; e += trans;
			//表示
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 255, 0), 2);
			//DrawFormatString((int)(s.real() + e.real())/ 2, (int)(s.imag() + e.imag()) / 2, 0, "E%d", j);
		}
	}

	//多角形の番号 (ゾンビは無視)
	for (i = 0; i < wakus.size(); i++) {
		if (wakus[i].size() <= 0) continue;
		s = Point(0, 0); for (j = 0; j < wakus[i].size(); j++) s += wakus[i].points[j]; s /= wakus[i].size();
		//変換
		s -= center; s *= scale; s += trans;
		//表示
		DrawFormatString((int)s.real(), (int)s.imag(), GetColor(255, 0, 0), "%d", i);
	}

	for (i = 0; i < pieces.size(); i++) {
		if (pieces[i].size() <= 0) continue;
		s = Point(0, 0); for (j = 0; j < pieces[i].size(); j++) s += pieces[i].points[j]; s /= pieces[i].size();
		//変換
		s -= center; s *= scale; s += trans;
		//表示
		DrawFormatString((int)s.real(), (int)s.imag(), GetColor(0, 0, 255), "%d", i);
	}


	//頂点列（紫, 赤）
	for (i = 0; i < wakus.size(); i++) {
		for (j = 0; j < wakus[i].size(); j++) {
			s = wakus[i].points[j];
			e = wakus[i].points[j + 1];
			//変換
			s -= center; s *= scale; s += trans;
			e -= center; e *= scale; e += trans;
			//表示
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(255, 0, 255), 2);
			DrawFormatString((int)s.real(), (int)s.imag(), GetColor(255, 0, 255), "%d", j);
		}
	}

	for (i = 0; i < pieces.size(); i++) {
		for (j = 0; j < pieces[i].size(); j++) {
			s = pieces[i].points[j];
			e = pieces[i].points[j + 1];
			//変換
			s -= center; s *= scale; s += trans;
			e -= center; e *= scale; e += trans;
			//表示
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(255, 0, 0), 2);
			DrawFormatString((int)s.real(), (int)s.imag(), GetColor(255, 0, 0), "%d", j);
		}
	}

	//頂点数
	/*for (i = 0; i < pieces.size(); i++) {
		if (pieces[i].size() <= 0) continue;
		s = Point(0, 0); for (j = 0; j < pieces[i].size(); j++) s += pieces[i].points[j]; s /= pieces[i].size();
		//変換
		s -= center; s *= scale; s += trans;
		s += 20;
		//表示
		DrawFormatString((int)s.real(), (int)s.imag(), GetColor(0, 0, 0), "%d", pieces[i].size());
	}*/
}
