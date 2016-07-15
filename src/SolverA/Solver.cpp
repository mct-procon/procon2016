#include "Solver.h"
#include "DxLib.h"

//問題を解きます
void Solver::solve() {
	piece[0].move(0, waku[0].get_point(0), waku[0].get_point(1));
	piece[1].move(0, waku[0].get_point(0), waku[0].get_point(1));
	piece[2].move(0, waku[0].get_point(0), waku[0].get_point(1));
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
		for (j = 0; j < piece[i].size(); j++) {
			Point s = piece[i].get_point(j);
			Point e = piece[i].get_point(j + 1);
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 0, 255), 2);
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