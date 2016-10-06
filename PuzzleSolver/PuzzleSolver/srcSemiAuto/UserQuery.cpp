//ユーザの入力を受け付けて、ソルバーに計算させたい情報を決める。
//変数はread_only

#include "UserQuery.h"
#include "Extern.h"

//マウスと最も近い辺を返す (描画座標で、distより遠ければ(false, -1, -1)を返す)
tuple<bool, int, int> UserQuery::get_nearest_line(int mouseX, int mouseY, double dist) {
	int i, j;

	Point mouse_point = Point((double)mouseX, (double)mouseY);
	double min_dist = dist;
	tuple<bool, int, int> ret = tuple<bool, int, int>(false, -1, -1);

	for (i = 0; i < wakus.size(); i++) {
		if (wakus[i].size() <= 0) continue;		//ゾンビは無視
		for (j = 0; j < wakus[i].size(); j++) {
			Line line = Line(to_draw_point( wakus[i].points[j] ), to_draw_point( wakus[i].points[j + 1] ));	//描画座標で枠iの辺jを取得
			if (line.dist(mouse_point) < min_dist) {
				min_dist = line.dist(mouse_point);
				ret = tuple<bool, int, int>(false, i, j);
			}
		}
	}

	for (i = 0; i < pieces.size(); i++) {
		if (pieces[i].size() <= 0) continue;	//ゾンビは無視
		for (j = 0; j < pieces[i].size(); j++) {
			Line line = Line(to_draw_point( pieces[i].points[j] ), to_draw_point( pieces[i].points[j + 1] ));
			if (line.dist(mouse_point) < min_dist) {
				min_dist = line.dist(mouse_point);
				ret = tuple<bool, int, int>(true, i, j);
			}
		}
	}
	return ret;
}


//マウスと最も近い頂点を返す
tuple<bool, int, int> UserQuery::get_nearest_point(int mouseX, int mouseY, double dist) {
	int i, j;

	Point mouse_point = Point((double)mouseX, (double)mouseY);
	double min_dist = dist;
	tuple<bool, int, int> ret = tuple<bool, int, int>(false, -1, -1);

	for (i = 0; i < wakus.size(); i++) {
		if (wakus[i].size() <= 0) continue;		//ゾンビは無視
		for (j = 0; j < wakus[i].size(); j++) {
			Point point = to_draw_point( wakus[i].points[j] ); //描画座標で枠iの頂点jを取得
			if (abs(point - mouse_point) < min_dist) {
				min_dist = abs(point - mouse_point);
				ret = tuple<bool, int, int>(false, i, j);
			}
		}
	}

	for (i = 0; i < pieces.size(); i++) {
		if (pieces[i].size() <= 0) continue;	//ゾンビは無視
		for (j = 0; j < pieces[i].size(); j++) {
			Point point = to_draw_point( pieces[i].points[j] );	//描画座標でピースiの頂点jを取得
			if (abs(point - mouse_point) < min_dist) {
				min_dist = abs(point - mouse_point);
				ret = tuple<bool, int, int>(true, i, j);
			}
		}
	}
	return ret;
}


//右クリックのイベント：多角形の反転, (頂点に近ければ頂点の削除）
void UserQuery::right_click(int mouseX, int mouseY) {
	//頂点の削除
	tuple<bool, int, int> point = get_nearest_point(mouseX, mouseY, 8);
	bool is_piece = get<0>(point);
	int poly_id = get<1>(point);
	int point_id = get<2>(point);
	
	if (poly_id >= 0) {
		//頂点の貼り直しをします
		Poly &poly = (is_piece) ? pieces[poly_id] : wakus[poly_id];
		vector<Point> points;
		vector<Line> lines;
		for (int i = 0; i < poly.size(); i++) {
			if (i == point_id) continue;
			points.push_back(poly.points[i]);
		}
		points.push_back(points[0]);
		lines = poly.lines;
		
		//貼り直しにも(そこそこの)条件を付けよう -> 削除予定の頂点から最も近い辺までの距離 (計算座標)
		double min_dist = 114514;
		for (int i = 0; i < (int)points.size() - 1; i++) {
			Line line = Line(points[i], points[i + 1]);
			min_dist = min(min_dist, line.dist(poly.points[point_id]));
		}
		if (min_dist <= 20) {
			poly = Poly(points, lines);
		}
		return;
	}

	//反転
	tuple<bool, int, int> line = get_nearest_line(mouseX, mouseY, 1000);
	if (get<0>(line)) {	//ピースの場合
		pieces[get<1>(line)].turn();
	}
}


//左クリックのイベント：出発辺(src), 到着辺(dst)の辺の選択
void UserQuery::left_click(int mouseX, int mouseY) {
	tuple<bool, int, int> res = get_nearest_line(mouseX, mouseY, 40);
	bool is_piece = get<0>(res);
	int poly_id = get<1>(res);
	int line_id = get<2>(res);

	if (poly_id < 0) return;

	//既に登録している辺を選択した場合
	if (is_exist_src && is_piece && src_poly_id == poly_id && src_line_id == line_id) {
		if (!is_reverse_line) {
			is_reverse_line = true;
		}
		else {
			is_exist_src = false;
		}
		return;
	}
	if (is_exist_dst && dst_is_piece == is_piece && dst_poly_id == poly_id && dst_line_id == line_id) {
		if (!is_reverse_line) {
			is_reverse_line = true;
		}
		else {
			is_exist_dst = false;
		}
		return;
	}

	//新規
	if (!is_exist_dst) {	//dstinationの登録
		dst_is_piece = is_piece;
		dst_poly_id = poly_id;
		dst_line_id = line_id;
		is_exist_dst = true;
		is_reverse_line = false;
	}
	else if (!is_exist_src) {		//sourceの登録
		if (!is_piece) {
			//srcが枠, dstがピースっておかしいよね。
			if (is_exist_dst && dst_is_piece) {
				//新規のをdstにして、dstになっているのをsrcにするか～
				src_poly_id = dst_poly_id;
				src_line_id = dst_line_id;
				is_exist_src = true;
				dst_is_piece = false;
				dst_poly_id = poly_id;
				dst_line_id = line_id;
				is_exist_dst = true;
				is_reverse_line = false;
			}
			return;
		}
		src_poly_id = poly_id;
		src_line_id = line_id;
		is_exist_src = true;
		is_reverse_line = false;
	}
}


//中クリックのイベント：ピースのドラッグ
void UserQuery::middle_push(int mouseX, int mouseY) {
	complex<double> trans = complex<double>(mouseX - (double)bmouseX, mouseY - (double)bmouseY);	//描画座標における平行移動量
	if (abs(trans) <= 2) { return; }

	tuple<bool, int, int> res = get_nearest_line(bmouseX, bmouseY, 60);
	bool is_piece = get<0>(res);
	int poly_id = get<1>(res);
	int line_id = get<2>(res);

	if (poly_id < 0 || !is_piece) return;

	//ピースpoly_idをドラッグする
	trans /= scale;
	for (int i = 0; i < pieces[poly_id].points.size(); i++) pieces[poly_id].points[i] += trans;
	for (int i = 0; i < pieces[poly_id].lines.size(); i++) { pieces[poly_id].lines[i].s += trans; pieces[poly_id].lines[i].e += trans; }
	pieces[poly_id] = Poly(pieces[poly_id].points, pieces[poly_id].lines);
}


//情報のリセット
void UserQuery::init() {
	is_exist_src = false;
	is_exist_dst = false;
	is_reverse_line = false;
	prev_mouseInput = 0;
	now_mouseInput = 0;
}


//矢印の描画
void UserQuery::DrawArrow(int x1, int y1, int x2, int y2, int color, int thickness) {
	double size = 15;	//矢印の先っぽの大きさ

	//計算
	Point a = Point((double)x2 - x1, (double)y2 - y1);	a /= abs(a);		//方向ベクトル
	Point b = Point((double)y1 - y2, (double)x2 - x1);	b /= abs(b);		//法線ベクトル
	Point C = Point((double)x2, (double)y2) - a * (sqrt(3.0) / 2 * size);	//交点
	Point tri1 = C + b * (-size / 2);
	Point tri2 = C + b * (size / 2);

	//描画
	DrawLine(x1, y1, x2, y2, color, thickness);
	DrawTriangle((int)tri1.real(), (int)tri1.imag(), (int)tri2.real(), (int)tri2.imag(), x2, y2, color, TRUE);
}


//プロセスメッセージを返すメイン
int UserQuery::set_query() {
	init();
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0) {
		memcpy(bkey, key, 256);
		GetHitKeyStateAll(key);
		if (!bkey[KEY_INPUT_RETURN] && key[KEY_INPUT_RETURN]) break;	//エンターで終了

		prev_mouseInput = now_mouseInput;
		now_mouseInput = GetMouseInput();
		int mouseX, mouseY;
		GetMousePoint(&mouseX, &mouseY);

		if (!(prev_mouseInput & MOUSE_INPUT_RIGHT) && (now_mouseInput & MOUSE_INPUT_RIGHT)) right_click(mouseX, mouseY);
		if (!(prev_mouseInput & MOUSE_INPUT_LEFT) && (now_mouseInput & MOUSE_INPUT_LEFT)) left_click(mouseX, mouseY);
		if (now_mouseInput & MOUSE_INPUT_MIDDLE) middle_push(mouseX, mouseY);

		//BackSpaceキーを押すと、手が戻る
		if (!bkey[KEY_INPUT_BACK] && key[KEY_INPUT_BACK]) {
			State res = backup.pop();
			pieces = res.pieces;
			wakus = res.wakus;
		}

		//スペースキーでマージ処理の実行許可を操作
		if (!bkey[KEY_INPUT_SPACE] && key[KEY_INPUT_SPACE]) {
			is_do_marge = !is_do_marge;
		}
		DrawFormatString(100, 150, 0, "マージ処理：%s", (is_do_marge) ? "許可" : "拒否");

		//描画
		update_draw_option();
		draw();
		if (is_exist_src) {
			Line line = Line(pieces[src_poly_id].points[src_line_id], pieces[src_poly_id].points[src_line_id + 1]);
			line.s = to_draw_point(line.s);
			line.e = to_draw_point(line.e);
			DrawFormatString((int)(line.s.real() + line.e.real()) / 2, (int)(line.s.imag() + line.e.imag()) / 2, GetColor(0, 255, 255), "src");
			if (is_reverse_line) {	//始点 = e
				DrawArrow((int)line.e.real(), (int)line.e.imag(), (int)line.s.real(), (int)line.s.imag(), GetColor(0, 255, 255), 4);	//移動元は水色で
			}
			else {	//始点 = s
				DrawArrow((int)line.s.real(), (int)line.s.imag(), (int)line.e.real(), (int)line.e.imag(), GetColor(0, 255, 255), 4);	//移動元は水色で
			}
		}
		if (is_exist_dst) {
			Line line;
			if (dst_is_piece) line = Line(pieces[dst_poly_id].points[dst_line_id], pieces[dst_poly_id].points[dst_line_id + 1]);
			else line = Line(wakus[dst_poly_id].points[dst_line_id], wakus[dst_poly_id].points[dst_line_id + 1]);
			line.s = to_draw_point(line.s);
			line.e = to_draw_point(line.e);
			DrawFormatString((int)(line.s.real() + line.e.real()) / 2, (int)(line.s.imag() + line.e.imag()) / 2, GetColor(255, 100, 0), "dst");
			if (is_reverse_line) {	//始点 = s
				DrawArrow((int)line.s.real(), (int)line.s.imag(), (int)line.e.real(), (int)line.e.imag(), GetColor(255, 100, 0), 4);	//移動先はオレンジで
			}
			else {	//始点 = e
				DrawArrow((int)line.e.real(), (int)line.e.imag(), (int)line.s.real(), (int)line.s.imag(), GetColor(255, 100, 0), 4);	//移動先はオレンジで
			}
		}
		//DrawFormatString(100, 100, 0, "pieceNum = %d\n", pieces.size());

		//マウス座標の記録
		bmouseX = mouseX;
		bmouseY = mouseY;
	}
	return ProcessMessage();
}
