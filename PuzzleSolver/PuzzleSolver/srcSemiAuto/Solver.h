
//SolverBaseに機能を追加する。

#pragma once
#include "SolverBase.h"
#include "MargePoly.h"
#include "SolverParameter.h"
#include "MoveNode.h"


class Solver : public SolverBase {
private:
	//移動の仕方を全列挙する関数
	MoveNode get_hand_moving(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line);
	vector<MoveNode> get_semiAuto_moving(bool is_turn_ok, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line);
	vector<MoveNode> get_auto_moving(bool is_turn_ok);

	//移動
	int moved_piece_id;
	Poly backup_piece;
	void move(MoveNode &move_node);	//ピースを移動
	void restore();					//ピースを元の位置に戻す

	//評価, （当たり判定の計算量は, 前処理をすることで, ピースの個数倍くらい落とすことができます）
	double evaluation(Poly &poly1, Poly &poly2, bool is_poly2_piece);	//評価 O(頂点数1 * 頂点数2), 定数重い

	//「移動→評価→戻す」の一連動作をする関数
	double action_score(MoveNode &move_node);

	//多角形の結合
	MargePoly marge_poly_obj;
	bool can_marge(int src_poly_id, int dst_poly_id, bool dst_is_piece);		//2つの多角形をくっつけられるかだけ判定する
	bool marge_poly(int src_poly_id, int dst_poly_id, bool dst_is_piece);		//2つの多角形をくっつける→成功時はtrue, 失敗時はfalseを返す。

public:
	//くっつけ方を「よさげな順番で」リストアップする。リストの最大長はmax_moveList_size. 反転無しで解きたい場合は、is_turn_okにfalseを指定すること。
	vector<MoveNode> connect_auto(bool is_turn_ok, int max_moveList_size);
	vector<MoveNode> connect_semiAuto(bool is_turn_ok, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line, int max_moveList_size);

	//手動でくっつける。成功時はtrue, 失敗時はfalseを返す。
	bool connect_hand(MoveNode move_node);
};