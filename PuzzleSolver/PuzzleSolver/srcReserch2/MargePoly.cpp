//方針としては…マージ後の図形が（Re軸を→, Im軸を↑に取った時に）反時計回りの頂点列を持つようにする。枠穴が増える場合にも対応する。
//グラフ構築で頭いい条件で辺をはると、面積比較無しで多角形のマージが成功するので、なんかうれしい。

#include "MargePoly.h"
#include "DxLib.h"


//dst_polyの辺や頂点に合わせてsrc_polyを変形する -> その後、src_polyに合わせてdst_polyを変形する
void MargePoly::point_marge(Poly &src_poly, Poly &dst_poly) {
	int i, j;

	for (i = 0; i < src_poly.size(); i++) {
		Point &point = src_poly.points[i];
		
		//dst_polyの頂点に近ければ、そいつの座標にする
		for (j = 0; j < dst_poly.size(); j++) {
			if (abs(point - dst_poly.points[j]) <= dist_error) {
				point = dst_poly.points[j];
				break;
			}
		}
		if (j < dst_poly.size()) { continue; }

		//dst_polyの辺に近ければ、src_poly.{points[i] -> points[i+1]}の交点, src_poly.{points[i-1] -> points[i]}との交点のうち、近い方の座標にする
		for (j = 0; j < dst_poly.size(); j++) {
			Line line = Line(dst_poly.points[j], dst_poly.points[j + 1]);
			if (line.dist(point) <= dist_error) {
				
				Line line1 = Line(src_poly.points[(i + src_poly.size() - 1) % src_poly.size()], src_poly.points[i]);
				Line line2 = Line(src_poly.points[i], src_poly.points[i + 1]);
				Point p1 = line.cross_point(line1);
				Point p2 = line.cross_point(line2);

				if (abs(p1 - point) < abs(p2 - point)) { point = p1; }
				else { point = p2; }
				break;
			}
		}
	}
	src_poly.points[src_poly.size()] = src_poly.points[0];

	for (i = 0; i < dst_poly.size(); i++) {
		Point &point = dst_poly.points[i];

		//src_polyの頂点に近ければ、そいつの座標にする
		for (j = 0; j < src_poly.size(); j++) {
			if (abs(point - src_poly.points[j]) <= dist_error) {
				point = src_poly.points[j];
				break;
			}
		}
		if (j < src_poly.size()) { continue; }

		//src_polyの辺に近ければ、dst_poly.{points[i] -> points[i + 1]}の交点, dst_poly.{points[i - 1] -> points[i]}との交点の内、近いほうの座標にする
		for (j = 0; j < src_poly.size(); j++) {
			Line line = Line(src_poly.points[j], src_poly.points[j + 1]);
			if (line.dist(point) <= dist_error) {

				Line line1 = Line(dst_poly.points[(i + dst_poly.size() - 1) % dst_poly.size()], dst_poly.points[i]);
				Line line2 = Line(dst_poly.points[i], dst_poly.points[i + 1]);
				Point p1 = line.cross_point(line1);
				Point p2 = line.cross_point(line2);

				if (abs(p1 - point) < abs(p2 - point)) { point = p1; }
				else { point = p2; }
				break;
			}
		}
	}
	dst_poly.points[dst_poly.size()] = dst_poly.points[0];
}


//points, linesの設定 (uniqueするだけ)
void MargePoly::make_group(Poly &src_poly, Poly &dst_poly) {
	int i, j;

	points.clear();
	for (i = 0; i < src_poly.size(); i++) {
		for (j = 0; j < points.size(); j++) { if (abs(points[j] - src_poly.points[i]) <= 1) break; }
		if (j == points.size()) { points.push_back(src_poly.points[i]); }
	}
	for (i = 0; i < dst_poly.size(); i++) {
		for (j = 0; j < points.size(); j++) { if (abs(points[j] - dst_poly.points[i]) <= 1) break; }
		if (j == points.size()) { points.push_back(dst_poly.points[i]); }
	}

	lines.clear();
	for (i = 0; i < src_poly.size(); i++) {
		Line line = Line(src_poly.points[i], src_poly.points[i + 1]);
		for (j = 0; j < lines.size(); j++) { if (lines[j] == line) break; }
		if (j == lines.size()) { lines.push_back(line); }
	}
	for (i = 0; i < dst_poly.size(); i++) {
		Line line = Line(dst_poly.points[i], dst_poly.points[i + 1]);
		for (j = 0; j < lines.size(); j++) { if (lines[j] == line) break; }
		if (j == lines.size()) { lines.push_back(line); }
	}
}


//辺s -> eの左側にマージ領域があり、右側にマージ後の領域が無い⇔trueを返す。（ただしRe軸を→, Im軸を↑に取って考える）
bool MargePoly::can_connect(Point s, Point e, Poly &src_poly, Poly &dst_poly, bool dst_is_piece) {
	Point mid = (s + e) * 0.5;
	double len = dist_error * 0.2;
	Point hosen = complex<double>(0, 1) * (e - s) / abs(e - s);

	bool is_left_in;	//左側にマージ領域がある⇔true, マージ領域 := マージ後の多角形
	bool is_right_in;	//右側にマージ領域がある⇔true

	Point left = mid + hosen * len;
	Point right = mid - hosen * len;

	if (dst_is_piece) {
		is_left_in  = src_poly.is_cover_point(left) || dst_poly.is_cover_point(left);
		is_right_in = src_poly.is_cover_point(right) || dst_poly.is_cover_point(right);
	}
	else {
		is_left_in  = (!src_poly.is_cover_point(left) && dst_poly.is_cover_point(left));
		is_right_in = (!src_poly.is_cover_point(right) && dst_poly.is_cover_point(right));
	}
	if (is_left_in && !is_right_in) return true;
	return false;
}


//tableの生成
void MargePoly::make_graph(Poly &src_poly, Poly &dst_poly, bool dst_is_piece) {
	int i, j, k;

	for (i = 0; i < points.size(); i++) for (j = 0; j < points.size(); j++) table[i][j] = false;

	for (i = 0; i < lines.size(); i++) {
		//点リスト生成
		vector<pair<Point, int> > p_list;
		for (j = 0; j < points.size(); j++) {	//グループjの代表点（代表点j）
			if (lines[i].dist(points[j]) <= dist_error * 0.1) {
				//代表点jは辺lines[i]上に乗っている！
				p_list.push_back(pair<Point, int>(points[j], j));
			}
		}

		//ソート
		for (j = 0; j < (int)p_list.size() - 1; j++) {
			for (k = (int)p_list.size() - 1; k > j; k--) {
				double dist_l = norm(p_list[k - 1].first - lines[i].s);
				double dist_r = norm(p_list[k].first - lines[i].s);
				if (dist_l > dist_r) {
					swap(p_list[k - 1], p_list[k]);
				}
			}
		}

		//辺追加
		for (j = 0; j < (int)p_list.size() - 1; j++) {
			int u = p_list[j].second;
			int v = p_list[j+1].second;

			//points[u] -> points[v]の左側にマージ後の領域があり、右側にマージ後の領域が無い場合、u->vをつなぐ
			if (can_connect(points[u], points[v], src_poly, dst_poly, dst_is_piece)) {
				table[u][v] = true;
			}
			//points[v] -> points[u]の左側にマージ後の領域があり、右側にマージ後の領域が無い場合、v->uをつなぐ
			if (can_connect(points[v], points[u], src_poly, dst_poly, dst_is_piece)) {
				table[v][u] = true;
			}
		}
	}
}



//反時計回りの多角形の生成（ただし, この段階では冗長な辺を含む)
//マージ後の多角形に含まれる点を1つ選ぶ(points[start_id]を選んだ)→次の点は全部試す(一意かも)→3点目以降は貪欲にとる→ループができるまで繰り返す。
//頂点列の向きが正しいかを判定する。OKならマージ終了。(面積比較はいらなくなった）
vector<Point> MargePoly::get_cycle(int start_id) {
	vector<bool> visited;
	int i, j;
	
	visited.resize(points.size());

	for (i = 0; i < points.size(); i++) {
		if (!table[start_id][i]) continue;

		//点start_id -> iと訪れてみた
		Point prev = points[start_id];
		Point now = points[i];
		int now_id = i;

		vector<Point> cycle;
		vector<int> cycle_point_id;
		for (j = 0; j < points.size(); j++) visited[j] = false;

		cycle.push_back(points[start_id]); cycle_point_id.push_back(start_id); visited[start_id] = true;
		cycle.push_back(points[i]); cycle_point_id.push_back(i);
			
		while (!visited[now_id]) {
			//フラグを立てる
			visited[now_id] = true;
			//次の場所を決める
			double tmp_ang = 114514;		//時計回りで内側を取るならmin化, 反時計回りで外周を取るならmin化 -> min化！！
			int next_id = -1;
			for (j = 0; j < points.size(); j++) {
				if (!table[now_id][j] || points[j] == prev) continue;

				Point vec = (points[j] - now) / (prev - now);
				double ang = atan2(vec.imag(), vec.real());
				if (ang < 0) ang += 2 * 3.14159265358979;
				if (tmp_ang > ang) { tmp_ang = ang; next_id = j; }
			}
			if (next_id == -1) {
				//printfDx("マージ処理エラー：サイクルが見つかりません\n");
				break;
			}
			//移動する
			prev = now;
			now = points[next_id];
			now_id = next_id;
			cycle.push_back(now);
			cycle_point_id.push_back(now_id);
		}

		if (now_id != start_id) { continue; }	//始点に戻れなかったので不可
		//面積計算
		Poly poly; poly.points = cycle;
		if (poly.area() < 0) continue;		//時計回りなのでNG
		
		//マージ可能！！ -> tableの更新もする
		for (j = 0; j < (int)cycle_point_id.size(); j++) {
			int u = cycle_point_id[j];
			int v = cycle_point_id[(j + 1) % cycle_point_id.size()];
			table[u][v] = false;
		}
		return cycle;
	}
	//マージ不可（マージすると空領域になってしまう）
	vector<Point> null_points;
	return null_points;
}


bool MargePoly::has_no_holl(vector<int> ids, Poly &poly) {
	sort(ids.begin(), ids.end());
	for (int i = 0; i < (int)ids.size(); i++) {	//ids[i]からスタート
		bool flag = true;
		for (int j = 0; j < (int)ids.size() - 1; j++) {	//ids[i+j]とids[i+j+1]の比較
			int u = (i + j) % ids.size();
			int v = (i + j + 1) % ids.size();
			u = (u < i) * poly.size() + ids[u];
			v = (v < i) * poly.size() + ids[v];
			if (v - u >= 2) {
				flag = false;
				break;
			}
		}
		if (flag) return true;
	}
	return false;
}

//サイクルから冗長な点を除いて多角形を作成
Poly MargePoly::to_poly(vector<Point> cycle, Poly &src_poly, Poly &dst_poly, double dist_error, bool dst_is_piece) {
	//冗長な点を取り除く
	vector<Point> new_cycle;

	while (true) {
		int i, j;
		for (i = 0; i < (int)cycle.size() - 1; i++) {
			Point a = cycle[(i == 0) ? cycle.size() - 2 : i - 1];
			Point b = cycle[i];
			Point c = cycle[i + 1];
			Line line = Line(a, c);

			if (line.dist(b) <= dist_error) {	//頂点iを取り除く
				vector<Point> next_cycle;
				for (j = 0; j < (int)cycle.size() - 1; j++) {
					if (j != i) {
						next_cycle.push_back(cycle[j]);
					}
				}
				if (next_cycle.size() <= 0) break;
				next_cycle.push_back(next_cycle[0]);
				cycle = next_cycle;
				break;
			}
		}
		if (i >= (int)cycle.size() - 1) {
			new_cycle = cycle;
			break;
		}
	}

	//マージした多角形を返す
	vector<Line> lines;
	for (int i = 0; i < src_poly.lines.size(); i++) lines.push_back(src_poly.lines[i]);
	for (int i = 0; i < dst_poly.lines.size(); i++) lines.push_back(dst_poly.lines[i]);

	Poly ret = Poly(new_cycle, lines);
	if (!dst_is_piece) { ret.point_reverse(); }
	return ret;
}


//マージ可能ならtrueを返し, 不可能ならfalseを返す
//dist_error, angle_error_degは、marge_polyに渡す値ではなく、評価関数evaluationで用いられている値にする。
bool MargePoly::can_marge(double dist_error, Poly &src_poly, Poly &dst_poly, bool dst_is_piece, vector<Poly> &wakus) {
	int i, j;

	if (dst_is_piece == false) return true;

	//穴の存在判定
	vector<int> ids, ids2;

	for (i = 0; i < src_poly.size(); i++) {
		for (j = 0; j < dst_poly.size(); j++) {
			Line line1 = Line(src_poly.points[i], src_poly.points[i + 1]);
			Line line2 = Line(dst_poly.points[j], dst_poly.points[j + 1]);
			
			double angle_error_deg = (asin(dist_error / line1.length()) + asin(dist_error / line2.length())) * 180.0 / 3.1415926;
			//angle_error_deg /= 1.41421356;

			Point rot = (line1.e - line1.s) / (line2.e - line2.s); rot /= abs(rot);
			if (abs(rot.imag()) > sin(angle_error_deg * 3.1415926 / 180.0)) { continue; }
			if (line1.dist(line2.s) > dist_error && line1.dist(line2.e) > dist_error && line2.dist(line1.s) > dist_error && line2.dist(line1.e) > dist_error) { continue; }
			double score = line1.match_score(line2, dist_error, angle_error_deg);
			if (score == 2) { continue; }

			//line1とline2が重なっている場合
			//printfDx("%d %d\n", i, j);
			ids.push_back(i);
			ids2.push_back(j);
			break;
		}
	}
	if (has_no_holl(ids, src_poly) && has_no_holl(ids2, dst_poly)) return true;
	return false;
}


//多角形のマージ
//dist_error…ある頂点iから距離dist_error以下にある頂点をグループ化するなど…。評価関数で用いているdist_errorよりも1.5倍くらい大きい方がよい。
//angle_error…冗長な点を取り除く際に用いる。大きすぎると頂点が欠落し、小さすぎると冗長な点が残る。評価関数で用いているangle_error_degと同じくらいにすると無難。
pair<bool, vector<Poly>> MargePoly::marge_poly(double dist_error, Poly src_poly, Poly dst_poly, bool dst_is_piece) {
	this->dist_error = dist_error * 1.6;
	
	point_marge(src_poly, dst_poly);
	make_group(src_poly, dst_poly);
	make_graph(src_poly, dst_poly, dst_is_piece); 
	
	double area = (dst_is_piece ? abs(src_poly.area()) + abs(dst_poly.area()) : abs(dst_poly.area()) - abs(src_poly.area()));
	vector<Point> cycle;

	vector<Poly> ret;
	int sid;
	for (sid = 0; sid < points.size(); sid++) {
		cycle = get_cycle(sid);
		if (cycle.size() <= 0) continue;
		
		//多角形を作る
		ret.push_back(to_poly(cycle, src_poly, dst_poly, this->dist_error, dst_is_piece));
	}
	if (ret.size() > 0) {
		return pair<bool, vector<Poly>>(true, ret);
	}

	//We cannot find any marged_polys
	if (!dst_is_piece && abs(area) <= 1000) {	//枠を埋めた
			cycle.push_back(Point(-114514, -114514));	//ダミーの点を追加
			ret.push_back(to_poly(cycle, src_poly, dst_poly, 0.002, dst_is_piece));
			return pair<bool, vector<Poly>>(true, ret);
	}
	//隣接行列を表示（デバッグ）
	/*printfDx("-------隣接行列------------\n");
	for (int i = 0; i < points.size(); i++) {
		for (int j = 0; j < points.size(); j++) {
			if (table[i][j]) {
				printfDx("%d ", j);
			}
		}
		printfDx("\n");
	}
	printfDx("---------------------------\n");*/

	return pair<bool, vector<Poly>>(false, ret);
}
