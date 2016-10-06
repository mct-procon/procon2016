//多角形の結合の実装が意外と重かったので、クラスにしました。
#pragma once
#include "Poly.h"

class MargePoly {
private:
	double dist_error;			//permition of dist error for marge point
	vector<Point> points;		//points[i]   = 頂点i（＝グループi）の座標, 1000頂点以下だと信じてる()
	vector<Line> lines;			//lines[i] = all_l[i]をグループ化に合わせて修正したもの
	bool table[1000][1000];		//table[i][j] = (points[i] -> points[j]が直接つながっていて, かつ左側にマージ後の領域があり、右側にマージ後の領域がない)⇔true, 無向グラフ

	void point_marge(Poly &src_poly, Poly &dst_poly);											//dst_polyの辺や頂点に合わせてsrc_polyを変形する -> その後、src_polyに合わせてdst_polyを変形する
	void make_group(Poly &src_poly, Poly &dst_poly);											//points, linesの設定 (uniqueするだけ)
	bool can_connect(Point s, Point e, Poly &src_poly, Poly &dst_poly, bool dst_is_piece);		//辺s->eを貼れるか判定
	void make_graph(Poly &src_poly, Poly &dst_poly, bool dst_is_piece);							//tableの生成 (有向辺を貼るために, Polyの引数が必要）
	vector<Point> get_cycle(int start_id);														//「反時計回りの」多角形の生成 (ただし, この段階では冗長な辺を含む）

	bool has_no_holl(vector<int> ids, Poly &poly);

	Poly to_poly(vector<Point> cycle, Poly &src_poly, Poly &dst_poly, double angle_error_deg, bool dst_is_piece);		//サイクルから冗長な点を除いて多角形を作成

public:
	bool can_marge(double dist_error, double angle_error_deg, Poly &src_poly, Poly &dst_poly, bool dst_is_piece, vector<Poly> &wakus);		//穴ができたらfalse, （マージすると, どの枠にも入らないピースができる場合もfalse）
	pair<bool, vector<Poly> > marge_poly(double dist_error, double angle_error_deg, Poly &src_poly, Poly &dst_poly, bool dst_is_piece);		//多角形のマージ, マージ失敗時はfalseを返す
};