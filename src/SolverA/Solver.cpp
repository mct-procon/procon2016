#include "Solver.h"
#include "DxLib.h"

namespace std {
	bool operator<(const complex<double> &l, const complex<double> &r) {
		return (l.real() != r.real()) ? l.real() < r.real() : l.imag() < r.imag();
	}
};

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
	for (i = 0; i < n; i++) {
		point[i] = line[i].s;
	}
	point0 = point[0];
	for (i = 0; i < n; i++) {
		point[i] -= point0;
	}
	small_poly = Poly(point0, point);
	return small_poly;
}

//当たり判定。polyが既に置いたピース or 枠と重なる⇔true。
//dist = 0が厳密, distを大きくするほど「当たらなくなる」
//低速versionです。putted_line.size()が100を超えたら、高速化したい。
bool Solver::isoverlap(Poly &poly, double dist) {
	int i, j;
	Poly small_poly = get_scaledown_poly (poly, dist);	//ピースを縮小することで、イレギュラーなケースの発生を防ぐ。

	//線の交差判定
	for (i = 0; i < small_poly.size(); i++) {
		Line line = Line(small_poly.get_point(i), small_poly.get_point(i + 1));
		for (j = 0; j < putted_line.size(); j++) {
			if (line.ishit(putted_line[j])) {
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

//polyの結合度を計算します。
//実部昇順でソートされたputted_point (基準となる頂点の集合）を使います。 
//(今は愚直実装なので、ソートする必要ないけど…）
int Solver::get_combi_level(Poly &poly, double r) {
	int i, j;
	int cnt = 0;

	for (i = 0; i < putted_point.size(); i++) {
		for (j = 0; j < poly.size(); j++) {
			Point p = poly.get_point(j);
			if (abs(putted_point[i] - p) <= r) {
				cnt++;
				break;
			}
		}
	}
	return cnt;
}

//頂点dstと最も結合する頂点srcを返します
Vertex Solver::get_best_vertex(Vertex dst) {
	int best_score = -1;
	Vertex src = Vertex(false, -1, 0);
	int i, j;
	double r = 5;		//結合度用の許容範囲
	double dist = 5;	//当たり判定のゆるさ

	for (i = 0; i < piece.size(); i++) {
		if (isputted[i]) { continue; }
		for (j = 0; j < piece[i].size(); j++) {
			//「ピースiの頂点j」をdstに合わせる
			move(Vertex(true, i, j), dst);
			//ピースiの結合度を計算する
			int score = get_combi_level(piece[i], r);
			//結合度が最大なら、当たり判定をしてみて、当たってなければ結合度とか最善の頂点を更新する
			if (best_score < score && !isoverlap(piece[i], dist)) {
				best_score = score;
				src = Vertex(true, i, j);
			}
		}
	}
	return src;
}

//問題を解きます
void Solver::solve() {
	int i, j;

	//基準となる頂点集合の初期化
	for (i = 0; i < (int)waku.size(); i++) {
		for (j = 0; j < waku[i].size(); j++) {
			standard_point.push(Vertex(false, i, j));
		}
	}
	//位置が確定している点の集合の初期化
	for (i = 0; i < (int)waku.size(); i++) {
		for (j = 0; j < waku[i].size(); j++) {
			putted_point.push_back(waku[i].get_point(j));
		}
	}
	//位置が確定している辺の集合の初期化
	for (i = 0; i < (int)waku.size(); i++) {
		for (j = 0; j < waku[i].size(); j++) {
			putted_line.push_back(Line(waku[i].get_point(j), waku[i].get_point(j + 1)));
		}
	}

	while (!standard_point.empty()) {
		//基準となる頂点dstを一つ選ぶ
		Vertex dst = standard_point.front();
		//頂点dstを「基準となる頂点の集合」から削除する
		standard_point.pop();
		//頂点dstとペアになる頂点srcを探す
		Vertex src = get_best_vertex(dst);
		if (src.poly_id < 0) { continue; }
		//頂点srcが存在すれば、頂点dstと頂点srcがくっつくようにする
		move(src, dst);
		//頂点srcを持つ多角形(ピース)を「位置が確定した多角形」にする
		int id = src.poly_id;
		isputted[id] = true;
		//頂点srcを持つ多角形（ピース）の全頂点を「基準となる頂点の集合」に入れる
		for (i = 0; i < piece[id].size(); i++) {
			standard_point.push(Vertex(true, id, i));
		}
		//頂点srcを持つ多角形（ピース）の全頂点を「位置が確定している点の集合」に入れる
		for (i = 0; i < piece[id].size(); i++) {
			putted_point.push_back(piece[id].get_point(i));
		}
		sort(putted_point.begin(), putted_point.end());
		//頂点srcを持つ多角形（ピース）の全辺を「位置が確定している辺の集合」に入れる
		for (i = 0; i < piece[id].size(); i++) {
			putted_line.push_back(Line(piece[id].get_point(i), piece[id].get_point(i + 1)));
		}
	}
}

//回答を表示します
void Solver::draw(bool is_draw_didnot_put) {
	int i, j;

	//枠
	for (i = 0; i < (int)waku.size(); i++) {
		for (j = 0; j < waku[i].size(); j++) {
			Point s = waku[i].get_point(j);
			Point e = waku[i].get_point(j + 1);
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 255, 0), 2);
			DrawFormatString((int)s.real(), (int)s.imag(), GetColor(255, 0, 0), "%d", j);
		}
	}

	//ピース
	for (i = 0; i < (int)piece.size(); i++) {
		if (!isputted[i] && !is_draw_didnot_put) { continue; }
		for (j = 0; j < piece[i].size(); j++) {
			Point s = piece[i].get_point(j);
			Point e = piece[i].get_point(j + 1);
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 0, 255), 2);
		}
	}

	//当たり判定枠
	for (i = 0; i < (int)piece.size(); i++) {
		if (!isputted[i] && !is_draw_didnot_put) { continue; }
		Poly poly = get_scaledown_poly(piece[i], 5);
		for (j = 0; j < poly.size(); j++) {
			Point s = poly.get_point(j);
			Point e = poly.get_point(j + 1);
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(255, 0, 255), 2);
		}
	}
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