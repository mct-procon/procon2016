#include "SolverBase.h"

//多角形の描画
void SolverBase::draw_poly(Poly poly, int red, int green, int blue, double scale) {
	for (int j = 0; j < poly.size(); j++) {
		Point s = poly.get_point(j) * scale;
		Point e = poly.get_point(j + 1) * scale;
		DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(red, green, blue), 2);
		//DrawFormatString((int)s.real(), (int)s.imag(), 0, "%d", j);
	}
}

//多角形を読み込む
Poly SolverBase::read_poly(ifstream &ifs, bool is_piece) {
	Point point0;
	vector<Point> points;

	//頂点数
	int tyotenNum;
	ifs >> tyotenNum;

	//x座標 -> y座標
	vector<double> x, y;
	x.resize(tyotenNum);
	y.resize(tyotenNum);
	for (int i = 0; i < tyotenNum; i++) ifs >> x[i];
	for (int i = 0; i < tyotenNum; i++) ifs >> y[i];

	//頂点列の生成
	for (int i = 0; i < tyotenNum; i++) points.push_back(Point(x[i], y[i]));
	point0 = points[0];
	for (int i = 0; i < tyotenNum; i++) points[i] -= point0;

	//多角形を返す
	return Poly(point0, points, is_piece);
}

//多角形を書き込む
void SolverBase::print_poly(ofstream &ofs, Poly poly) {
	//頂点数
	ofs << poly.size() << endl;

	//x座標
	ofs << poly.get_point(0).real();
	for (int i = 1; i < poly.size(); i++) ofs << poly.get_point(i).real() << endl;
	ofs << endl;

	//y座標
	ofs << poly.get_point(0).imag();
	for (int i = 1; i < poly.size(); i++) ofs << poly.get_point(i).imag() << endl;
	ofs << endl;

	//改行
	ofs << endl;

}


//当たり判定用の輪郭を返す。
//dist > 0のとき…ピースの場合は当たり判定枠が内側にできる。枠の場合は当たり判定枠が外側にできる。
Poly SolverBase::get_scaledown_poly(Poly poly) {
	int i;
	int n = poly.size();
	vector<Line> line(n);
	Point point0;
	vector<Point> point(n);

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
	return Poly(point0, point, poly.is_piece());
}

//多角形wakuの内部に置かれているピースの番号を返す
vector<int> SolverBase::get_contain_piece_id(Poly waku) {
	int id;
	vector<int> ret;

	waku = get_scaledown_poly(waku);		//当たり判定枠を代入

	for (id = 0; id < pieces.size(); id++) {
		if (!waku.is_hit(pieces[id]) && waku.is_contain(pieces[id]) && pieces[id].is_putted) {
			ret.push_back(id);
		}
	}
	return ret;
}


//多角形srcを動かす。
//turnmode…多角形の反転状態. 初期状態と比べて反転している状態にしたいときはtrueにする。
void SolverBase::move(Poly &src, int src_id, bool src_turnmode, Poly &dst, int dst_id, bool dst_turnmode) {
	if (src.is_turned() != src_turnmode) src.turn();
	if (dst.is_turned() != dst_turnmode) dst.turn();
	Point s = dst.get_point(dst_id);
	Point e = dst.get_point(dst_id - 1);
	src.move(src_id, s, e);
}

//polyを置けるか？
//is_piece…polyがピースならtrue, 枠ならfalse
bool SolverBase::canput(Poly poly) {
	int i;

	poly = get_scaledown_poly(poly);		//当たり判定枠を代入

	for (i = 0; i < pieces.size(); i++) if (pieces[i].is_putted && poly.is_hit(pieces[i])) return false;		//ピースと接触したらダメ
	for (i = 0; i < wakus.size(); i++) if (wakus[i].is_putted && poly.is_hit(wakus[i])) return false;			//枠と接触したらダメ

	for (i = 0; i < wakus.size(); i++) if (wakus[i].is_putted && poly.is_contain(wakus[i])) return false;		//枠を含んだらダメ
	for (i = 0; i < pieces.size(); i++) if (pieces[i].is_putted && poly.is_contained(pieces[i])) return false;	//ピースに含まれたらダメ

	if (poly.is_piece()) {
		for (i = 0; i < pieces.size(); i++) if (pieces[i].is_putted && poly.is_contain(pieces[i])) return false;	//ピースを含んだらダメ
		return true;
	}
	else {
		for (i = 0; i < wakus.size(); i++) if (wakus[i].is_putted && poly.is_contained(wakus[i])) return false;		//枠に含まれたらダメ
		return true;
	}
	return false;	//ダミーの戻り値
}

//答えを生成する。（動かした枠を元に戻す。枠内のピースも一緒に動かす。）
void SolverBase::make_answer() {
	int id, i;

	for (id = 0; id < wakus.size(); id++) {
		vector<int> ids = get_contain_piece_id(wakus[id]);
		//枠の反転
		bool is_turn = wakus[id].is_turned();
		if (is_turn) wakus[id].turn2(0);
		//枠の回転
		Point src_s = wakus[id].get_point(0);
		Point src_e = wakus[id].get_point(1);
		Point dst_s = wakusIni[id].get_point(0);
		Point dst_e = wakusIni[id].get_point(1);
		Point mul = (dst_e - dst_s) / (src_e - src_s); mul /= abs(mul);
		wakus[id].move2(mul, Point(0, 0), Point(0, 0));
		//枠の平行移動
		Point trans = wakusIni[id].get_point(0) - wakus[id].get_point(0);
		wakus[id].move2(Point(1, 0), Point(0, 0), trans);
		wakus[id].is_putted = true;

		//ピースの移動
		for (i = 0; i < ids.size(); i++) {
			if (is_turn) pieces[ids[i]].turn2(0);
			pieces[ids[i]].move2(mul, Point(0, 0), trans);
		}
	}
}


//初期化
SolverBase::SolverBase() {
	scale = 0.45;
}

//描画
void SolverBase::draw(bool is_draw_didnot_put) {
	int i;

	//枠
	for (i = 0; i < wakus.size(); i++) { 
		draw_poly(wakus[i], 0, 0, 0, scale);
	}

	//ピース
	for (i = 0; i < pieces.size(); i++) {
		if (pieces[i].is_putted) {
			draw_poly(pieces[i], 0, 0, 255, scale);
		}
	}

	//置いてないピース
	for (i = 0; i < pieces.size(); i++) {
		if (!pieces[i].is_putted && is_draw_didnot_put) {
			draw_poly(pieces[i], 0, 255, 0, scale);
		}
	}

	//当たり判定枠
	/*for (i = 0; i < wakus.size(); i++) {
		if (!wakus[i].is_putted && !is_draw_didnot_put) { continue; }
		draw_poly(get_scaledown_poly(wakus[i]), 255, 0, 255, scale);
	}
	for (i = 0; i < pieces.size(); i++) {
		if (!pieces[i].is_putted && !is_draw_didnot_put) { continue; }
		draw_poly(get_scaledown_poly(pieces[i]), 255, 0, 255, scale);
	}*/

	//スコア
	int score = 0;
	for (i = 0; i < pieces.size(); i++) { score += pieces[i].is_putted; }
	DrawFormatString(500, 150, 0, "置けた石の数 = %d (/%d)", score, (int)pieces.size());

	//埋めた割合[%]
	double area_sum = 0, area_putted = 0;
	for (i = 0; i < pieces.size(); i++) {
		area_sum += pieces[i].area();
		area_putted += pieces[i].is_putted * pieces[i].area();
	}
	DrawFormatString(500, 180, 0, "埋めた割合 = %.1f[%%]", area_putted * 100.0 / area_sum);
}

//入力
void SolverBase::input(string filename, double dist) {
	ifstream ifs(filename); if (ifs.fail()) return;
	int num;

	this->dist = dist;

	//枠
	ifs >> num;
	for (int i = 0; i < num; i++) wakus.push_back(read_poly(ifs, false));

	//ピース
	ifs >> num;
	for (int i = 0; i < num; i++) pieces.push_back(read_poly(ifs, true));

	//枠の頂点列を時計回りにする
	for (int i = 0; i < wakus.size(); i++) {
		if (wakus[i].area() > 0) {
			wakus[i].point_reverse();
		}
	}

	//ピースの頂点列を反時計回りにする
	for (int i = 0; i < pieces.size(); i++) {
		if (pieces[i].area() < 0) {
			pieces[i].point_reverse();
		}
	}

	//初期枠
	for (int i = 0; i < wakus.size(); i++) wakusIni.push_back(wakus[i]);
	ifs.close();
}

//出力
void SolverBase::print(string filename) {
	ofstream ofs(filename); if (ofs.fail()) return;

	//枠
	ofs << wakus.size() << endl;
	for (int i = 0; i < wakus.size(); i++) print_poly(ofs, wakus[i]);
	//ピース
	ofs << pieces.size() << endl;
	for (int i = 0; i < pieces.size(); i++) print_poly(ofs, pieces[i]);
	ofs.close();
}
