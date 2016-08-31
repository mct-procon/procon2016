//多角形クラスの宣言

#pragma once
#include "Point.h"
#include "Line.h"

class Poly {
private:
	Point point0;				//頂点0の座標（point0.real(), point0.imag()）
	vector<Point> point;		//頂点0に対する頂点0, 1, 2, …, n - 1の座標 (枠の場合は時計周り, ピースの場合は反時計周り）
	bool is_piece_;				//ピースならtrue, 枠ならfalse
	int turn_cnt;				//turn()を実行した回数

	void update();				//外接長方形の情報を更新

public:
	bool is_putted;			//当たり判定に参加するならtrue（置き場所が確定したならtrue）
	double lx, ly, rx, ry;	//Polyの外接長方形. 当たり判定高速化用. (オブジェクトの外から書き込まないこと）

	Poly(Point point0, vector<Point> point, bool is_piece_);
	Poly();

	int size();						//頂点数
	Point get_point(int id);		//点の絶対位置
	double get_angle_deg(int id);	//角度 (枠なら外角が出る）

	void move(int id, Point s, Point e);				//移動
	void move2(Point mul, Point center, Point trans);	//回転ベクトル, 回転の中心, 平行移動量を指定して移動。回答生成時のみ使用。
	void turn();										//反転
	void turn2(double center_imag);						//反転 (軸を指定). 回答生成時のみ使用。

	bool is_hit(Poly &poly);			//polyと当たっているか？
	bool is_contain(Poly &poly);		//polyを含んでいるか？
	bool is_contained(Poly &poly);		//polyに含まれているか？

	double area();			//多角形Polyの符号付き面積 (正…反時計まわり, 負…時計回り）
	bool is_turned();		//turn_cntが奇数ならtrueを返す

	void point_reverse();	//頂点列の向きを強制的に変更する
	bool is_piece();		//ピースかどうかを判定

	Line get_line(int id);
};
