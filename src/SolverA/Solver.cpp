#include "Solver.h"
#include "DxLib.h"

//頂点srcを頂点dstにくっつけるように、移動する
void Solver::move(Vertex src, Vertex dst) {
	Point s, e;
	if (dst.is_piece) {
		s = piece[dst.poly_id].get_point(dst.vertex_id + 1);
		e = piece[dst.poly_id].get_point(dst.vertex_id);
	}
	else {
		s = waku[dst.poly_id].get_point(dst.vertex_id);
		e = waku[dst.poly_id].get_point(dst.vertex_id + 1);
	}
	piece[src.poly_id].move(src.vertex_id, s, e);
}

//「polyの縮小多角形」を返します. distが大きいほど縮小多角形は小さい。
Poly Solver::get_scaledown_poly (Poly &poly, double dist) {
	int i;
	int n = poly.size();
	vector<Line> line(n);
	Point point0;
	vector<Point> point(n);
	Poly small_poly;

	//多角形内部に線分を引く
	for (i = 0; i < n; i++) {
		line[i] = Line(poly.get_point(i), poly.get_point(i + 1));
		line[i].move_line(-dist);
	}
	for (i = 0; i < n; i++) {
		int id1 = (i - 1 + n) % n;
		int id2 = (i + 1) % n;
		line[i].cut_line(line[id1], line[id2]);	//lines[i].s <- lines[id1]との交点, lines[i].e <- lines[id2]との交点
	}

	//多角形を構成
	point0 = line[0].s;
	for (i = 0; i < n; i++) {
		point[i] = line[i].s - point0;
	}
	small_poly = Poly(point0, point);
	return small_poly;
}

//当たり判定。polyが既に置いたピース or 枠と重なる⇔true。
//dist = 0が厳密, distを大きくするほど「当たらなくなる」
//中速versionです。putted_line.size()が100を超えたら、高速化したい。
bool Solver::isoverlap(Poly &poly, double dist) {
	int i, j;
	Poly small_poly = get_scaledown_poly (poly, dist);	//ピースを縮小することで、イレギュラーなケースの発生を防ぐ。

	//線の交差判定
	double lx = 10000, ly = -10000, rx = 10000, ry = -10000;
	vector<Line> line(poly.size());

	for (i = 0; i < small_poly.size(); i++) {
		Point p = small_poly.get_point(i);
		lx = min(lx, p.real());
		rx = max(rx, p.real());
		ly = min(ly, p.imag());
		ry = max(ry, p.imag());
		line[i] = Line(p, small_poly.get_point(i + 1));
	}

	for (i = 0; i < putted_line.size(); i++) {
		double min_x = min(putted_line[i].s.real(), putted_line[i].e.real()); if (min_x > rx) { continue; }
		double max_x = max(putted_line[i].s.real(), putted_line[i].e.real()); if (max_x < lx) { continue; }
		double min_y = min(putted_line[i].s.imag(), putted_line[i].e.imag()); if (min_y > ry) { continue; }
		double max_y = max(putted_line[i].s.imag(), putted_line[i].e.imag()); if (max_y < ly) { continue; }
		
		for (j = 0; j < small_poly.size(); j++) {
			if (putted_line[i].ishit(line[j])) {
				return true;
			}
		}
	}

	//点の内外判定 (small_polyの代表点1つについて判定すればよい）
	Point p = small_poly.get_point(0);
	Line hl = Line(p, Point(p.real(), 10000));
	int cnt = 0;

	//半直線hlと当たるputted_line[i]のカウント
	for (i = 0; i < putted_line.size(); i++) {
		if (hl.ishit(putted_line[i])) {
			cnt++;
		}
	}
	//偶数回当たったら、枠の外かピースの中に点がある
	if (cnt % 2 == 0) {
		return true;
	}
	return false;
}

//回答を表示します
void Solver::draw(bool is_draw_didnot_put) {
	int i, j;
	double scale = 0.45;

	//枠
	for (i = 0; i < (int)waku.size(); i++) {
		for (j = 0; j < waku[i].size(); j++) {
			Point s = waku[i].get_point(j) * scale;
			Point e = waku[i].get_point(j + 1) * scale;
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 0, 0), 2);
			DrawFormatString((int)s.real(), (int)s.imag(), GetColor(255, 0, 0), "%d", j);
		}
	}
	
	//ピース
	for (i = 0; i < (int)piece.size(); i++) {
		if (!isputted[i] && !is_draw_didnot_put) { continue; }
		for (j = 0; j < piece[i].size(); j++) {
			Point s = piece[i].get_point(j) * scale;
			Point e = piece[i].get_point(j + 1) * scale;

			if (isputted[i])
				DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 0, 255), 2);
			else
				DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 255, 0), 2);
		}
	}

	//当たり判定枠
	for (i = 0; i < (int)piece.size(); i++) {
		if (!isputted[i] && !is_draw_didnot_put) { continue; }
		Poly poly = get_scaledown_poly(piece[i], 5);
		for (j = 0; j < poly.size(); j++) {
			Point s = poly.get_point(j) * scale;
			Point e = poly.get_point(j + 1) * scale;
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(255, 0, 255), 2);
		}
	}

	//スコア
	int score = 0;
	for (i = 0; i < (int)piece.size(); i++) {
		if (isputted[i]) {
			score++;
		}
	}
	DrawFormatString(500, 150, 0, "置けた石の数 = %d (/%d)", score, (int)piece.size());

	//面積リスト
	vector<double> area; area.resize(piece.size());
	for (i = 0; i < (int)piece.size(); i++) {
		area[i] = 0;
		for (j = 0; j < piece[i].size(); j++) {
			double u = piece[i].get_point(j).real() * piece[i].get_point(j + 1).imag();
			double v = piece[i].get_point(j).imag() * piece[i].get_point(j + 1).real();
			double det = u - v;
			area[i] += det;
		}
		area[i] *= 0.5;
	}

	//埋めた割合[%]
	double area_sum = 0, area_putted = 0;
	for (i = 0; i < (int)piece.size(); i++) {
		area_sum += area[i];
		if (isputted[i]) area_putted += area[i];
	}
	DrawFormatString(500, 180, 0, "埋めた割合 = %.1f[%%]", area_putted * 100.0 / area_sum);

	//計測
	DrawFormatString(500, 210, 0, "計測に入った回数 = %d[回]", timer);
}

//初期化します
Solver::Solver() {
	for (int i = 0; i < 50; i++) {
		isputted[i] = false;
	}
}

//読み込みます
void Solver::input(string filename) {
	ifstream ifs(filename);
	if (ifs.fail()) { return; }

	int waku_num, piece_num;
	int point_num;
	int x[50], y[50];

	//枠
	ifs >> waku_num;
	for (int i = 0; i < waku_num; i++) {
		//頂点数
		ifs >> point_num;
		//X座標
		for (int j = 0; j < point_num; j++) {
			ifs >> x[j];
		}
		//Y座標
		for (int j = 0; j < point_num; j++) {
			ifs >> y[j];
		}
		//構築
		Point point0;
		vector<Point> point;
		for (int j = 0; j < point_num; j++) {
			point.push_back(Point(x[j], y[j]));
		}
		//相対座標形式にする
		point0 = point[0];
		for (int j = 0; j < point_num; j++) {
			point[j] = point[j] - point0;
		}
		//枠穴を追加
		waku.push_back(Poly(point0, point));
	}

	//ピース
	ifs >> piece_num;
	for (int i = 0; i < piece_num; i++) {
		//頂点数
		ifs >> point_num;
		//X座標
		for (int j = 0; j < point_num; j++) {
			ifs >> x[j];
		}
		//Y座標
		for (int j = 0; j < point_num; j++) {
			ifs >> y[j];
		}
		//構築
		Point point0;
		vector<Point> point;
		for (int j = 0; j < point_num; j++) {
			point.push_back(Point(x[j], y[j]));
		}
		//相対座標形式にする
		point0 = point[0];
		for (int j = 0; j < point_num; j++) {
			point[j] = point[j] - point0;
		}
		//ピースを追加
		piece.push_back(Poly(point0, point));
	}
}

//回答を書き込みます
void Solver::print(string filename) {
	ofstream ofs(filename);

	//枠穴
	ofs << waku.size() << endl;
	for (int i = 0; i < (int)waku.size(); i++) {
		//頂点数
		int n = waku[i].size();
		ofs << n << endl;
		//X座標
		ofs << waku[i].get_point(0).real();
		for (int j = 1; j < n; j++) {
			ofs << " " << waku[i].get_point(j).real();
		}
		ofs << endl;
		//Y座標
		ofs << waku[i].get_point(0).imag();
		for (int j = 1; j < n; j++) {
			ofs << " " << waku[i].get_point(j).imag();
		}
		ofs << endl;
		//改行
		ofs << endl;
	}

	//ピース
	ofs << piece.size() << endl;
	for (int i = 0; i < (int)piece.size(); i++) {
		//頂点数
		int n = piece[i].size();
		ofs << n << endl;
		//X座標
		ofs << piece[i].get_point(0).real();
		for (int j = 1; j < n; j++) {
			ofs << " " << piece[i].get_point(j).real();
		}
		ofs << endl;
		//Y座標
		ofs << piece[i].get_point(0).imag();
		for (int j = 1; j < n; j++) {
			ofs << " " << piece[i].get_point(j).imag();
		}
		ofs << endl;
		//改行
		ofs << endl;
	}	
}