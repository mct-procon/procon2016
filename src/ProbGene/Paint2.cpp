//wakudata.txt（入力は整数値です！）, を読み込む。それをデフォルトの線分とする。なければ無視。
//線分を画面に書いていく。-> sキークリックで、線分の集合をファイル[lines.txt]に出力。
//zキークリックで、1つ前に戻る

#include "DxLib.h"
#include <iostream>
#include <tuple>
#include <vector>
#include <fstream>
#include <cmath>
using namespace std;
typedef tuple<int, int, int, int> T;	//(始点x, 始点y) -> (終点x, 終点y)の順で格納

struct Point {
	int x, y;
	Point (int x, int y) { this->x = x; this->y = y; }
	Point () { }
};

//操作用
int bmouse, mouse;		//左クリックのみ。bmouseは1フレーム前, mouseは今の情報(1あり, 0なし)
Point s;				//始点を覚えておく

char bkey[256], key[256];	//キーの押下状態(0, 1)を持つ。bkeyは1フレーム前, keyは今の情報

//データ用
int defaultNum;		//デフォルトの線分(枠に使われている線分）の個数
vector<T> lines;

//プロトタイプ
void inputWaku();
void init();
void update();
void clicked();
void released();
void restore();
void draw();
void print(char *filename);

//メイン
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 600, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	inputWaku();
	init();

	// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ESCで終了)
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		update();
		if (bmouse == 0 && mouse == 1) { clicked(); }
		if (bmouse == 1 && mouse == 0) { released(); }
		if (mouse == 0 && bkey[KEY_INPUT_Z] == 0 && key[KEY_INPUT_Z] == 1) { restore(); }
		draw();
		if (bkey[KEY_INPUT_S] == 0 && key[KEY_INPUT_S] == 1) { print("lines.txt"); }
	}
	DxLib_End();
	return 0;
}

//枠に使われている線分を読み込む。整数値を読み込む。
//デフォルト線分の個数defaultNumもここで設定する。
void inputWaku() {
	defaultNum = 0;

	ifstream ifs("wakudata.txt");
	if (ifs.fail()) { return; }

	int wakuNum;
	ifs >> wakuNum;

	for (int i = 0; i < wakuNum; i++) {
		int tyotenNum;
		vector<Point> poly;
		ifs >> tyotenNum;
		poly.resize(tyotenNum);
		for (int j = 0; j < tyotenNum; j++) {
			ifs >> poly[j].x;
		}
		for (int j = 0; j < tyotenNum; j++) {
			ifs >> poly[j].y;
		}
		for (int j = 0; j < tyotenNum; j++) {
			int k = (j + 1) % tyotenNum;
			lines.push_back(T(poly[j].x, poly[j].y, poly[k].x, poly[k].y));
		}
	}
	defaultNum = lines.size();

	ifs.close();
}

//グローバル変数の初期化
void init() {
	mouse = 0;
	for (int i = 0; i < 256; i++) { key[i] = 0; }
}

//マウス・キーボード情報の更新
void update() {
	bmouse = mouse;
	mouse = GetMouseInput() & MOUSE_INPUT_LEFT;
	memcpy(bkey, key, 256);
	GetHitKeyStateAll(key);
}

//マウスをクリックしたときに呼ばれる関数
void clicked() {
	GetMousePoint(&s.x, &s.y);
}

//マウスを離したときに呼ばれる関数
void released() {
	Point g;
	GetMousePoint(&g.x, &g.y);
	if (abs(s.x - g.x) + abs(s.y - g.y) < 10) { return; }
	
	lines.push_back(T(s.x, s.y, g.x, g.y));
}

//最後に書いた線分を消すイベント(線分が無ければ何もしない)
void restore() {
	if (lines.size() > defaultNum) { lines.pop_back(); }
}

//線分の集合を描画する関数
void draw() {
	for (int i = 0; i < lines.size(); i++) {
		int x1 = get<0>(lines[i]);
		int y1 = get<1>(lines[i]);
		int x2 = get<2>(lines[i]);
		int y2 = get<3>(lines[i]);
		DrawLine(x1, y1, x2, y2, 0, 2);
	}
	if (mouse == 1) {
		Point g;
		GetMousePoint(&g.x, &g.y);
		DrawLine(s.x, s.y, g.x, g.y, 0, 2);
	}
}

//線分の集合をファイルに出力する関数
void print(char *filename) {
	ofstream ofs(filename);

	ofs << lines.size() << endl;
	for (int i = 0; i < lines.size(); i++) {
		int x1 = get<0>(lines[i]);
		int y1 = get<1>(lines[i]);
		int x2 = get<2>(lines[i]);
		int y2 = get<3>(lines[i]);
		ofs << x1 << " " << y1 << " " << x2 << " " << y2 << endl;
	}
}