//回答プログラムで共通して使うデータ, 関数を集めました。
//ピースの移動等（回答）は、isputted[], piece[]を書き換えることで行います。

#pragma once
#include "Poly.h"
#include "Line.h"
#include "Vertex.h"

class Solver {
private:
	Poly get_scaledown_poly(Poly &poly, double dist);	//「polyの縮小多角形」返す

protected:
	vector<Poly> waku;			//枠の集合
	vector<Poly> piece;			//ピースの集合
	bool isputted[50];			//isputted[id] = piece[id]の位置が確定した⇔true
	vector<Line> putted_line;	//位置が確定した辺の集合
	int timer;					//計測用

	void move(Vertex src, Vertex dst);					//頂点srcを頂点dstにくっつけるように、移動する
	bool isoverlap(Poly &poly, double dist);			//当たり判定。distを大きくするほど「当たらなくなる」

public:
	virtual void solve() = 0;			//問題を解きます。（solveの中身はSolverの子クラスで書きます）
	void draw(bool isdraw_didnot_put);	//回答を表示します。
	Solver();							//初期化をします。
	void input(string filename);		//ファイルfilenameから問題を入力します。
	void print(string filename);		//ファイルfilenameへ回答を出力します。
};