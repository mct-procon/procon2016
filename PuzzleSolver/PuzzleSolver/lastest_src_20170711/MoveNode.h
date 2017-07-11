//ピースのくっつけ方（7パラメータ）

#pragma once
#include "stdafx.h"
using namespace std;


class MoveNode {
public:
	double score;
	bool src_turnflag;
	int src_poly_id;
	int src_line_id;
	bool dst_is_piece;
	int dst_poly_id;
	int dst_line_id;
	bool is_reverse_line;

	bool operator==(const MoveNode &r) const {
		return (src_turnflag == r.src_turnflag && src_poly_id == r.src_poly_id && src_line_id == r.src_line_id && dst_is_piece == r.dst_is_piece && dst_poly_id == r.dst_poly_id && dst_line_id == r.dst_line_id && is_reverse_line == r.is_reverse_line);
	}

	//普通にsortすると「スコア降順, あとは昇順」になるようにする
	bool operator<(const MoveNode &r) const {
		if (score != r.score) return score > r.score;
		if (src_turnflag != r.src_turnflag) return src_turnflag < r.src_turnflag;
		if (src_poly_id != r.src_poly_id) return src_poly_id < r.src_poly_id;
		if (src_line_id != r.src_line_id) return src_line_id < r.src_line_id;
		if (dst_is_piece != r.dst_is_piece) return dst_is_piece < r.dst_is_piece;
		if (dst_poly_id != r.dst_poly_id) return dst_poly_id < r.dst_poly_id;
		if (dst_line_id != r.dst_line_id) return dst_line_id < r.dst_line_id;
		return is_reverse_line < r.is_reverse_line;
	}

	MoveNode(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line) {
		this->src_turnflag = src_turnflag;
		this->src_poly_id = src_poly_id;
		this->src_line_id = src_line_id;
		this->dst_is_piece = dst_is_piece;
		this->dst_poly_id = dst_poly_id;
		this->dst_line_id = dst_line_id;
		this->is_reverse_line = is_reverse_line;
	}
	
	MoveNode() {
	}
};
