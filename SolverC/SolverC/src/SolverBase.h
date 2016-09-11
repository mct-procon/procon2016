//基本機能

#pragma once
#include "Line.h"
#include "Poly.h"
#include "DxLib.h"

class SolverBase {
private:
	double dist;		//当たり判定の緩さ（distが大きいほど置きやすくなる. dist > 0とすること. dist = 5がちょうど良いかも. dist値のセットをできるのはinput()関数のみ.）

	void draw_poly(Poly poly, int red, int green, int blue, double scale);
	Poly read_poly(ifstream &ifs, bool is_piece);
	void print_poly(ofstream &ofs, Poly poly);

	vector<int> get_contain_piece_id(Poly waku);

protected:
	vector<Poly> pieces;	//反時計回りの頂点列
	vector<Poly> wakus;		//時計回りの頂点列
	vector<Poly> wakusIni;	//時計回りの頂点列
	double scale;			//表示スケール

	Poly get_scaledown_poly(Poly poly);
	void move(Poly &src, int src_id, bool src_turnmode, Poly &dst, int dst_id, bool dst_turnmode);
	bool canput(Poly poly);		//当たり判定。dist=0で厳密, distが大きいほど置きやすくなる
	void make_answer();

public:
	SolverBase();
	virtual void solve() = 0;
	void draw(bool is_draw_didnot_put);
	void input(string filename, double dist);
	void print(string filename);
};