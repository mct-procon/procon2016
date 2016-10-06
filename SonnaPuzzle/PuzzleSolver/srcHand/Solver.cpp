#include "Solver.h"
#include "Extern.h"

//ピースsrc_poly_idを動かす (辺srcを辺dstにくっつけるイメージ)
//動かしてくっつける, くっつけるのが無理ならfalseを返す
//reverse…dst辺をひっくり返すかどうか？
bool Solver::move(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line) {
	Poly &src = pieces[src_poly_id];
	Poly &dst = (dst_is_piece ? pieces[dst_poly_id] : wakus[dst_poly_id]);

	if (src.is_turn != src_turnflag) { src.turn(); }

	if (is_reverse_line) {
		src.move_reverse(src_line_id, dst.points[dst_line_id], dst.points[dst_line_id + 1]);	//終点を合わせる
	}
	else {
		src.move(src_line_id, dst.points[dst_line_id + 1], dst.points[dst_line_id]);
	}

	//多角形のマージ
	if (!is_do_marge) return false;

	Poly scaledown_dst = scaledown_poly(dst, dist);
	if (src.is_hit(scaledown_dst) || src.is_cover(scaledown_dst)) return false;
	if (dst_is_piece && scaledown_dst.is_cover(src)) return false;
	if (!marge_poly.can_marge(dist, angle_error_deg, src, dst, dst_is_piece, wakus)) return false;

	pair<bool, vector<Poly>> res = marge_poly.marge_poly(dist, angle_error_deg, src, dst, dst_is_piece);
	if (res.first == false) return false;
	
	vector<Poly> marged_polys = res.second;
	if (dst_is_piece) {
		for (int i = 0; i < marged_polys.size(); i++) pieces.push_back(marged_polys[i]);
		pieces[src_poly_id].points.clear();
		pieces[dst_poly_id].points.clear();
	}
	else {
		for (int i = 0; i < marged_polys.size(); i++) wakus.push_back(marged_polys[i]);
		pieces[src_poly_id].points.clear();
		wakus[dst_poly_id].points.clear(); wakus[dst_poly_id].lines.clear();
	}
	return true;
}
