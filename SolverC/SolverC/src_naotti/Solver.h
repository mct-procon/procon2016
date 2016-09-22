
//SolverBaseに機能を追加する。

#pragma once
#include "SolverBase.h"
#include "MargePoly.h"

class Solver : public SolverBase {
private:
	//移動
	int moved_piece_id;
	Poly backup_piece;
	void move(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id);	//ピースを移動 (バックアップも取る)
	void restore();									//ピースを移動前の位置に戻す

	//評価
	double evaluation(Poly &poly1, Poly &poly2, bool is_poly2_piece);	//評価 O(頂点数1 * 頂点数2), 定数重い
	
	//「移動→評価→戻す」の一連動作をする関数
	double action_score(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id);

	//多角形の結合
	MargePoly marge_poly;

	//ベストな組でくっつける, くっつく組み合わせがないとき0を返す
	bool connect();

public:
	void solve();
};