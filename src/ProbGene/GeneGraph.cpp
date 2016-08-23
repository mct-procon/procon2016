//lines_splited.txtを読み込んで、頂点と辺のグラフを構築する。グラフをlines_graph.txtに出力する。
//フォーマット：頂点数N→頂点の座標(x, y)→辺数M→辺の接続状態（src頂点id(>=0), dst頂点id(>=0))
//頂点座標は、小数点4桁まで出力する。
//線分の本数は1000本以下であることが保障される。

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

struct Point {
	double x, y;
	Point (double x, double y) { this->x = x; this->y = y; }
	Point () { }
	bool operator<(const Point &r) const { return (x != r.x) ? x < r.x : y < r.y; }
};
typedef pair<Point, Point> L;	//始点 -> 終点の順で格納

//データ用
int n;
L lines[1000];

int gNum;		//グループの数
int gid[2000];	//lines[i].firstのグループ番号がgid[2*i]に、lines[i].secondのグループ番号がgid[2*i + 1]に入る。

//プロトタイプ
bool input();			//データの入力と初期化
void makeGroup();		//linesの各頂点に番号を割り振る。近い位置にある頂点に同じグループ番号を割り振る。
vector<Point> makeTyoten();
vector<pair<int, int>> makeGraph();
void print(vector<Point> v, vector<pair<int, int>> e);

void draw(vector<Point> v, vector<pair<int, int>> e);	//グラフの描画

//メイン
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 600, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	input();
	makeGroup();

	vector<Point> v = makeTyoten();
	vector<pair<int, int> > e = makeGraph();
	print(v, e);

	// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ESCで終了)
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		draw(v, e);
	}
	DxLib_End();
	return 0;
}

//読み込み（と初期化）, 成功時true, 失敗時falseを返す
bool input() {
	ifstream ifs("lines_splited.txt");
	if (ifs.fail()) { return false; }
	ifs >> n;
	for (int i = 0; i < n; i++) {
		double x1, y1, x2, y2;
		ifs >> x1 >> y1 >> x2 >> y2;
		lines[i] = L(Point(x1, y1), Point(x2, y2));
	}
	return true;
}

//linesの各頂点に番号を割り振る。近い位置にある頂点に同じグループ番号を割り振る。
//各グループの代表元は頂点番号（lines[i].firstならi * 2, lines[i].secondならi * 2 + 1)が最も小さいもの！
void makeGroup() {
	int i, j;

	for (i = 0; i < 2 * n; i++) { gid[i] = -1; }

	int id = 0;
	for (i = 0; i < 2 * n; i++) {
		if (gid[i] != -1) { continue; }

		Point p1 =(i % 2) ? lines[i / 2].second : lines[i / 2].first;
		gid[i] = id;
		for (j = i + 1; j < 2 * n; j++) {
			Point p2 = (j % 2) ? lines[j / 2].second : lines[j / 2].first;
			if (hypot(p1.x - p2.x, p1.y - p2.y) <= 1e-4) {	//1e-3にしたほうがよいかも…
				gid[j] = id;
			}
		}
		id++;
	}
	gNum = id;
}

//グループ番号iの頂点の（代表元の）位置をret[i]に入れて、retを返す
//各グループの代表元は頂点番号（lines[i].firstならi * 2, lines[i].secondならi * 2 + 1)が最も小さいもの！
vector<Point> makeTyoten() {
	vector<Point> ret;

	ret.resize(gNum);
	for (int i = 2 * n  - 1; i >= 0; i--) {
		ret[gid[i]] = (i % 2) ? lines[i / 2].second : lines[i / 2].first;
	}
	return ret;
}

//グループ番号でまとめた頂点についてグラフを構築する。(srcグループ番号 -> dstグループ番号という感じで！）
vector<pair<int, int>> makeGraph() {
	vector<pair<int, int>> ret;

	for (int i = 0; i < n; i++) {
		//lines[i].first と lines[i].secondを双方向でつなぐので…
		//lines[i].firstのグループ番号(gid[2 * i])とlines[i].secondのグループ番号(gid[2 * i + 1])を双方向につなげばよい
		ret.push_back(pair<int, int>(gid[2 * i], gid[2 * i + 1]));
		ret.push_back(pair<int, int>(gid[2 * i + 1], gid[2 * i]));
	}
	return ret;
}

//グラフの出力
void print(vector<Point> v, vector<pair<int, int>> e) {
	ofstream ofs("lines_graph.txt");
	
	ofs << v.size() << endl;
	for (int i = 0; i < v.size(); i++) {
		ofs << setprecision(4) << fixed << v[i].x << " " << v[i].y << endl;
	}
	ofs << e.size() << endl;
	for (int i = 0; i < e.size(); i++) {
		ofs << e[i].first << " " << e[i].second << endl;
	}
}

//グラフの描画
void draw(vector<Point> v, vector<pair<int, int>> e) {
	
	for (int i = 0; i < e.size(); i++) {
		Point s = v[e[i].first];
		Point g = v[e[i].second];
		DrawLine(s.x, s.y, g.x, g.y, 0, 2);
	}
	
	static int t = 0;
	for (int i = 0; i < v.size(); i++) {
		if (i == (t / 30) % (v.size())) {
			DrawFormatString(v[i].x, v[i].y, GetColor(255, 0, 0), "%d", i);
		}
	}
	t++;
}
