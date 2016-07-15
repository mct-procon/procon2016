
#pragma once
#include "Poly.h"
#include "Line.h"

typedef pair<int, int> P;	//(poly_id, tyoten_id)

class Solver {
protected:
	vector<Poly> piece;
	vector<Poly> waku;

	bool isputted[50];						//isputted[id] = piece[id]の位置が確定している⇔true
	vector<Line> putted_line;				//位置が確定している辺の集合
	vector<Point> putted_point;				//位置が確定している頂点の集合
	queue<P> standard_point;				//基準となる頂点の集合

	Poly get_scaledown_poly(Poly poly);		//polyの縮小多角形を返す
	bool isoverlap(Poly poly);				//当たり判定。polyが既に置いたピース or 枠と重なる⇔true。
	int get_combi_level(Poly poly);			//結合度計算
	P get_best_point(P pointX);				//頂点X (多角形pointX.firstの頂点pointX.second)と最も結合する頂点Yを返す

public:
	void solve();
	void draw(bool isdraw_didnot_put);
	Solver();
	void input(string filename);
	void print(string filename);
};