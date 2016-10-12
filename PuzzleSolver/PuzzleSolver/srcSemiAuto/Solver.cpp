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
void Solver::set_auto_moving(bool is_turn_ok)
{
	int i, j, k;

	auto_moving.clear();

	for (i = 0; i < wakus.size(); i++) {
		for (j = 0; j < wakus[i].size(); j++) {
			for (k = 0; k <= 1; k++) {
				vector<MoveNode> res = get_semiAuto_moving(is_turn_ok, false, i, j, k);
				for (int l = 0; l < res.size(); l++) { auto_moving.push_back(res[l]); }
			}
		} 
	}

	for (i = 0; i < pieces.size(); i++) {
		for (j = 0; j < pieces[i].size(); j++) {
			vector<MoveNode> res = get_semiAuto_moving(is_turn_ok, true, i, j, k);
			for (int l = 0; l < res.size(); l++) { auto_moving.push_back(res[l]); }
		}
	}
	return;
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


double Solver::evaluation_line(Poly &poly1, Poly &poly2) {
	SolverParameter &param = parameter; 
	
	double score = 0;

	vector<double> angle_error_poly1(poly1.size());
	vector<double> angle_error_poly2(poly2.size());
	double dist_error = param.dist_error();

	for (int i = 0; i < poly1.size(); i++) {
		angle_error_poly1[i] = param.angle_error(abs(poly1.points[i + 1] - poly1.points[i]));
	}
	for (int i = 0; i < poly2.size(); i++) {
		angle_error_poly2[i] = param.angle_error(abs(poly2.points[i + 1] - poly2.points[i]));
	}

	//辺の評価
	for (int i = 0; i < poly1.size(); i++) {
		Line line1 = Line(poly1.points[i], poly1.points[i + 1]);
		for (int j = 0; j < poly2.size(); j++) {
			Line line2 = Line(poly2.points[j], poly2.points[j + 1]);
			double angle_error = angle_error_poly1[i] + angle_error_poly2[j];
			angle_error /= 1.41421356;

			double res = line1.match_score(line2, dist_error, angle_error);
			if (res == 4) { score += 4; }
			if (res == 2) { score += 2; }	//180°
			if (res == 1) { score += 1; }
		}
	}
	return score;
}

//角度の評価
double Solver::evaluation_angle(Poly & poly1, Poly & poly2)
{
	SolverParameter &param = parameter;

	vector<double> angle_error_poly1(poly1.size());
	vector<double> angle_error_poly2(poly2.size());
	double dist_error = param.dist_error();

	for (int i = 0; i < poly1.size(); i++) {
		angle_error_poly1[i] = param.angle_error(abs(poly1.points[i + 1] - poly1.points[i]));
	}
	for (int i = 0; i < poly2.size(); i++) {
		angle_error_poly2[i] = param.angle_error(abs(poly2.points[i + 1] - poly2.points[i]));
	}

	double score = 0;

	//角の評価（360°)
	for (int i = 0; i < poly1.size(); i++) {
		for (int j = 0; j < poly2.size(); j++) {
			//角poly1.points[i], 角poly2.points[j]が一致していればスコア加算
			if (norm(poly1.points[i] - poly2.points[j]) > dist_error * dist_error) continue;
		
			int i_1 = i - 1; if (i_1 < 0) i_1 += poly1.size();
			int j_1 = j - 1; if (j_1 < 0) j_1 += poly2.size();
			
			double angle_error = angle_error_poly1[i_1]
				+ angle_error_poly1[i]
			+ angle_error_poly2[j_1]
			+ angle_error_poly2[j];
			angle_error /= 2;

			if (angle_error > 10) { continue; }		//10度もずれてたら信用ならない！
			
			double angle = poly1.angle(i) + poly2.angle(j);
			double error = abs(360 - angle);
			if (error <= angle_error) {
				score += 3;
				score -= exp(-abs(abs(poly1.angle(i) - 90)) / 5.0) * 1.5;		//最頻値(90°)はスコア半減
			}
		}
	}

	return score;
}


//評価関数 (移植しただけ）
double Solver::evaluation(Poly & poly1, Poly & poly2, bool is_poly2_piece)
{
	SolverParameter &param = parameter;

	//当たり判定
	{
		double dist_error = (is_poly2_piece) ? param.dist_error() : param.dist_error_waku();
		Poly dst = scaledown_poly(poly2, dist_error);
		if (dst.is_hit(poly1)) { return -114514; }
		if (poly1.is_cover(dst) && !dst.is_cover(poly1)) { return -114514; }
	}
	return evaluation_angle(poly1, poly2) + evaluation_line(poly1, poly2);
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

	double dist_error = (dst_is_piece) ? parameter.dist_error() : parameter.dist_error_waku();
	Poly scaledown_dst = scaledown_poly(dst, dist_error);
	if (src.is_hit(scaledown_dst) || src.is_cover(scaledown_dst)) return false;
	if (dst_is_piece && scaledown_dst.is_cover(src)) return false;
	if (!marge_poly_obj.can_marge(dist_error, src, dst, dst_is_piece, wakus)) return false;

	pair<bool, vector<Poly>> res = marge_poly_obj.marge_poly(dist_error, src, dst, dst_is_piece);
	if (res.first == false) return false;

	double max_area = 0;
	Poly poly = res.second[0];	//マージ後の多角形
	for (int i = 0; i < res.second.size(); i++) {
		if (max_area < abs(res.second[i].area())) {
			max_area = abs(res.second[i].area());
			poly = res.second[i];
		}
	}

	//枝刈り
	if (dst_is_piece) {
		//辺の長さのMaxで枝刈り
		double max_waku_length = 0;
		int i, j;

		for (i = 0; i < wakus.size(); i++) {
			for (j = 0; j < wakus[i].size(); j++) {
				max_waku_length = max(max_waku_length, abs(wakus[i].points[j + 1] - wakus[i].points[j]));
			}
		}
		
		for (i = 0; i < poly.size(); i++) {
			Point point = poly.points[i];
			double max_dist = 0;
			for (j = 0; j < poly.size(); j++) {
				max_dist = max(max_dist, Line(poly.points[j], poly.points[j + 1]).straight_dist(point));
			}
			if (max_dist > max_waku_length) { return false; }
		}
	}
	
	//(マージ後多角形の)最大角 + (他のピースの)最小角 > 360°なら枝刈り, ただし、最大角 > 350°なら枝刈りしない。
	for (int i = 0; i < poly.size(); i++) {
		double angle = poly.angle(i);
		if (angle > 350) { continue; }
		if (angle + eval_obj.get_min_angle() > 360) { return false; }
	}

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

	double dist_error = (dst_is_piece) ? parameter.dist_error() : parameter.dist_error_waku();
	Poly scaledown_dst = scaledown_poly(dst, dist_error);
	if (src.is_hit(scaledown_dst) || src.is_cover(scaledown_dst)) return false;
	if (dst_is_piece && scaledown_dst.is_cover(src)) return false;
	if (!marge_poly_obj.can_marge(dist_error, src, dst, dst_is_piece, wakus)) return false;

	pair<bool, vector<Poly>> res = marge_poly_obj.marge_poly(dist_error, src, dst, dst_is_piece);
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
	set_auto_moving(is_turn_ok);
	vector<MoveNode> &res = auto_moving;
	vector<pair<double, MoveNode> > move_list;

	eval_obj.make_histgram(pieces);

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
		if (!can_marge(move_node.src_poly_id, move_node.dst_poly_id, move_node.dst_is_piece)) { continue; }
		ret.push_back(move_node);
		restore();
	}

	//printfDx("score = %.1f\n", move_list[0].first);
	//for (int angle = 0; angle <= 360; angle += 5) printfDx("%.1f ", eval_obj.get_percent_angle(angle, 5));
	return ret;
}


//半自動でくっつける
vector<MoveNode> Solver::connect_semiAuto(bool is_turn_ok, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line, int max_moveList_size)
{
	vector<MoveNode> res = get_semiAuto_moving(is_turn_ok, dst_is_piece, dst_poly_id, dst_line_id, is_reverse_line);
	vector<pair<double, MoveNode> > move_list;

	eval_obj.make_histgram(pieces);

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
		restore();
	}
	//printfDx("候補数 = %d\n", (int)ret.size());

	//printfDx("score = %.1f\n", move_list[0].first);
	return ret;
}


//手動でくっつける, くっつけられないときはfalseを返すが、ピースは移動している
bool Solver::connect_hand(MoveNode move_node)
{
	move(move_node);

	//Poly src = pieces[move_node.src_poly_id];
	//Poly dst = (move_node.dst_is_piece) ? pieces[move_node.dst_poly_id] : wakus[move_node.dst_poly_id];

	//printfDx("score = %.1f\n", evaluation(src, dst, move_node.dst_is_piece));

	if (!marge_poly(move_node.src_poly_id, move_node.dst_poly_id, move_node.dst_is_piece)) { return false; }
	return true;
}
