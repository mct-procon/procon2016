#include "Solver.h"

class MySolver : public Solver {
private:
	queue<Vertex> standard_point;

	bool is_combi_vertex(Vertex piece_v, Vertex dst, double error_dist, double error_angle);	//頂点がくっついているか？ (くっついていればtrueを返す）
	int get_combi_level(Vertex src, double error_dist, double error_angle);						//結合度計算（☆）
	Vertex get_best_vertex(Vertex dst);															//頂点dstと最も結合する頂点srcを返す（☆）
	void update(int piece_id);																	//ピースpiece_idの位置が確定するときの更新

public:
	void solve();	//問題を解きます
};