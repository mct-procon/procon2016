//多角形クラスの宣言

#pragma once
#include "Point.h"
#include "Line.h"

class Poly {
private:
	void update_rect();			//外接長方形の情報を更新

public:
	vector<Point> points;		//頂点0に対する頂点0, 1, 2, …, n - 1, 0の座標 (枠の場合は時計周り, ピースの場合は反時計周り）
	vector<Line> lines;			//表示する線 (順不同)
	bool is_turn;				//最初から見て, ひっくり返った状態ならtrueを返す.
	double lx, ly, rx, ry;		//Polyの外接長方形. 当たり判定高速化用.

	//初期化
	Poly();
	Poly(vector<Point> points, vector<Line> lines);

	//更新
	void point_reverse();								//頂点列の向きを強制的に変更する
	void move(int id, Point s, Point e);				//移動（半直線s->eにpoint[id]->point[id+1]をくっつける)
	void move_reverse(int id, Point s, Point e);		//移動 (半直線e->sにpoint[id+1]->point[id]をくっつける)
	void turn();										//反転 (絶対座標における, y = point[0].imag()を中心)
	
	//取得
	int size();						//頂点数 (points.size() - 1)
	double area();					//多角形Polyの符号付き面積 (正…反時計まわり, 負…時計回り）
	double angle(int point_id);

	bool is_hit(Poly &poly);		//polyと当たっているか？
	bool is_cover_point(Point &p);	//点pを含んでいるか？
	bool is_cover(Poly &poly);		//polyを含んでいるか？（is_hit(poly) == falseのときは正しい結果を返す）
};
