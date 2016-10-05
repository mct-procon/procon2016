//参照型(Poly&など)の仕様…ポインタ変数に*を付けたものと同じです。別名を付ける～という感覚でOK.

#include "Solver.h"
#include "Extern.h"

void capture();			//Main側の関数をプロトタイプ


//ピースsrc_poly_idを動かす (辺srcを辺dstにくっつけるイメージ)
//reverse…dst辺をひっくり返すかどうか？
void Solver::move(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line) {
	Poly &src = pieces[src_poly_id];
	Poly &dst = (dst_is_piece ? pieces[dst_poly_id] : wakus[dst_poly_id]);

	backup_piece = src;
	moved_piece_id = src_poly_id;

	if (src.is_turn != src_turnflag) { src.turn(); }

	if (is_reverse_line) {
		src.move_reverse(src_line_id, dst.points[dst_line_id], dst.points[dst_line_id + 1]);	//終点を合わせる
	}
	else {
		src.move(src_line_id, dst.points[dst_line_id + 1], dst.points[dst_line_id]);
	}
}


//ピースを元の位置に戻す
void Solver::restore()
{
	pieces[moved_piece_id] = backup_piece;
}


//評価関数
double Solver::evaluation(Poly & poly1, Poly & poly2, bool is_poly2_piece)
{

	double dist_error = (is_poly2_piece) ? dist : 2 * dist;

	if (poly2.is_hit(scaledown_poly(poly1, dist_error))) { return -114514; }
	if (poly1.is_cover(scaledown_poly(poly2, dist_error)) && !poly2.is_cover(scaledown_poly(poly1, dist_error))) { return -114514; }

	double score = 0;

	//辺の評価
	for (int i = 0; i < poly1.size(); i++) {
		for (int j = 0; j < poly2.size(); j++) {
			Line line1 = Line(poly1.points[i], poly1.points[i + 1]);
			Line line2 = Line(poly2.points[j], poly2.points[j + 1]);
			score += line1.machi_score(line2, dist_error, angle_error_deg);
		}
	}

	//角の評価（360°)
	for (int i = 0; i < poly1.size(); i++) {
		for (int j = 0; j < poly2.size(); j++) {
			//角poly1.points[i], 角poly2.points[j]が一致していればスコア加算
			if (abs(poly1.points[i] - poly2.points[j]) > dist) continue;
			Line line1a = Line(poly1.points[i], poly1.points[i + 1]);
			Line line1b = Line(poly1.points[i], poly1.points[(i - 1 + poly1.size()) % poly1.size()]);
			Line line2b = Line(poly2.points[j], poly2.points[j + 1]);
			Line line2a = Line(poly2.points[j], poly2.points[(j - 1 + poly2.size()) % poly2.size()]);
			double score_a = line1a.machi_score(line2a, dist_error, angle_error_deg); if (!(score_a == 1 || score_a == 4)) continue;	//向きが一致しているかを判定
			double score_b = line1b.machi_score(line2b, dist_error, angle_error_deg); if (!(score_b == 1 || score_b == 4)) continue;	//向きが一致しているかを判定
			score += 3;
			break;
		}
	}
	return score;
}


//「移動→評価→戻す」の一連動作をする関数
double Solver::action_score(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line)
{
	move(src_turnflag, src_poly_id, src_line_id, dst_is_piece, dst_poly_id, dst_line_id, is_reverse_line);

	double score;
	if (dst_is_piece) score = evaluation(pieces[src_poly_id], pieces[dst_poly_id], true);
	else score = evaluation(pieces[src_poly_id], wakus[dst_poly_id], false);

	restore();
	return score;
}


//2つの多角形 (dst_is_piece == trueの場合は、ピース[src_poly_id]とピース[dst_poly_id]）をくっつける
//成功時はtrueを返す, 失敗時はfalseを返す
bool Solver::marge_poly(int src_poly_id, int dst_poly_id, bool dst_is_piece)
{
	//多角形のマージ
	if (!is_do_marge) return false;

	Poly &src = pieces[src_poly_id];
	Poly &dst = (dst_is_piece ? pieces[dst_poly_id] : wakus[dst_poly_id]);

	Poly scaledown_dst = scaledown_poly(dst, dist);
	if (src.is_hit(scaledown_dst) || src.is_cover(scaledown_dst)) return false;
	if (dst_is_piece && scaledown_dst.is_cover(src)) return false;
	if (!marge_poly_obj.can_marge(dist, angle_error_deg, src, dst, dst_is_piece, wakus)) return false;

	pair<bool, vector<Poly>> res = marge_poly_obj.marge_poly(dist, angle_error_deg, src, dst, dst_is_piece);
	if (res.first == false) return false;

	vector<Poly> marged_polys = res.second;
	if (dst_is_piece) {
		for (int i = 0; i < marged_polys.size(); i++) pieces.push_back(marged_polys[i]);
		pieces[src_poly_id].points.clear();
		pieces[dst_poly_id].points.clear();
	}
	else {
		for (int i = 0; i < marged_polys.size(); i++) wakus.push_back(marged_polys[i]);
		pieces[src_poly_id].points.clear();
		wakus[dst_poly_id].points.clear(); wakus[dst_poly_id].lines.clear();
	}
	return true;
}


//ベストな組でくっつける, くっつく組み合わせがないとき0を返す
bool Solver::connect()
{
	int turnflag, i, j, k, l;
	typedef tuple<double, bool, int, int, bool, int, int, bool> T;
	vector<T> score_move;
	double score;

	for (i = 0; i < pieces.size(); i++) {
		for (turnflag = 0; turnflag <= 1; turnflag++) {
			for (j = 0; j < pieces[i].size(); j++) {
				double length_error = dist;
				double src_edge_length = abs(pieces[i].points[j + 1] - pieces[i].points[j]);

				for (k = 0; k < wakus.size(); k++) {
					for (l = 0; l < wakus[k].size(); l++) {
						if (abs(src_edge_length - abs(wakus[k].points[l + 1] - wakus[k].points[l])) > length_error) { continue; }	//くっつける辺の長さが等しくなければ, やり直し
						score = action_score((turnflag == 1), i, j, false, k, l, false); if (score > 0) { score_move.push_back(T(score, (turnflag == 1), i, j, false, k, l, false)); }
						score = action_score((turnflag == 1), i, j, false, k, l, true); if (score > 0) { score_move.push_back(T(score, (turnflag == 1), i, j, false, k, l, true)); }
					}
				}
				for (k = 0; k < pieces.size(); k++) {
					if (k == i) continue;
					for (l = 0; l < pieces[k].size(); l++) {
						if (abs(src_edge_length - abs(pieces[k].points[l + 1] - pieces[k].points[l])) > length_error) { continue; }	//くっつける辺の長さが等しくなければ, やり直し
						score = action_score((turnflag == 1), i, j, true, k, l, false); if (score > 0) { score_move.push_back(T(score, (turnflag == 1), i, j, true, k, l, false)); }
						score = action_score((turnflag == 1), i, j, true, k, l, true); if (score > 0) { score_move.push_back(T(score, (turnflag == 1), i, j, true, k, l, true)); }
					}
				}
			}
		}
	}

	if (score_move.size() == 0) {
		for (i = 0; i < pieces.size(); i++) {
			for (turnflag = 0; turnflag <= 1; turnflag++) {
				for (j = 0; j < pieces[i].size(); j++) {
					for (k = 0; k < wakus.size(); k++) {
						for (l = 0; l < wakus[k].size(); l++) {
							score = action_score((turnflag == 1), i, j, false, k, l, false); if (score > 0) { score_move.push_back(T(score, (turnflag == 1), i, j, false, k, l, false)); }
							score = action_score((turnflag == 1), i, j, false, k, l, true); if (score > 0) { score_move.push_back(T(score, (turnflag == 1), i, j, false, k, l, true)); }
						}
					}
					for (k = 0; k < pieces.size(); k++) {
						if (k == i) continue;
						for (l = 0; l < pieces[k].size(); l++) {
							score = action_score((turnflag == 1), i, j, true, k, l, false); if (score > 0) { score_move.push_back(T(score, (turnflag == 1), i, j, true, k, l, false)); }
							score = action_score((turnflag == 1), i, j, true, k, l, true); if (score > 0) { score_move.push_back(T(score, (turnflag == 1), i, j, true, k, l, true)); }
						}
					}
				}
			}
		}
	}

	//一番スコアが高い組み合わせでくっつける
	if (score_move.size() == 0) return false;
	sort(score_move.begin(), score_move.end(), greater<T>());

	for (i = 0; i < score_move.size(); i++) {
		move(get<1>(score_move[i]), get<2>(score_move[i]), get<3>(score_move[i]), get<4>(score_move[i]), get<5>(score_move[i]), get<6>(score_move[i]), false);

		//多角形のマージ
		int src_poly_id = get<2>(score_move[i]);
		int dst_poly_id = get<5>(score_move[i]);
		bool dst_is_piece = get<4>(score_move[i]);
		bool is_reverse = get<7>(score_move[i]);

		bool is_success = marge_poly(src_poly_id, dst_poly_id, dst_is_piece);

		if (!is_success) { restore(); continue; }
		else { return true; }			//score_move[i]の移動方法で移動した（で、マージもした）。
	}
	return false;
}

//問題を解く
void Solver::solve()
{
	capture();
	while (connect()) {
		capture();
	}
}

