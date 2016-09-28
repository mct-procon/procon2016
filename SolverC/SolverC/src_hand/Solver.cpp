#include "Solver.h"
#include "Extern.h"

//ピースsrc_poly_idを動かす (辺srcを辺dstにくっつけるイメージ)
//動かしてくっつける, くっつけるのが無理ならfalseを返す
bool Solver::move(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id) {
	Poly &src = pieces[src_poly_id];
	Poly &dst = (dst_is_piece ? pieces[dst_poly_id] : wakus[dst_poly_id]);

	if (src.is_turn != src_turnflag) { src.turn(); }
	src.move(src_line_id, dst.points[dst_line_id + 1], dst.points[dst_line_id]);

	//多角形のマージ
	if (src.is_hit(scaledown_poly(dst, dist)) || src.is_cover(dst)) return false;
	if (dst_is_piece && dst.is_cover(src)) return false;
	if (!marge_poly.can_marge(dist, angle_error_deg, src, dst, dst_is_piece, wakus)) return false;
	
	Poly marged_poly = marge_poly.marge_poly(dist, angle_error_deg, src, dst, dst_is_piece);
	if (dst_is_piece) {
		pieces.push_back(marged_poly);
		pieces[src_poly_id].points.clear();
		pieces[dst_poly_id].points.clear();
	}
	else {
		wakus.push_back(marged_poly);
		pieces[src_poly_id].points.clear();
		wakus[dst_poly_id].points.clear(); wakus[dst_poly_id].lines.clear();
	}
}
