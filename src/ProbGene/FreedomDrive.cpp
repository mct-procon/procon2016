//全部一つにまとめた。→で次の工程に進み、←で1つ前に戻る。
//コード：キーボード情報の更新（keyboard.update())があらゆる位置に書かれていて害悪である。キーボードやマウスのクリック判定をもっときれいに書けないものだろうか…

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

//キーボード (全クラス共通で用いるオブジェクト, keyboardを用意する）
class Keyboard {
	char bkey[256], key[256];
public:
	Keyboard() {
		for (int i = 0; i < 256; i++) bkey[i] = 0;
	}
	void update() {
		for (int i = 0; i < 256; i++) bkey[i] = key[i];
		GetHitKeyStateAll(key);
	}
	bool isClick(int code) {
		return (bkey[code] == 0 && key[code] == 1);
	}
}keyboard;

//空のMainがある、基底クラス
class Project {
public:
	virtual int Main() = 0;
};

//工程1. ペイント
class MyPaint : public Project {
private:
	typedef tuple<int, int, int, int> T;	//(始点x, 始点y) -> (終点x, 終点y)の順で格納

	struct Point {
		int x, y;
		Point (int x, int y) { this->x = x; this->y = y; }
		Point () { }
	};

	//操作用
	int bmouse, mouse;		//左クリックのみ。bmouseは1フレーム前, mouseは今の情報(1あり, 0なし)
	Point s;				//始点を覚えておく

	//データ用
	int defaultNum;		//デフォルトの線分(枠に使われている線分）の個数
	vector<T> lines;

	//枠に使われている線分を読み込む。整数値を読み込む。
	//デフォルト線分の個数defaultNumもここで設定する。
	void inputWaku() {

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
		lines.clear();
	}

	//マウス・キーボード情報の更新
	void update() {
		bmouse = mouse;
		mouse = GetMouseInput() & MOUSE_INPUT_LEFT;
		keyboard.update();
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

public:
	//メイン
	int Main() {
		init();
		inputWaku();

		// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ←→で終了)
		while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && !keyboard.isClick(KEY_INPUT_LEFT) && !keyboard.isClick(KEY_INPUT_RIGHT)) {
			update();
			if (bmouse == 0 && mouse == 1) { clicked(); }
			if (bmouse == 1 && mouse == 0) { released(); }
			if (mouse == 0 && keyboard.isClick(KEY_INPUT_Z)) { restore(); }
			draw();
		}
		print("lines.txt");
		return ProcessMessage();
	}
};

//工程2. 線分カット
class Cut : public Project {
private:
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

public:
	//メイン
	int Main() {
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

		// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ←→で終了)
		while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && !keyboard.isClick(KEY_INPUT_LEFT) && !keyboard.isClick(KEY_INPUT_RIGHT)) {
			keyboard.update();
			draw();
		}
		print();
		return ProcessMessage();
	}
};

//工程3. 線分スプリット
class Split : public Project {
private:
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

public:
	//メイン
	int Main() {
		input();
		set_iPointLists();
		sort_iPointLists();
		vector<L> spLines = split_lines();
		n = spLines.size();
		for (int i = 0; i < n; i++) {
			lines[i] = spLines[i];
		}
		//sort(lines, lines + n);

		// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ←→で終了)
		t = 0;
		while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && !keyboard.isClick(KEY_INPUT_LEFT) && !keyboard.isClick(KEY_INPUT_RIGHT)) {
			keyboard.update();
			draw();
			t++;
		}
		print();
		return ProcessMessage();
	}
};

//工程4. グラフ化
class GeneGraph : public Project {
private:
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
				if (hypot(p1.x - p2.x, p1.y - p2.y) <= 1e-4) {
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

public:
	//メイン
	int Main() {
		input();
		makeGroup();

		vector<Point> v = makeTyoten();
		vector<pair<int, int> > e = makeGraph();
		print(v, e);

		// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ←→で終了)
		while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && !keyboard.isClick(KEY_INPUT_LEFT) && !keyboard.isClick(KEY_INPUT_RIGHT)) {
			keyboard.update();
			draw(v, e);
		}
		return ProcessMessage();
	}
};

//工程5. ピースの生成 (頂点列は時計回り, 表示は(y軸が下向きなので)反時計回りになる）
class GenePiece : public Project {
private:
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

	//読み込み（と初期化）, 成功時true, 失敗時falseを返す
	bool input() {
		ifstream ifs("lines_graph.txt");
		if (ifs.fail()) { return false; }

		ifs >> n;
		for (int i = 0; i < n; i++) {
			ifs >> pos[i].x >> pos[i].y;
		}
		for (int i = 0; i < n; i++) {
			et[i].clear();
			eang[i].clear();
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
		bool used[1000][1000];		//辺の使用フラグ
		bool used2[1000];			//頂点の使用フラグ (一時保管用)
		int i, j;

		for (i = 0; i < n; i++) { for (j = 0; j < n; j++) { used[i][j] = false; } }
		
		while (true) {
			//最初の辺を探索する
			for (i = 0; i < n; i++) {
				for (j = 0; j < et[i].size(); j++) {
					if (used[i][et[i][j]]) continue;
					
					//未使用辺を見つけたので、ピースを作ってみる
					vector<int> cycle;
					int v = et[i][j];
					double ang = eang[i][j];
					for (int k = 0; k < n; k++) { used2[k] = false; }
					
					used2[i] = true;
					cycle.push_back(i);
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
					//cycleに使った辺 (有向辺) のフラグをtrueにする
					for (int k = 0; k < cycle.size(); k++) {
						if (k + 1 == cycle.size()) {
							used[cycle[k]][cycle[0]] = true;
						}
						else {
							used[cycle[k]][cycle[k+1]] = true;
						}
					}
					break;

				}
				if (j < et[i].size()) break;
			}
			if (i == n) break;
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

public:
	//メイン
	int Main() {
		input();
		vector<vector<int>> res = search();

		/*for (int i = 0; i < res.size(); i++) {
		for (int j = 0; j < res[i].size(); j++) {
		printfDx("%d ", res[i][j]);
		}
		printfDx("\n");
		}*/

		// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ←→で終了)
		while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && !keyboard.isClick(KEY_INPUT_LEFT) && !keyboard.isClick(KEY_INPUT_RIGHT)) {
			keyboard.update();
			draw(res);
		}
		print(res);
		return ProcessMessage();
	}
};

//工程6. 問題（ピースのみ）の生成, 頂点列は反時計回り（表示としては時計回り）
class GeneProb : public Project {
private:
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
			poly[i].clear();
		}
	}

	//加工用変数を弄る関数
	void convert() {
		for (int i = 0; i < n; i++) {
			//rot[i] = GetRand(628) / 100.0 - 3.1415926;
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

public:
	//メイン
	int Main() {
		input();
		init();
		convert();
		makePoly();

		// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ←→で終了)
		while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && !keyboard.isClick(KEY_INPUT_LEFT) && !keyboard.isClick(KEY_INPUT_RIGHT)) {
			keyboard.update();
			draw();
		}
		print();
		return ProcessMessage();
	}
};

//C++のオブジェクトは参照ではなく実体！！ ポリモーフィズムをしたければ、ポインターを使え！！
MyPaint paint;
Cut cut;
Split split;
GeneGraph gene_graph;
GenePiece gene_piece;
GeneProb gene_prob;

int stage = 0;	//今の、工程を表す番号
Project *p[6] = {&paint, &cut, &split, &gene_graph, &gene_piece, &gene_prob};	//「Projectクラスの子クラスの実体」の場所を配列に入れておく。

//全体メイン
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 600, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ESCで終了)
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		keyboard.update();
		if (p[stage]->Main() != 0) break;
		if (keyboard.isClick(KEY_INPUT_LEFT) && stage > 0) stage--;
		if (keyboard.isClick(KEY_INPUT_RIGHT)) stage++;
		if (stage > 5) break;
	}

	DxLib_End();
	return 0;
}