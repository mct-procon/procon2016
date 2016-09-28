//グラフ作って頑張る
#include "MargePoly.h"
#include "DxLib.h"

void MargePoly::make_all_pl(Poly &src_poly, Poly &dst_poly) {
	all_p.clear();
	all_l.clear();
	for (int i = 0; i < src_poly.size(); i++) {
		all_p.push_back(src_poly.points[i]);
		all_l.push_back(Line(src_poly.points[i], src_poly.points[i + 1]));
	}
	for (int i = 0; i < dst_poly.size(); i++) {
		all_p.push_back(dst_poly.points[i]);
		all_l.push_back(Line(dst_poly.points[i], dst_poly.points[i + 1]));
	}
}


//全頂点にグループ番号をつけ、各グループの場所（＝代表点の座標）を記録する
void MargePoly::make_group() {
	int i, j, gid = 0;

	points.clear();
	for (i = 0; i < all_p.size(); i++) group_id[i] = -1;
	for (i = 0; i < all_p.size(); i++) {
		if (group_id[i] >= 0) continue;
		
		group_id[i] = gid;
		for (j = i + 1; j < all_p.size(); j++) {
			if (group_id[j] >= 0) continue;
			if (abs(all_p[i] - all_p[j]) <= dist_error) {
				group_id[j] = gid;
			}
		}
		points.push_back(all_p[i]);		//points[gid] = all_p[i]
		gid++;
	}
}


//各線分について、その線分との距離がdist_error以下の頂点（グループ化後のもの）を全て検出し、
//（頂点座標, グループ番号)のペアを[x座標 -> y座標]でソートする。
//その後、前から貪欲に辺リスト ( = 隣接行列 )に入れていく。
void MargePoly::make_graph() {
	int i, j, k;

	for (i = 0; i < points.size(); i++) for (j = 0; j < points.size(); j++) table[i][j] = false;

	for (i = 0; i < all_l.size(); i++) {
		//点リスト生成
		vector<pair<Point, int> > p_list;
		for (j = 0; j < points.size(); j++) {	//グループjの代表点（代表点j）
			if (all_l[i].dist(points[j]) <= dist_error) {
				//代表点jは辺all_l[i]上に乗っている！
				p_list.push_back(pair<Point, int>(points[j], j));
			}
		}
		//バブルソート
		for (j = 0; j < (int)p_list.size() - 1; j++) {
			for (k = (int)p_list.size() - 1; k > j; k--) {
				Point lp = p_list[k-1].first;
				Point rp = p_list[k].first;
				if (lp.real() > rp.real() || (lp.real() == rp.real() && lp.imag() > rp.imag())) {
					swap(p_list[k-1], p_list[k]);
				}
			}
		}
		//辺追加
		for (j = 0; j < (int)p_list.size() - 1; j++) {
			int u = p_list[j].second;
			int v = p_list[j+1].second;
			table[u][v] = true;
			table[v][u] = true;
		}
	}
}


//dist > 0のとき…拡大多角形を返す。ピースの場合は当たり判定枠が外側にできる。枠の場合は当たり判定枠が内側にできる。
//dist < 0のとき…縮小多角形を返す。
Poly MargePoly::scalechange_poly(Poly &poly, double dist) {
	int i;
	int n = poly.size();
	vector<Line> lines(n);
	vector<Point> points(n + 1);

	//多角形内部に線分を引く
	for (i = 0; i < n; i++) {
		lines[i] = Line(poly.points[i], poly.points[i + 1]);
		lines[i].move(dist);
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


//マージ後も消えない点（代表点）のidを一つ返す
int MargePoly::get_startPoint_id(Poly &src_poly, Poly &dst_poly, bool dst_is_piece) {
	if (dst_is_piece) {
		for (int i = 0; i < points.size(); i++) { if (abs(points[i].imag() - min(src_poly.ly, dst_poly.ly)) <= dist_error) return i; }
	}
	else {
		//拡大多角形を得る
		Poly poly = scalechange_poly(src_poly, dist_error);
		//拡大多角形polyの内部にない頂点を選ぶ
		for (int i = 0; i < points.size(); i++) {
			if (!poly.is_cover_point(points[i])) return i;
		}
	}
	return -1;
}


//多角形の生成（ただし, この段階では冗長な辺を含む)
//マージ後の多角形に含まれる点を1つ選ぶ(points[start_id]を選んだ)→次の点は全部試す→3点目以降は貪欲にとる→ループができるまで繰り返す。
//面積比較によって穴が存在しないか判定したり、頂点列の向きが正しいかを判定する。OKならマージ終了。
vector<Point> MargePoly::get_cycle(int start_id, bool is_clockwise, double area) {
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
		for (j = 0; j < points.size(); j++) visited[j] = false;

		cycle.push_back(points[start_id]); visited[start_id] = true;
		cycle.push_back(points[i]);
			
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
			if (next_id == -1) { vector<Point> null_points; printfDx("マージ処理エラー：サイクルが見つかりません\n"); return null_points; }
			//移動する
			prev = now;
			now = points[next_id];
			now_id = next_id;
			cycle.push_back(now);
		}

		if (now_id != start_id) { continue; }	//始点に戻れなかったので不可
		//面積計算
		Poly poly; poly.points = cycle;
		if (!is_clockwise) { if (poly.area() < 0) continue; }	//マージするとピースになるのに、時計回りなのでNG
		else { if (poly.area() > 0) continue; }					//マージすると枠になるのに、反時計回りなのでNG
		if (abs((abs(poly.area()) - area)) > dist_error * 1000) { continue; }	//面積が合わないのでNG
		//マージ可能！！
		return cycle;
	}
	//マージ不可（マージすると空領域になってしまう）
	vector<Point> null_points;
	return null_points;
}



//can_margeの補助関数（平面幾何）, http://www.prefield.com/algorithm/　のほとんど写し。norm(x)はxの長さの2乗を返す
double MargePoly::cross(Point &a, Point &b) {
	return imag(conj(a) * b);
}

double MargePoly::dot(Point &a, Point &b) {
	return real(conj(a) * b);
}

//点の進行方向
int MargePoly::ccw(Point a, Point b, Point c) {
	b -= a; c -= a;
	if (cross(b, c) > 0)   return +1;       // counter clockwise
	if (cross(b, c) < 0)   return -1;       // clockwise
	if (dot(b, c) < 0)     return +2;       // c--a--b on line
	if (norm(b) < norm(c)) return -2;       // a--b--c on line
	return 0;
}

//凸包, (反時計回り)
Poly MargePoly::convex_hull(vector<Point> points) {
	int n = points.size(), k = 0;
	sort(points.begin(), points.end(), [](Point &a, Point &b) { return (a.real() == b.real()) ? a.imag() < b.imag() : a.real() < b.real(); });	//x(=real())昇順にソート
	vector<Point> ret(2 * n);
	for (int i = 0; i < n; ret[k++] = points[i++]) // lower-hull
		while (k >= 2 && ccw(ret[k-2], ret[k-1], points[i]) <= 0) --k;
	for (int i = n-2, t = k+1; i >= 0; ret[k++] = points[i--]) // upper-hull
		while (k >= t && ccw(ret[k-2], ret[k-1], points[i]) <= 0) --k;
	ret.resize(k);
	
	vector<Line> null_line;
	return Poly(ret, null_line);
}

//凸多角形の直径
double MargePoly::convex_diameter(vector<Point> points) {
	const int n = (int)points.size() - 1;	//頂点数
	int is = 0, js = 0;
	for (int i = 1; i < n; i++) {
		if (imag(points[i]) > imag(points[is])) is = i;
		if (imag(points[i]) < imag(points[js])) js = i;
	}
	double maxd = norm(points[is] - points[js]);	//直径(暫定値)の2乗を記録

	int i, maxi, j, maxj;
	i = maxi = is;
	j = maxj = js;
	do {
		Point diff_i = points[(i + 1) % n] - points[i];
		Point diff_j = points[(j + 1) % n] - points[j];
		if (cross(diff_i, diff_j) >= 0) j = (j + 1) % n;
		else i = (i + 1) % n;
		if (norm(points[i] - points[j]) > maxd) {
			maxd = norm(points[i] - points[j]);
			maxi = i, maxj = j;
		}
	} while (i != is || j != js);
	return sqrt(maxd);
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


//マージ可能ならtrueを返し, 不可能ならfalseを返す
//dist_error, angle_error_degは、marge_polyに渡す値ではなく、評価関数evaluationで用いられている値にする。
bool MargePoly::can_marge(double dist_error, double angle_error_deg, Poly &src_poly, Poly &dst_poly, bool dst_is_piece, vector<Poly> &wakus) {
	int i, j;

	if (dst_is_piece) {
		//src_poly, dst_polyを囲う最小の凸多角形Aを求める（凸包） -> 直径を求める（尺取り法）
		vector<Point> points;
		for (i = 0; i < src_poly.size(); i++) points.push_back(src_poly.points[i]);
		for (i = 0; i < dst_poly.size(); i++) points.push_back(dst_poly.points[i]);
		Poly polyA = convex_hull(points);
		polyA = scalechange_poly(polyA, -dist_error * 3);
		double rA = convex_diameter(polyA.points);

		for (i = 0; i < wakus.size(); i++) {
			if (wakus[i].size() <= 0) continue;	//ゾンビは無視
			//枠iを囲う最小の凸多角形Biを求める（凸包） -> 直径を求める（尺取り法）
			Poly polyB = convex_hull(wakus[i].points);
			double rB = convex_diameter(polyB.points);
			//AがBiに入らない⇔src_poly, dst_polyをマージするとBiに入らない⇒src_poly, dst_polyをマージすると枠に入らない
			//今回は明らかにAがBiに入らない場合だけを検出する
			if (rA > rB) continue;
			if (abs(polyA.area()) > abs(polyB.area())) continue;
			break;
		}
		if (i == wakus.size()) { return false; }	//どの枠にも（明らかに）入らない場合
	}

	//穴の存在判定
	vector<int> ids, ids2;
	for (i = 0; i < src_poly.size(); i++) {
		for (j = 0; j < dst_poly.size(); j++) {
			Line line1 = Line(src_poly.points[i], src_poly.points[i + 1]);
			Line line2 = Line(dst_poly.points[j], dst_poly.points[j + 1]);
			
			Point rot = (line1.e - line1.s) / (line2.e - line2.s); rot /= abs(rot);
			if (abs(rot.imag()) > sin(angle_error_deg * 3.1415926 / 180.0)) { continue; }
			if (line1.dist(line2.s) > dist_error && line1.dist(line2.e) > dist_error && line2.dist(line1.s) > dist_error && line2.dist(line1.e) > dist_error) { continue; }
			double score = line1.machi_score(line2, dist_error, angle_error_deg);
			if (score == 2) { continue; }

			//line1とline2が重なっている場合
			printfDx("%d %d\n", i, j);
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
Poly MargePoly::marge_poly(double dist_error, double angle_error_deg, Poly &src_poly, Poly &dst_poly, bool dst_is_piece) {
	Poly null_poly;

	this->dist_error = dist_error * 1.6;
	make_all_pl(src_poly, dst_poly);
	make_group();
	make_graph(); 
	
	int start_id = get_startPoint_id(src_poly, dst_poly, dst_is_piece);
	double area = (dst_is_piece ? abs(src_poly.area()) + abs(dst_poly.area()) : abs(dst_poly.area()) - abs(src_poly.area()));
	vector<Point> cycle;

	if (start_id < 0) { //自明な開始点が存在しないので開始点を全探索する（枠の全頂点をピースが囲んでいる場合）
		int sid;
		for (sid = 0; sid < points.size(); sid++) {
			cycle = get_cycle(sid, !dst_is_piece, area);
			if (cycle.size() > 0) { break; }
		}
		if (sid == points.size()) { //多角形を作れなかった or 枠を埋めた
			if (!dst_is_piece && abs(area) <= 1000) {	//枠を埋めた
				cycle.push_back(Point(-114514, -114514));	//ダミーの点を追加
			}
			else {
				return null_poly;
			}
		}
	}
	else {	//自明な開始点が存在するのでそこから開始する
		cycle = get_cycle(start_id, !dst_is_piece, area);
		if (cycle.size() == 0) {	//多角形を作れなかった or 枠を埋めた
			if (!dst_is_piece && abs(area) <= 1000) {	//枠を埋めた
				cycle.push_back(Point(-114514, -114514));	//ダミーの点を追加
			}
			else {
				return null_poly;
			}
		}
	}

	//冗長な点を取り除く
	vector<Point> new_cycle;
	new_cycle.push_back(cycle[0]);
	for (int i = 0; i < (int)cycle.size() - 2; i++) {
		Point vec = (cycle[i + 1] - cycle[i]) / (cycle[i + 2] - cycle[i + 1]);
		vec /= abs(vec);
		if (abs(vec.imag()) > sin(angle_error_deg * 3.1415926 / 180.0)) {	//点j + 1で曲がっている
			new_cycle.push_back(cycle[i + 1]);
		}
	}
	new_cycle.push_back(new_cycle[0]);

	//マージした多角形を返す
	vector<Line> lines;
	for (int i = 0; i < src_poly.lines.size(); i++) lines.push_back(src_poly.lines[i]);
	for (int i = 0; i < dst_poly.lines.size(); i++) lines.push_back(dst_poly.lines[i]);
	return Poly(new_cycle, lines);
}
