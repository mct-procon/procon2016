
//SolverBaseに機能を追加する。

#pragma once
#include "SolverBase.h"
#include "MargePoly.h"

class Solver : public SolverBase {
private:
	//移動
	int moved_piece_id;
	Poly backup_piece;
	void move(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line);	//ピースを移動
	void restore();			//ピースを元の位置に戻す

	//評価, （当たり判定の計算量は, 前処理をすることで, ピースの個数倍くらい落とすことができます）
	double evaluation(Poly &poly1, Poly &poly2, bool is_poly2_piece);	//評価 O(頂点数1 * 頂点数2), 定数重い

	//「移動→評価→戻す」の一連動作をする関数
	double action_score(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line);

	//多角形の結合
	MargePoly marge_poly_obj;
	bool marge_poly(int src_poly_id, int dst_poly_id, bool dst_is_piece);		//2つの多角形をくっつける→成功時はtrue, 失敗時はfalseを返す。

	//ベストな組でくっつける, くっつく組み合わせがないとき0を返す
	bool connect();

public:
	//グローバル変数のpieces, wakusを変化させながら、問題を解いていく。
	void solve();
};