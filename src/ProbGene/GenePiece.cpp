//lines_graph.txtを読み込んで、ピースデータを作る。ピースデータをpiecedata.txtに出力する。
//フォーマットは適当, 座標値は小数点以下4桁で出力する。今回、ピースの頂点0を位置0に合わせる処理は不要とする。
//sキーを押したら、ピースデータのファイル出力を開始する。

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
int n;						//グラフの頂点の数
Point pos[1000];			//pos[i] = 頂点iの座標
int m;						//グラフの辺の数
vector<int> et[1000];		//et[i][j] = 頂点iのj番目の接続先が頂点et[i][j]
vector<double> eang[1000];	//eang[i][j] = 辺(頂点i→頂点et[i][j])の角度をラジアン表記したもの

//プロトタイプ
bool input();
vector<vector<int>> search();
void draw(vector<vector<int>> polyList);
void print(vector<vector<int>> polyList);	//ピースデータをpiecedata.txtに出力する

//小型関数
int selectId(double ang, int v);
vector<Point> toPoints(vector<int> pointId);	//多角形は頂点番号(pointId[0], pointId[1], …, )で構成される
double getArea(vector<Point> points);			//頂点points[0] -> points[1]…の順でつなげた多角形の符号付き面積を返す（<0なら時計回り）

//メイン
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 600, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	input();
	vector<vector<int>> res = search();

	/*for (int i = 0; i < res.size(); i++) {
		for (int j = 0; j < res[i].size(); j++) {
			printfDx("%d ", res[i][j]);
		}
		printfDx("\n");
	}*/

	// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ESCで終了)
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		draw(res);
		if (CheckHitKey(KEY_INPUT_S)) { print(res); }
	}
	DxLib_End();
	return 0;
}

//読み込み（と初期化）, 成功時true, 失敗時falseを返す
bool input() {
	ifstream ifs("lines_graph.txt");
	if (ifs.fail()) { return false; }
	
	ifs >> n;
	for (int i = 0; i < n; i++) {
		ifs >> pos[i].x >> pos[i].y;
	}
	ifs >> m;
	for (int i = 0; i < m; i++) {
		int src, dst;
		ifs >> src >> dst;
		et[src].push_back(dst);
		eang[src].push_back(atan2(pos[dst].y - pos[src].y, pos[dst].x - pos[src].x));	//src -> dst
	}
	return true;
}

//ret[i][j]には多角形iの頂点jにあたる頂点の番号を入れる。retを返す。
vector<vector<int>> search() {
	vector<vector<int>> ret;
	bool used[1000] = {false};
	bool used2[1000];
	int i, j;

	for (i = 0; i < n; i++) {
		if (used[i]) { continue; }

		//最初の辺を探索する
		for (j = 0; j < et[i].size(); j++) {
			vector<int> cycle;
			for (int k = 0; k < n; k++) {
				used2[k] = false;
			}
			used2[i] = true;
			cycle.push_back(i);

			int v = et[i][j];			//頂点番号はv
			double ang = eang[i][j];	//辺の角度はeang[i][j]
			while (!used2[v]) {
				used2[v] = true;
				cycle.push_back(v);
				int id = selectId(ang, v);
				ang = eang[v][id];
				v = et[v][id];
			}
			if (v != i) { continue; }
			if (getArea(toPoints(cycle)) >= 0) { continue; }
			//頂点iを使う多角形を登録
			ret.push_back(cycle);
			break;
		}
		//cycleに使った頂点のフラグをtrueにする
		for (j = 0; j < n; j++) {
			used[j] |= used2[j];
		}
	}
	return ret;
}

//直前の移動方向がangで、今頂点vにいるとき、どの辺で移動するかを返す。
int selectId(double ang, int v) {
	int i;
	double minEval = INF;
	int selectId = -1;

	//angを反転します
	ang = ang + PAI;
	if (ang > PAI) { ang -= 2 * PAI; }

	//angと、vから出る辺の方向eang[v][i]を比較します。
	//angから反時計まわりに見ていってもっとも最初にあたる方向を探す. ただしangと同じ向きは無視したい.
	for (i = 0; i < eang[v].size(); i++) {
		if (fabs(ang -eang[v][i]) <= 1e-5) {
			continue;
		}
		double eval = (eang[v][i] < ang) ? eang[v][i] - ang + 2 * PAI : eang[v][i] - ang;
		if (eval < minEval) {
			minEval = eval;
			selectId = i;
		}
	}

	return selectId;
}

//頂点座標を返す
vector<Point> toPoints(vector<int> pointId) {
	vector<Point> ret;

	for (int i = 0; i < pointId.size(); i++) {
		ret.push_back(pos[pointId[i]]);
	}
	return ret;
}

//頂点points[0] -> points[1]…の順でつなげた多角形の符号付き面積を返す（<0なら時計回り）
double getArea(vector<Point> points) {
	double area = 0;

	for (int i = 0; i < points.size(); i++) {
		int j = (i + 1) % points.size();
		area += points[i].x * points[j].y - points[j].x * points[i].y;
	}
	area *= 0.5;
	return area;
}

//描画
void draw(vector<vector<int>> polyList) {
	static int t = 0;
	
	//普通の表示
	for (int i = 0; i < polyList.size(); i++) {	//i個目の多角形
		for (int j = 0; j < polyList[i].size(); j++) {	//j個目の頂点
			int src = polyList[i][j];
			int dst = polyList[i][(j + 1) % polyList[i].size()];

			DrawLine(pos[src].x, pos[src].y, pos[dst].x, pos[dst].y, GetColor(0, 0, 0), 2);
		}
	}

	//多角形の表示
	int id = (t / 90) % polyList.size();
	for (int j = 0; j < polyList[id].size(); j++) {	//j個目の頂点
		int src = polyList[id][j];
		int dst = polyList[id][(j + 1) % polyList[id].size()];

		DrawLine(pos[src].x, pos[src].y, pos[dst].x, pos[dst].y, GetColor(0, 255, 0), 2);
	}

	//頂点番号の表示
	for (int j = 0; j < polyList[id].size(); j++) {	//j個目の頂点
		int src = polyList[id][j];
		DrawFormatString(pos[src].x, pos[src].y, GetColor(255, 0, 0), "%d", j);
	}
	t++;
}

//ピースデータをpiecedata.txtに出力する
//整数で、小数点は四捨五入。
void print(vector<vector<int>> polyList) {
	ofstream ofs("piecedata.txt");
	
	ofs << polyList.size() << endl;
	for (int i = 0; i < polyList.size(); i++) {
		//ピースの頂点数
		ofs << polyList[i].size() << endl;
		//x座標
		ofs << fixed << setprecision(4) << pos[polyList[i][0]].x;
		for (int j = 1; j < polyList[i].size(); j++) {
			int src = polyList[i][j];
			ofs << " " << fixed << setprecision(4) << pos[src].x;
		}
		ofs << endl;
		//y座標
		ofs << fixed << setprecision(4) << pos[polyList[i][0]].y;
		for (int j = 1; j < polyList[i].size(); j++) {
			int src = polyList[i][j];
			ofs << " " << fixed << setprecision(4) << pos[src].y;
		}
		ofs << endl;
		//改行
		ofs << endl;
	}
	ofs.close();
}