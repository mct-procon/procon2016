//piecedata.txtを読み込んで、諸加工を施す。
//反転・回転・平行移動を「画面に収まるくらい？」のやんちゃさで行う。
//加工後のデータを「probdata.txt」に出力する。sキーで出力。
//注意：piecedata.txtは時計回りの多角形データ。しかし、probdata.txtは反時計回りの多角形データにする。

#include "DxLib.h"
#include <iostream>
#include <tuple>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <iomanip>
using namespace std;

const int INF = 876765346;
const double PAI = 3.14159265358979;

struct Point {
	double x, y;
	Point (double x, double y) { this->x = x; this->y = y; }
	Point () { }
	bool operator<(const Point &r) const { return (x != r.x) ? x < r.x : y < r.y; }
};

//データ用
int n;
vector<Point> polyIni[100];	//加工前のピース
Point base[100];	//重心

vector<Point> poly[100];	//加工後のピース

//加工用
double rot[100];	//重心を中心に回転
bool opFlag[100];	//重心Xを中心に対称移動
Point trans[100];	//平行移動

//プロトタイプ
void input();
void init();
void convert();		//加工用変数を弄る関数
void makePoly();	//加工後のピースpoly[i]を作る関数
void draw();
void print();

//メイン
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 600, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	input();
	init();
	convert();
	makePoly();

	// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ESCで終了)
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		draw();
		if (CheckHitKey(KEY_INPUT_S)) { print(); }
	}
	DxLib_End();
	return 0;
}

//入力
void input() {
	ifstream ifs("piecedata.txt");

	ifs >> n;
	for (int i = 0; i < n; i++) {
		int tyotenNum;
		//頂点数
		ifs >> tyotenNum;
		polyIni[i].resize(tyotenNum);
		//x座標
		for (int j = 0; j < tyotenNum; j++) {
			ifs >> polyIni[i][j].x;
		}
		//y座標
		for (int j = 0; j < tyotenNum; j++) {
			ifs >> polyIni[i][j].y;
		}
	}
}

//初期化
void init() {
	for (int i = 0; i < n; i++) {
		int tyotenNum = polyIni[i].size();

		base[i] = Point(0, 0);
		for (int j = 0; j < tyotenNum; j++) {
			base[i].x += polyIni[i][j].x;
			base[i].y += polyIni[i][j].y;
		}
		base[i].x /= tyotenNum;
		base[i].y /= tyotenNum;
		rot[i] = 0.0;
		opFlag[i] = false;
		trans[i] = Point(0, 0);
	}
}

//加工用変数を弄る関数
void convert() {
	for (int i = 0; i < n; i++) {
		rot[i] = GetRand(628) / 100.0 - 3.1415926;
		trans[i].x = GetRand(201) - 100;
		trans[i].y = GetRand(201) - 100;
		//opFlag[i] = ((GetRand(2) == 1) ? true : false);
	}
}

//加工後のピースpoly[i]を作る関数
void makePoly() {
	
	for (int i = 0; i < n; i++) {
		//コピー
		poly[i] = polyIni[i];
		//回転 (重心を中心とした回転）
		for (int j = 0; j < poly[i].size(); j++) {
			double x = base[i].x + cos(rot[i]) * (poly[i][j].x - base[i].x) - sin(rot[i]) * (poly[i][j].y - base[i].y);
			double y = base[i].y + sin(rot[i]) * (poly[i][j].x - base[i].x) + cos(rot[i]) * (poly[i][j].y - base[i].y);
			poly[i][j].x = x;
			poly[i][j].y = y;
		}
		//反転（重心Xを軸とした反転）
		if (opFlag[i]) {
			for (int j = 0; j < poly[i].size(); j++) {
				poly[i][j].x = 2 * base[i].x - poly[i][j].x;
			}
		}
		//平行移動（足すだけ）
		for (int j = 0; j < poly[i].size(); j++) {
			poly[i][j].x += trans[i].x;
			poly[i][j].y += trans[i].y;
		}
		//頂点番号を時計回りから反時計回りに変換 (頂点0の位置が変わるので注意）, ただし反転処理をしている場合は既に反時計になっているので、この処理をしない。
		if (!opFlag[i]) {
			vector<Point> tmp;
			for (int j = poly[i].size() - 1; j >= 0; j--) {
				tmp.push_back(poly[i][j]);
			}
			for (int j = 0; j < poly[i].size(); j++) {
				poly[i][j] = tmp[j];
			}
		}
	}
}

//描画
void draw() {
	static int t = 0;
	int id = (t / 60) % n;

	//黒線
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < poly[i].size(); j++) {
			int x1 = poly[i][j].x;
			int y1 = poly[i][j].y;
			int x2 = poly[i][(j + 1) % poly[i].size()].x;
			int y2 = poly[i][(j + 1) % poly[i].size()].y;
			DrawLine(x1, y1, x2, y2, 0, 2);
		}
	}
	//緑線
	for (int j = 0; j < poly[id].size(); j++) {
		int x1 = poly[id][j].x;
		int y1 = poly[id][j].y;
		int x2 = poly[id][(j + 1) % poly[id].size()].x;
		int y2 = poly[id][(j + 1) % poly[id].size()].y;
		DrawLine(x1, y1, x2, y2, GetColor(0, 255, 0), 2);
	}
	//頂点番号
	for (int j = 0; j < poly[id].size(); j++) {
		int x = poly[id][j].x;
		int y = poly[id][j].y;
		DrawFormatString(x, y, GetColor(255, 0, 0), "%d", j);
	}
	t++;
}

//出力
void print() {
	ofstream ofs("probdata.txt");
	
	//ピース数
	ofs << n << endl;
	for (int i = 0; i < n; i++) {
		int tyotenNum = poly[i].size();
		
		//頂点数
		ofs << tyotenNum << endl;
		//x座標
		ofs << (int)poly[i][0].x;
		for (int j = 1; j < poly[i].size(); j++) {
			ofs << " " << (int)poly[i][j].x;
		}
		ofs << endl;
		//y座標
		ofs << (int)poly[i][0].y;
		for (int j = 1; j < poly[i].size(); j++) {
			ofs << " " << (int)poly[i][j].y;
		}
		ofs << endl;
		//改行
		ofs << endl;
	}
	ofs.close();
}