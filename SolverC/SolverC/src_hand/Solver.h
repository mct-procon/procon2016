
//SolverBaseに機能を追加する。

#pragma once
#include "SolverBase.h"
#include "MargePoly.h"

class Solver : public SolverBase {
private:
	//多角形の結合
	MargePoly marge_poly;

public:
	//動かしてくっつける, くっつけるのが無理ならfalseを返す
	bool move(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id);	//ピースを移動
};