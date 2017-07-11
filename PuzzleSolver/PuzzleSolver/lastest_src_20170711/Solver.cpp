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
			if (pieces[j].size() == 0) { continue; }
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
			for (k = 0; k <= 0; k++) {
				vector<MoveNode> res = get_semiAuto_moving(is_turn_ok, false, i, j, k);
				for (int l = 0; l < res.size(); l++) { auto_moving.push_back(res[l]); }
			}
		} 
	}

	for (i = 0; i < pieces.size(); i++) {
		for (j = i + 1; j < pieces[i].size(); j++) {
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
				//score -= exp(-abs(abs(poly1.angle(i) - eval_obj.get_most_exist_angle())) / 5.0) * 1.5;		//最頻値はスコア半減
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
		Poly &src = poly1;
		Poly &dst = poly2;

		double dist_error = (is_poly2_piece) ? parameter.dist_error() : parameter.dist_error_waku();
		Poly scaledown_dst = dst.scaledown_poly( dist_error );
		if (src.is_hit(scaledown_dst) || src.is_cover(scaledown_dst)) return false;
		if (is_poly2_piece && scaledown_dst.is_cover(src)) return false;
	}

	double score = evaluation_angle(poly1, poly2) + evaluation_line(poly1, poly2);
	return score;
}


//「移動→評価→戻す」の一連動作をする関数
double Solver::action_score(MoveNode & move_node)
{
	move(move_node);

	double score;
	if (move_node.dst_is_piece) {
		score = evaluation(pieces[move_node.src_poly_id], pieces[move_node.dst_poly_id], true);
	}
	else {
		score = evaluation(pieces[move_node.src_poly_id], wakus[move_node.dst_poly_id], false);
	}
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
	Poly scaledown_dst = dst.scaledown_poly( dist_error );
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
	
	//(マージ後多角形の)最大角 + (他のピースの)最小角 > 360°なら枝刈り, ただし、最大角 > 350°なら枝刈りしない。
	/*for (int i = 0; i < poly.size(); i++) {
		double angle = poly.angle(i);
		if (angle > 350) { continue; }
		if (angle + eval_obj.get_min_angle() > 360) { return false; }
	}*/

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

	pair<bool, vector<Poly>> res = marge_poly_obj.marge_poly(dist_error, src, dst, dst_is_piece);
	if (res.first == false) return false;

	vector<Poly> marged_polys = res.second;

	lastestMargedPolyIds.clear();

	if (dst_is_piece) {
		for (int i = 0; i < marged_polys.size(); i++) {
pieces.push_back(marged_polys[i]);
lastestMargedPolyIds.push_back((int)pieces.size() - 1);
		}
		pieces[src_poly_id].points.clear();
		pieces[dst_poly_id].points.clear();

	}
	else {
		for (int i = 0; i < marged_polys.size(); i++) {
			wakus.push_back(marged_polys[i]);
			lastestMargedPolyIds.push_back(-((int)wakus.size() - 1));
		}
		pieces[src_poly_id].points.clear();
		wakus[dst_poly_id].points.clear(); wakus[dst_poly_id].lines.clear();
	}
	return true;
}

//自動でくっつける
vector<MoveNode> Solver::connect_auto(bool is_turn_ok, int max_moveList_size)
{
	int i, j, k;	//多角形番号, 辺番号, 辺の始点同士をくっつけるか終点同士をくっつけるか（is_reverse_line）
	map<DstType, vector<MoveNode> >::iterator it;

	eval_obj.make_histgram(wakus, pieces);
	//scores.clear();

	//新規
	if (lastestMargedPolyIds.size() == 0) {
		for (i = 0; i < wakus.size(); i++) {
			for (j = 0; j < wakus[i].size(); j++) {
				vector<MoveNode> res = connect_semiAuto(is_turn_ok, false, i, j, false, max_moveList_size);
				DstType type = DstType(is_turn_ok, false, i, j, false);
				for (int m = 0; m < res.size(); m++) { scores[type].push_back(res[m]); }

				res = connect_semiAuto(is_turn_ok, false, i, j, true, max_moveList_size);
				type = DstType(is_turn_ok, false, i, j, true);
				for (int m = 0; m < res.size(); m++) { scores[type].push_back(res[m]); }
			}
		}

		for (i = 0; i < pieces.size(); i++) {
			for (j = 0; j < pieces[i].size(); j++) {
				vector<MoveNode> res = connect_semiAuto(is_turn_ok, true, i, j, false, max_moveList_size);
				DstType type = DstType(is_turn_ok, true, i, j, false);
				for (int m = 0; m < res.size(); m++) { scores[type].push_back(res[m]); }

				res = connect_semiAuto(is_turn_ok, true, i, j, true, max_moveList_size);
				type = DstType(is_turn_ok, true, i, j, true);
				for (int m = 0; m < res.size(); m++) { scores[type].push_back(res[m]); }
			}
		}
	}

	//新たにできた枠穴たちとの組み合わせを試す
	else if (lastestMargedPolyIds[0] < 0) {
		for (i = 0; i < lastestMargedPolyIds.size(); i++) {
			int id = -lastestMargedPolyIds[i];
			for (j = 0; j < wakus[id].size(); j++) {
				vector<MoveNode> res = connect_semiAuto(is_turn_ok, false, id, j, false, max_moveList_size);
				DstType type = DstType(is_turn_ok, false, id, j, false);
				for (int m = 0; m < res.size(); m++) { scores[type].push_back(res[m]); }

				res = connect_semiAuto(is_turn_ok, false, id, j, true, max_moveList_size);
				type = DstType(is_turn_ok, false, id, j, true);
				for (int m = 0; m < res.size(); m++) { scores[type].push_back(res[m]); }
			}
		}
	}

	//新たにできたピースとの組み合わせを試す
	else {
		int id = lastestMargedPolyIds[0];
		for (j = 0; j < pieces[id].size(); j++) {
			vector<MoveNode> res = connect_semiAuto(is_turn_ok, true, id, j, false, max_moveList_size);
			DstType type = DstType(is_turn_ok, true, id, j, false);
			for (int m = 0; m < res.size(); m++) { scores[type].push_back(res[m]); }

			res = connect_semiAuto(is_turn_ok, true, id, j, true, max_moveList_size);
			type = DstType(is_turn_ok, true, id, j, true);
			for (int m = 0; m < res.size(); m++) { scores[type].push_back(res[m]); }
		}
	}

	//表の更新
	map<DstType, vector<MoveNode>> newScores;
	for (it = scores.begin(); it != scores.end(); it++) {
		vector<MoveNode> &table = (*it).second;
		vector<MoveNode> newTable;
		for (i = 0; i < table.size(); i++) {
			if (table[i].dst_is_piece) { if (pieces[table[i].dst_poly_id].points.size() == 0) continue; }
			else { if (wakus[table[i].dst_poly_id].points.size() == 0) continue; }
			if (pieces[table[i].src_poly_id].points.size() == 0) continue;
			newTable.push_back(table[i]);
		}
		if (newTable.size() > 0) {
			newScores.insert(pair<DstType, vector<MoveNode>>(it->first, newTable));
		}
	}
	scores = newScores;


	//選別
	double max_score = -114514;
	for (it = scores.begin(); it != scores.end(); it++) {
		vector<MoveNode> &table = (*it).second;
		for (i = 0; i < table.size(); i++) {
			max_score = max(max_score, table[i].score);
		}
	}

	//「全評価値の最大値」を持つくっつけ方だけが候補. この中で「ユニークなもの」を評価する
	typedef pair<int, DstType> P;	//max_scoreになってるものの個数, dst辺
	vector<P> dst_list;

	for (it = scores.begin(); it != scores.end(); it++) {
		vector<MoveNode> &table = (*it).second;

		int max_score_cnt = 0;

		for (i = 0; i < table.size(); i++) {
			if (table[i].score < max_score) {
				continue;
			}
			max_score_cnt++;
		}
		if (max_score_cnt == 0) { continue; }

		dst_list.push_back(P(max_score_cnt, (*it).first));
	}

	if (dst_list.size() == 0) { return vector<MoveNode>(); }
	sort(dst_list.begin(), dst_list.end());

	//printfDx("最高評価 = %d [通り]\n", dst_list.size());

	DstType best_dst = dst_list[0].second;	//この辺にくっつく辺を探すとよい
	vector<MoveNode> table = (*scores.find(best_dst)).second;
	sort(table.begin(), table.end());		//score順にソート

	vector<MoveNode> res;
	for (i = 0; i < min(table.size(), max_moveList_size); i++) {
		res.push_back(table[i]);
	}
	return res;
}


//半自動でくっつける (辺と辺のくっつけ方の「評価値」も返す）
vector<MoveNode> Solver::connect_semiAuto(bool is_turn_ok, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line, int max_moveList_size)
{
	vector<MoveNode> res = get_semiAuto_moving(is_turn_ok, dst_is_piece, dst_poly_id, dst_line_id, is_reverse_line);
	vector<pair<double, MoveNode> > move_list;

	eval_obj.make_histgram(wakus, pieces);

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
		if (can_marge(move_node.src_poly_id, move_node.dst_poly_id, move_node.dst_is_piece)) {
			ret.push_back(move_node);
			ret.rbegin()->score = move_list[i].first;		//評価値の情報も入れる
		}
		restore();
	}
	
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
