//lines_cuted.txtから線分の集合を読み込む。平行な2線分は存在せず、線分のカット処理も不要となっている。
//ここでは、3つ以上交点を持つ線分を、交点数 - 1本の線分に分割する加工を行う。
//加工後の線分の集合をlines_splited.txtに書き込む。
//分割後の線分の本数は1000本以下であることが保障されている。

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
vector<Point> iPointLists[1000];	//iPointLists[i] = 線分iと実在する線分の交点, の集合

//制御用
int t;

//プロトタイプ
bool input();				//データの入力と初期化
void set_iPointLists();		//線分の交点リストiPointLists[]を作成する
void sort_iPointLists();	//各線分の交点リストの点を(x昇順 -> y昇順)でソートする
vector<L> split_lines();	//線分を分割する処理。分割後の線分集合を返す。
void draw();
void print();

//プロトタイプ（小型関数）
bool isHit(Point a, Point b, Point c, Point d);			//線分a -> b, c -> dの交差判定
Point getIPoint(Point a, Point b, Point c, Point d);	//線分a -> b, c -> dの交点を返す（無い場合は、(INF, INF)を返す）

//メイン
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 600, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	input();
	set_iPointLists();
	sort_iPointLists();
	vector<L> spLines = split_lines();
	n = spLines.size();
	for (int i = 0; i < n; i++) {
		lines[i] = spLines[i];
	}
	//sort(lines, lines + n);

	// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ESCで終了)
	t = 0;
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		draw();
		if (CheckHitKey(KEY_INPUT_S)) { print(); }
		t++;
	}
	DxLib_End();
	return 0;
}

//読み込み（と初期化）, 成功時true, 失敗時falseを返す
bool input() {
	ifstream ifs("lines_cuted.txt");
	if (ifs.fail()) { return false; }
	ifs >> n;
	for (int i = 0; i < n; i++) {
		double x1, y1, x2, y2;
		ifs >> x1 >> y1 >> x2 >> y2;
		lines[i] = L(Point(x1, y1), Point(x2, y2));
	}
	return true;
}

//線分の交点リストiPointLists[]を作成する
void set_iPointLists() {
	int i, j;

	for (i = 0; i < n; i++) {
		iPointLists[i].clear();
	}

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			if (i == j) { continue; }
			Point p = getIPoint(lines[i].first, lines[i].second, lines[j].first, lines[j].second);	//交点p
			if (p.x == INF && p.y == INF) { continue; }	//交点が存在しない
			iPointLists[i].push_back(p);	//線分iの交点リストを更新
		}
	}
}

//各線分の交点リストの点を(x昇順 -> y昇順)でソートする
void sort_iPointLists() {
	for (int i = 0; i < n; i++) {
		sort(iPointLists[i].begin(), iPointLists[i].end());
	}
}

//ソート後の交点リストを用いて、線分を分割する。分割後の線分集合を返す。
vector<L> split_lines() {
	vector<L> spLines;

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < iPointLists[i].size() - 1; j++) {
			spLines.push_back(L(iPointLists[i][j], iPointLists[i][j+1]));
		}
	}
	return spLines;
}

//描画
void draw() {
	for (int i = 0; i < n; i++) {
		int x1 = lines[i].first.x;
		int y1 = lines[i].first.y;
		int x2 = lines[i].second.x;
		int y2 = lines[i].second.y;
		DrawLine(x1, y1, x2, y2, 0, 2);
		if ((t / 60) % n == i) {
			DrawLine(x1, y1, x2, y2, GetColor(0, 255, 0), 2);
			DrawFormatString(x1, y1, GetColor(0, 0, 255), "%d", i);
			DrawFormatString(x2, y2, GetColor(255, 0, 0), "%d", i);
		}
	}
}

//出力
void print() {
	ofstream ofs("lines_splited.txt");

	ofs << n << endl;
	for (int i = 0; i < n; i++) {
		ofs << fixed << setprecision(6) << lines[i].first.x << " " << lines[i].first.y << " " << lines[i].second.x << " " << lines[i].second.y << endl;
	}
}

//線分a -> b, c -> dの交差判定 (T字も交差とみなす)
bool isHit(Point a, Point b, Point c, Point d) {
	long double u, v;
	//0 = (a.y - b.y)(x - a.x) + (a.x - b.x)(a.y - y)の両側にc, dがあるか
	u = (a.y - b.y) * (c.x - a.x) + (a.x - b.x) * (a.y - c.y);
	v = (a.y - b.y) * (d.x - a.x) + (a.x - b.x) * (a.y - d.y);
	if (u * v > 0) { return false; }
	//0 = (c.y - d.y)(x - c.x) + (c.x - d.x)(c.y - y)の両側にa, bがあるか
	u = (c.y - d.y) * (a.x - c.x) + (c.x - d.x) * (c.y - a.y);
	v = (c.y - d.y) * (b.x - c.x) + (c.x - d.x) * (c.y - b.y);
	if (u * v > 0) { return false; }
	return true;
}

//線分a -> b, c -> dの交点を返す（無い場合は、(INF, INF)を返す）
Point getIPoint(Point a, Point b, Point c, Point d) {
	if (!isHit(a, b, c, d)) return Point(INF, INF);
	//三角形a -> c -> bの面積 * 2をs1, 三角形a -> d -> bの面積 * 2をs2とおく
	long double s1 = abs((a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x));
	long double s2 = abs((a.x - d.x) * (b.y - d.y) - (a.y - d.y) * (b.x - d.x));
	if (s1 == 0) { return Point(c.x, c.y); }
	if (s2 == 0) { return Point(d.x, d.y); }
	long double x = (s1 * d.x + s2 * c.x) / (s1 + s2);
	long double y = (s1 * d.y + s2 * c.y) / (s1 + s2);
	return Point((double)x, (double)y);
}