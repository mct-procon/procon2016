//多角形の結合の実装が意外と重かったので、クラスにしました。
#include "Poly.h"

class MargePoly {
private:
	double dist_error;					//距離の許容誤差
	vector<Point> all_p;				//all_p…src_poly, dst_polyの全頂点を入れたもの
	vector<Line>  all_l;				//all_l…src_poly, dst_polyの全辺を入れたもの
	int group_id[10000];				//group_id[i] = all_p[i]のグループ番号 
	
	vector<Point> points;		//points[i]   = 頂点i（＝グループi）の座標, 1000頂点以下だと信じてる()
	bool table[1000][1000];		//table[i][j] = points[i] -> points[j]が直接つながっている, 無向グラフ

	void make_all_pl(Poly &src_poly, Poly &dst_poly);		//all_p, all_lを作る
	void make_group();										//group_id[i], pointsの設定
	void make_graph();															//tableの生成
	Poly scalechange_poly(Poly &poly, double dist);
	int get_startPoint_id(Poly &src_poly, Poly &dst_poly, bool dst_is_piece);	//マージ後も消えない点(代表点)のidを一つ返す
	vector<Point> get_cycle(int start_id, bool is_clockwise, double area);		//多角形の生成 (ただし, この段階では冗長な辺を含む）

	//can_margeの補助関数, http://www.prefield.com/algorithm/ の平面幾何を参考にした
	double cross(Point &a, Point &b);				//外積
	double dot(Point &a, Point &b);					//内積
	int ccw(Point a, Point b, Point c);				//点の進行方向
	Poly convex_hull(vector<Point> points);			//凸法
	double convex_diameter(vector<Point> points);	//凸多角形の直径

	bool has_no_holl(vector<int> ids, Poly &poly);

public:
	bool can_marge(double dist_error, double angle_error_deg, Poly &src_poly, Poly &dst_poly, bool dst_is_piece, vector<Poly> &wakus);		//穴ができたらfalse, （マージすると, どの枠にも入らないピースができる場合もfalse）
	Poly marge_poly(double dist_error, double angle_error_deg, Poly &src_poly, Poly &dst_poly, bool dst_is_piece);							//多角形のマージ
};