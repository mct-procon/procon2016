#include "MySolver.h"

void capture(MySolver backup);

//頂点piece_vと頂点dstがくっついているか？を判定 (くっついていればtrue)
bool MySolver::is_combi_vertex(Vertex piece_v, Vertex dst, double error_dist, double error_angle) {
	if (dst.is_piece) {
		//ピース角同士
		Point p = piece[piece_v.poly_id].get_point(piece_v.vertex_id);
		Point q = piece[dst.poly_id].get_point(dst.vertex_id);
		if (abs(p - q) > error_dist) return false;

		double ang_p = piece[piece_v.poly_id].get_angle_deg(piece_v.vertex_id);	
		double ang_q = piece[dst.poly_id].get_angle_deg(dst.vertex_id);

		if (360 - error_angle <= ang_p + ang_q && ang_p + ang_q <= 360 + error_angle) return true;
		if (180 - error_angle <= ang_p + ang_q && ang_p + ang_q <= 180 + error_angle) return true;
		return false;
	}
	//ピース角と枠角
	Point p = piece[piece_v.poly_id].get_point(piece_v.vertex_id);
	Point q = waku[dst.poly_id].get_point(dst.vertex_id);
	if (abs(p - q) > error_dist) return false;

	double ang_p = piece[piece_v.poly_id].get_angle_deg(piece_v.vertex_id);
	double ang_q = waku[dst.poly_id].get_angle_deg(dst.vertex_id);
	if (abs(ang_p - ang_q) > error_angle) return false;
	return true;
}

//頂点src (を持つ多角形) の結合度を計算します。寝ながら作ったので糞コードです。関数分けしたい。
//srcはピースの頂点である。
int MySolver::get_combi_level(Vertex src, double error_dist, double error_angle) {
	int i, j;
	int ret = 0;

	Point p = piece[src.poly_id].get_point(src.vertex_id);
	double ang_p = piece[src.poly_id].get_angle_deg(src.vertex_id);

	//頂点の結合度（位置と角度しか見ない)
	for (i = 0; i < waku.size(); i++) {
		int best_score = 0;

		for (j = 0; j < waku[i].size(); j++) {
			if (!is_combi_vertex(src, Vertex(false, i, j), error_dist, error_angle)) { continue; }
			
			//頂点pと頂点qがくっついている（可能性が高い）ので、ピースsrc.poly_idと枠iの結合度を本格的に計算します
			int id1 = src.vertex_id;
			int id2 = j;
			int score = 0;
			do {
				score++;
				id1 = (id1 + 1) % piece[src.poly_id].size();
				id2 = (id2 + 1) % waku[i].size();
			} while (is_combi_vertex(Vertex(true, src.poly_id, id1), Vertex(false, i, id2), error_dist, error_angle));
			if (best_score < score) {
				best_score = score;		//ピースsrc.poly_id と 枠iの結合度は、最も高評価なものとする
			}
		}
		ret += best_score;
	}

	for (i = 0; i < piece.size(); i++) {
		if (!isputted[i]) continue;
		int best_score = 0;

		for (j = 0; j < piece[i].size(); j++) {
			if (!is_combi_vertex(src, Vertex(true, i, j), error_dist, error_angle)) { continue; }

			//頂点pと頂点qがくっついている（可能性が高い）ので、ピースsrc.poly_idとピースiの結合度を本格的に計算します
			int id1 = src.vertex_id;
			int id2 = j;
			int score = 0;
			do {
				score++;
				id1 = (id1 + 1) % piece[src.poly_id].size();
				id2 = (id2 - 1 + piece[i].size()) % piece[i].size();
			} while (is_combi_vertex(Vertex(true, src.poly_id, id1), Vertex(true, i, id2), error_dist, error_angle));
			if (best_score < score) {
				best_score = score;		//ピースsrc.poly_id と ピースiの結合度は、最も高評価なものとする
			}
		}
		ret += best_score;
	}

	return ret;
}

//頂点dstと最も結合する頂点srcを返します
Vertex MySolver::get_best_vertex(Vertex dst) {
	int best_score = -1;
	Vertex src = Vertex(false, -1, 0);
	int i, j;
	double error_dist = 1;		//結合度用の許容位置誤差
	double error_angle = 1;		//結合度用の許容角度誤差
	double dist = 5;			//当たり判定のゆるさ

	for (i = 0; i < piece.size(); i++) {
		if (isputted[i]) { continue; }
		for (j = 0; j < piece[i].size(); j++) {
			//「ピースiの頂点j」をdstに合わせる
			move(Vertex(true, i, j), dst);
			//当たり判定をする
			if (isoverlap(piece[i], dist)) { continue; }
			//ピースiの結合度を計算する
			int score = get_combi_level(Vertex(true, i, j), error_dist, error_angle);
			//結合度が最大なら、最善の頂点を更新する
			if (best_score < score) {
				best_score = score;
				src = Vertex(true, i, j);
			}
		}
	}
	return src;
}

//問題を解きます
void MySolver::solve() {
	int i, j;

	//基準となる頂点集合の初期化
	for (i = 0; i < (int)waku.size(); i++) {
		for (j = 0; j < waku[i].size(); j++) {
			standard_point.push(Vertex(false, i, j));
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
		capture(*this);

		//頂点srcが存在すれば、頂点dstと頂点srcがくっつくようにする
		move(src, dst);
		//頂点srcを持つ多角形(ピース)を「位置が確定した多角形」にする
		int id = src.poly_id;
		isputted[id] = true;
		//頂点srcを持つ多角形（ピース）の全頂点を「基準となる頂点の集合」に入れる
		for (i = 0; i < piece[id].size(); i++) {
			standard_point.push(Vertex(true, id, i));
		}
		//頂点srcを持つ多角形（ピース）の全辺を「位置が確定している辺の集合」に入れる
		for (i = 0; i < piece[id].size(); i++) {
			putted_line.push_back(Line(piece[id].get_point(i), piece[id].get_point(i + 1)));
		}
	}
	capture(*this);
}