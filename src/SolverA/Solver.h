
//☆のついている変数・関数は、このソルバー独特のものです。別解法を実装するときには、使用しない, もしくは引数が変わるかもしれません。
//〇のついている関数は、このソルバー独特の実装です。別解法を実装するときには、内部のコードが変わるかもしれません。
//今後は、Solverクラスを継承して各解法を実装する予定です。☆は子クラスで追加, 〇は(Solverクラス側では)virtualで宣言しておいて, 子クラスで実装、が望ましい。
//あと、プログラムは遠慮なく書き換えよう。

#pragma once
#include "Poly.h"
#include "Line.h"
#include "Vertex.h"

class Solver {
protected:
	vector<Poly> piece;		//ピースの集まり
	vector<Poly> waku;		//枠の集まり

	bool isputted[50];						//isputted[id] = piece[id]の位置が確定している⇔true
	vector<Line> putted_line;				//位置が確定している辺の集合 (☆）
	queue<Vertex> standard_point;			//基準となる頂点の集合（☆）

	void move(Vertex src, Vertex dst);					//頂点srcを頂点dstにくっつけるように、移動する
	Poly get_scaledown_poly(Poly &poly, double dist);	//「polyの縮小多角形」返す
	bool isoverlap(Poly &poly, double dist);			//当たり判定。polyが既に置いたピース or 枠と重なる⇔true。dist = 0が厳密, distを大きくするほど「当たらなくなる」

	bool is_combi_vertex(Vertex piece_v, Vertex dst, double error_dist, double error_angle);	//頂点がくっついているか？ (くっついていればtrueを返す）
	int get_combi_level(Vertex src, double error_dist, double error_angle);						//結合度計算（☆）
	Vertex get_best_vertex(Vertex dst);															//頂点dstと最も結合する頂点srcを返す（☆）

public:
	void solve();						//問題を解きます。（〇）
	void draw(bool isdraw_didnot_put);	//回答を表示します。（〇）
	Solver();							//初期化をします。（〇）
	void input(string filename);		//ファイルfilenameから問題を入力します。
	void print(string filename);		//ファイルfilenameへ回答を出力します。

	int timer;	//計測用
};