//参照型(Poly&など)の仕様…ポインタ変数に*を付けたものと同じです。別名を付ける～という感覚でOK.

#include "Solver.h"
#include "Extern.h"


/*２つの多角形のくっつけ方を返す*/
//手動
MoveNode Solver::get_hand_moving(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line)
{
	return MoveNode(src_turnflag, src_poly_id, src_line_id, dst_is_piece, dst_poly_id, dst_line_id, is_reverse_line);
}

//半自動
vector<MoveNode> Solver::get_semiAuto_moving(bool is_turn_ok, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line)
{
	int i, j, k;
	vector<MoveNode> ret;

	for (i = 0; i <= (is_turn_ok); i++) {
		for (j = 0; j < pieces.size(); j++) {
			for (k = 0; k < pieces[j].size(); k++) {
				ret.push_back(MoveNode(i, j, k, dst_is_piece, dst_poly_id, dst_line_id, is_reverse_line));
			}
		}
	}
	return ret;
}

//自動
vector<MoveNode> Solver::get_auto_moving(bool is_turn_ok)
{
	int i, j, k;
	vector<MoveNode> ret;

	for (i = 0; i < wakus.size(); i++) {
		for (j = 0; j < wakus[i].size(); j++) {
			for (k = 0; k <= 1; k++) {
				vector<MoveNode> res = get_semiAuto_moving(is_turn_ok, false, i, j, k);
				for (int l = 0; l < res.size(); l++) { ret.push_back(res[l]); }
			}
		} 
	}

	for (i = 0; i < pieces.size(); i++) {
		for (j = 0; j < pieces[i].size(); j++) {
			for (k = 0; k <= 1; k++) {
				vector<MoveNode> res = get_semiAuto_moving(is_turn_ok, true, i, j, k);
				for (int l = 0; l < res.size(); l++) { ret.push_back(res[l]); }
			}
		}
	}
	return ret;
}


//ピースsrc_poly_idを動かす (辺srcを辺dstにくっつけるイメージ)
//reverse…dst辺をひっくり返すかどうか？
void Solver::move(MoveNode & move_node)
{
	Poly &src = pieces[move_node.src_poly_id];
	Poly &dst = (move_node.dst_is_piece ? pieces[move_node.dst_poly_id] : wakus[move_node.dst_poly_id]);

	backup_piece = src;
	moved_piece_id = move_node.src_poly_id;

	if (src.is_turn != move_node.src_turnflag) { src.turn(); }

	if (move_node.is_reverse_line) {
		src.move_reverse(move_node.src_line_id, dst.points[move_node.dst_line_id], dst.points[move_node.dst_line_id + 1]);	//終点を合わせる
	}
	else {
		src.move(move_node.src_line_id, dst.points[move_node.dst_line_id + 1], dst.points[move_node.dst_line_id]);
	}
}


//ピースを元の位置に戻す
void Solver::restore()
{
	pieces[moved_piece_id] = backup_piece;
}


//評価関数 (移植しただけ）
double Solver::evaluation(Poly & poly1, Poly & poly2, bool is_poly2_piece)
{
	SolverParameter &param = parameter;

	//当たり判定
	{
		double dist_error = (is_poly2_piece) ? param.dist_error : 2 * param.dist_error_waku;
		Poly dst = scaledown_poly(poly2, dist_error);
		if (dst.is_hit(poly1)) { return -114514; }
		if (poly1.is_cover(dst) && !dst.is_cover(poly1)) { return -114514; }
	}

	double score = 0;

	//辺の評価
	for (int i = 0; i < poly1.size(); i++) {
		for (int j = 0; j < poly2.size(); j++) {
			Line line1 = Line(poly1.points[i], poly1.points[i + 1]);
			Line line2 = Line(poly2.points[j], poly2.points[j + 1]);
			score += line1.machi_score(line2, param.dist_error, param.angle_error);
		}
	}

	//角の評価（360°)
	for (int i = 0; i < poly1.size(); i++) {
		for (int j = 0; j < poly2.size(); j++) {
			//角poly1.points[i], 角poly2.points[j]が一致していればスコア加算
			if (abs(poly1.points[i] - poly2.points[j]) > param.dist_error) continue;
			Line line1a = Line(poly1.points[i], poly1.points[i + 1]);
			Line line1b = Line(poly1.points[i], poly1.points[(i - 1 + poly1.size()) % poly1.size()]);
			Line line2b = Line(poly2.points[j], poly2.points[j + 1]);
			Line line2a = Line(poly2.points[j], poly2.points[(j - 1 + poly2.size()) % poly2.size()]);
			double score_a = line1a.machi_score(line2a, param.dist_error, param.angle_error); if (!(score_a == 1 || score_a == 4)) continue;	//向きが一致しているかを判定
			double score_b = line1b.machi_score(line2b, param.dist_error, param.angle_error); if (!(score_b == 1 || score_b == 4)) continue;	//向きが一致しているかを判定
			score += 3;
			break;
		}
	}
	return score;
}


//「移動→評価→戻す」の一連動作をする関数
double Solver::action_score(MoveNode & move_node)
{
	move(move_node);

	double score;
	if (move_node.dst_is_piece) score = evaluation(pieces[move_node.src_poly_id], pieces[move_node.dst_poly_id], true);
	else score = evaluation(pieces[move_node.src_poly_id], wakus[move_node.dst_poly_id], false);

	restore();
	return score;
}


//2つの多角形をくっつけられるかだけ判定する
bool Solver::can_marge(int src_poly_id, int dst_poly_id, bool dst_is_piece)
{
	if (!is_do_marge) return false;

	Poly &src = pieces[src_poly_id];
	Poly &dst = (dst_is_piece ? pieces[dst_poly_id] : wakus[dst_poly_id]);

	double dist_error = (dst_is_piece) ? parameter.dist_error : parameter.dist_error_waku;
	Poly scaledown_dst = scaledown_poly(dst, dist_error);
	if (src.is_hit(scaledown_dst) || src.is_cover(scaledown_dst)) return false;
	if (dst_is_piece && scaledown_dst.is_cover(src)) return false;
	if (!marge_poly_obj.can_marge(dist_error, parameter.angle_error, src, dst, dst_is_piece, wakus)) return false;

	pair<bool, vector<Poly>> res = marge_poly_obj.marge_poly(dist_error, parameter.angle_error, src, dst, dst_is_piece);
	if (res.first == false) return false;
	
	return true;
}


//2つの多角形 (dst_is_piece == trueの場合は、ピース[src_poly_id]とピース[dst_poly_id]）をくっつける
//成功時はtrueを返す, 失敗時はfalseを返す
bool Solver::marge_poly(int src_poly_id, int dst_poly_id, bool dst_is_piece)
{
	//多角形のマージ
	if (!is_do_marge) return false;

	Poly &src = pieces[src_poly_id];
	Poly &dst = (dst_is_piece ? pieces[dst_poly_id] : wakus[dst_poly_id]);

	double dist_error = (dst_is_piece) ? parameter.dist_error : parameter.dist_error_waku;
	Poly scaledown_dst = scaledown_poly(dst, dist_error);
	if (src.is_hit(scaledown_dst) || src.is_cover(scaledown_dst)) return false;
	if (dst_is_piece && scaledown_dst.is_cover(src)) return false;
	if (!marge_poly_obj.can_marge(dist_error, parameter.angle_error, src, dst, dst_is_piece, wakus)) return false;

	pair<bool, vector<Poly>> res = marge_poly_obj.marge_poly(dist_error, parameter.angle_error, src, dst, dst_is_piece);
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


//自動でくっつける
vector<MoveNode> Solver::connect_auto(bool is_turn_ok, int max_moveList_size)
{
	vector<MoveNode> res = get_auto_moving(is_turn_ok);
	vector<pair<double, MoveNode> > move_list;

	for (int i = 0; i < res.size(); i++) {
		MoveNode move_node = res[i];
		double score = action_score(move_node);
		if (score > 0) {
			move_list.push_back(pair<double, MoveNode>(score, move_node));
		}
	}
	sort(move_list.begin(), move_list.end(), greater<pair<double, MoveNode>>());
	
	//マージ処理をしていって、大丈夫なものを上位から(高々max_moveList_size個)取る
	vector<MoveNode> ret;
	for (int i = 0; i < move_list.size(); i++) {
		if (ret.size() >= max_moveList_size) break;
		
		MoveNode move_node = move_list[i].second;
		move(move_node);
		if (!can_marge(move_node.src_poly_id, move_node.dst_poly_id, move_node.dst_is_piece)) { restore(); continue; }
		ret.push_back(move_node);
	}
	return ret;
}


//半自動でくっつける
vector<MoveNode> Solver::connect_semiAuto(bool is_turn_ok, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line, int max_moveList_size)
{
	vector<MoveNode> res = get_semiAuto_moving(is_turn_ok, dst_is_piece, dst_poly_id, dst_line_id, is_reverse_line);
	vector<pair<double, MoveNode> > move_list;

	for (int i = 0; i < res.size(); i++) {
		MoveNode move_node = res[i];
		double score = action_score(move_node);
		if (score > 0) {
			move_list.push_back(pair<double, MoveNode>(score, move_node));
		}
	}
	sort(move_list.begin(), move_list.end(), greater<pair<double, MoveNode>>());

	//マージ処理 (副作用なし) をしてみて、大丈夫なものを上位から(高々max_moveList_size個)取る
	vector<MoveNode> ret;
	for (int i = 0; i < move_list.size(); i++) {
		if (ret.size() >= max_moveList_size) break;

		MoveNode move_node = move_list[i].second;
		move(move_node);
		if (!can_marge(move_node.src_poly_id, move_node.dst_poly_id, move_node.dst_is_piece)) { restore(); continue; }
		ret.push_back(move_node);
	}
	return ret;
}


//手動でくっつける, くっつけられないときはfalseを返すが、ピースは移動している
bool Solver::connect_hand(MoveNode move_node)
{
	move(move_node);
	if (!marge_poly(move_node.src_poly_id, move_node.dst_poly_id, move_node.dst_is_piece)) { return false; }
	return true;
}
