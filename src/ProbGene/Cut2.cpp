//lines.txtから線分の集合を読み込み、加工をし、加工後のデータをlines_cuted.txtに保存する。線分は1000本以下であることが保障される。
//加工の手順：
//①平行な線分をマージする。これにより、平行な2線分が存在しないようにする。
//②各線分について交点を列挙する→交点が1個以下の線分は削除し、交点が2個以上の線分ははみ出す部分をカットする。 これを削除する線分・カットする線分がなくなるまで繰り返す。
//
//加工後のデータを画面に表示する。
//
//ps. (isExist[i]等の)変数値→表示を定義すれば、表示の変更→変数値の変更も考えることができます。(このように、別々の2つのものを入出力関係にすることを「関連付け」「モデル化」といいます。)
//例えば、isExist[i] = falseなら線分を非表示にする、と定義すれば、線分を非表示にしたいならisExist[i] = falseにすればよい、ということが分かります。
//誤差対策をします。(カット時にロングヘアー処理(勝手に命名)を行う)（無限ループに陥らないように、ループ回数を固定する)
//
//lines_cuted.txtには線分の座標値を、小数点6桁まで出力します。（ただし、誤差は結構あるかも…. ）. sキーを押すと出力する。

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
bool isExist[1000];
vector<Point> iPointLists[1000];	//iPointLists[i] = 線分iと実在する線分の交点, の集合

//プロトタイプ
bool input();				//データの入力と初期化
void margeParallelLine();	//平行な線分をマージする。これによって平行な2線分が存在しなくなる。

void set_iPointLists();		//線分の交点リストiPointLists[]を作成する
void sort_iPointLists();	//各線分の交点リストの点を(x昇順 -> y昇順)でソートする
bool eraseLines();			//交点が1個以下の線分を消す。消す線分が無ければfalseを返す。
bool cutLines();			//交点が2個以上の線分について、はみ出す部分をカットする。カットする線分が無ければfalseを返す。

void draw();
void print();

//プロトタイプ（小型関数）
bool isParallel(Point a, Point b, Point c, Point d);	//直線a -> b, c -> dの平行判定
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
	margeParallelLine();
	bool isContinue = true;
	for (int i = 0; i < 2 * n && isContinue; i++) {
		isContinue = false;
		set_iPointLists();
		sort_iPointLists();
		isContinue = isContinue || eraseLines();
		isContinue = isContinue || cutLines();
	}

	// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ESCで終了)
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		draw();
		if (CheckHitKey(KEY_INPUT_S)) { print(); }
	}
	DxLib_End();
	return 0;
}

//読み込み（と初期化）, 成功時true, 失敗時falseを返す
bool input() {
	ifstream ifs("lines.txt");
	if (ifs.fail()) { return false; }
	ifs >> n;
	for (int i = 0; i < n; i++) {
		int x1, y1, x2, y2;
		ifs >> x1 >> y1 >> x2 >> y2;
		lines[i] = L(Point(x1, y1), Point(x2, y2));
	}
	ifs.close();

	for (int i = 0; i < n; i++) {
		isExist[i] = true;
	}
	return true;
}

//平行な線分をマージする
void margeParallelLine() {
	int i, j;

	for (i = 0; i < n; i++) {
		if (!isExist[i]) { continue; }
		for (j = 0; j < n; j++) {
			if (i == j || !isExist[j]) { continue; }
			if (!isParallel(lines[i].first, lines[i].second, lines[j].first, lines[j].second)) { continue; }
			//線分i + 線分jを線分iに代入し、線分jを消す
			Point p[4] = {lines[i].first, lines[i].second, lines[j].first, lines[j].second};
			sort(p, p + 4);
			lines[i] = L(p[0], p[3]);
			isExist[j] = false;
		}
	}
}

//線分の交点リストiPointLists[]を作成する
void set_iPointLists() {
	int i, j;

	for (i = 0; i < n; i++) {
		iPointLists[i].clear();
	}

	for (i = 0; i < n; i++) {
		if (!isExist[i]) { continue; }
		for (j = 0; j < n; j++) {
			if (i == j || !isExist[j]) { continue; }
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

//交点が1個以下の線分を消す。消す線分が無ければfalseを返す。
bool eraseLines() {
	bool ret = false;
	for (int i = 0; i < n; i++) {
		if (!isExist[i]) { continue; }
		if (iPointLists[i].size() <= 1) { isExist[i] = false; ret = true; }
	}
	return ret;
}

//交点が2個以上の線分について、はみ出す部分(どの2つの交点間にも存在しない部分)をカットする。カットする線分が無ければfalseを返す。
//sort_iPointLists()を呼んでから呼び出すこと。
bool cutLines() {
	bool ret = false;
	for (int i = 0; i < n; i++) {
		if (!isExist[i]) { continue; }
		if (iPointLists[i].size() <= 1) { continue; }	//eraseLines()を呼んでなくても誤動作させない「おまじない☆」
		
		Point p[2] = {lines[i].first, lines[i].second};
		sort(p, p + 2);

		if (p[0] < iPointLists[i][0] || iPointLists[i][iPointLists[i].size() - 1] < p[1]) {
			ret = true;
			lines[i].first = iPointLists[i][0];
			lines[i].second = iPointLists[i][iPointLists[i].size() - 1];
			//ロングヘアー処理（端点を少しだけ伸ばす）(勝手に命名)
			if (lines[i].first.x == lines[i].second.x) {
				lines[i].first.y -= 1e-4;
				lines[i].second.y += 1e-4;
			}
			else {
				double a = (lines[i].first.y - lines[i].second.y) / (lines[i].first.x - lines[i].second.x);
				double eps = 1e-4 / hypot(a, 1.0);
				lines[i].first.x -= eps;
				lines[i].first.y -= eps * a;
				lines[i].second.x += eps;
				lines[i].second.y += eps * a;
			}
		}
	}
	return ret;
}

//直線a -> b, c -> dの平行判定
bool isParallel(Point a, Point b, Point c, Point d) {
	return (a.x - b.x) * (c.y - d.y) == (a.y - b.y) * (c.x - d.x);
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
	if (isParallel(a, b, c, d) || !isHit(a, b, c, d)) return Point(INF, INF);
	//三角形a -> c -> bの面積 * 2をs1, 三角形a -> d -> bの面積 * 2をs2とおく
	long double s1 = abs((a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x));
	long double s2 = abs((a.x - d.x) * (b.y - d.y) - (a.y - d.y) * (b.x - d.x));
	if (s1 == 0) { return Point(c.x, c.y); }
	if (s2 == 0) { return Point(d.x, d.y); }
	long double x = (s1 * d.x + s2 * c.x) / (s1 + s2);
	long double y = (s1 * d.y + s2 * c.y) / (s1 + s2);
	return Point((double)x, (double)y);
}

//描画
void draw() {
	for (int i = 0; i < n; i++) {
		if (!isExist[i]) { continue; }
		int x1 = lines[i].first.x;
		int y1 = lines[i].first.y;
		int x2 = lines[i].second.x;
		int y2 = lines[i].second.y;
		DrawLine(x1, y1, x2, y2, 0, 2);
		DrawFormatString(x1, y1, GetColor(0, 0, 255), "%d", i);
		DrawFormatString(x2, y2, GetColor(255, 0, 0), "%d", i);
	}
}

//出力
void print() {
	int cnt = 0;

	for (int i = 0; i < n; i++) {
		if (!isExist[i]) { continue; }
		cnt++;
	}

	ofstream ofs("lines_cuted.txt");
	
	ofs << cnt << endl;
	for (int i = 0; i < n; i++) {
		if (!isExist[i]) { continue; }
		ofs << fixed << setprecision(6) << lines[i].first.x << " " << lines[i].first.y << " " << lines[i].second.x << " " << lines[i].second.y << endl;
	}
}