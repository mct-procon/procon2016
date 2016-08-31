#pragma once
#include "SolverBase.h"

//半直線
class HalfLine {
private:
	Point s, e;				//sが始点, s -> eが方向

	Point angle_vector();											//半直線の方向(s -> e)を大きさ1のベクトルで返す。
	double eval_function(double angle_vector_imag, double range);	//２つの半直線の角度差から、結合度を計算する関数　（単位ステップ関数 とか 正規分布 が良さげ）

public:
	HalfLine();
	HalfLine(Point s, Point e);									//初期化
	double evaluation(HalfLine &hl, double range, double dist);	//半直線hlとの結合度(0～1), range(>0)は角度の許容誤差, dist(>0)は位置の許容誤差. range≒0.02, dist≒5くらいで動かすと良い。

	Point start_point();
	Point end_point();
	bool operator==(const HalfLine &r) const { return s == r.s && e == r.e; }
};

//ソルバー
//gid = group id(グループ番号)の略, 同じグループ番号のピースは1つの塊として動かす
class Solver : public SolverBase {
private:
	vector<Line> selected_lines;
	vector<int>  selected_lines_gid;	//selected_lines_gid[i] = 選択辺iのグループ番号
	vector<int> pieces_gid;				//pieces_gid[i] = ピースiのグループ番号
	vector<int> wakus_gid;

	tuple<bool, Point, Point> move(int src_piece_id, int src_line_id, bool turnflag, int dst_line_id);
	void move_reverse(int gid, bool is_turn, Point mul, Point trans);
	double evaluation(int gid);
	double evaluation(int src_piece_id, int src_line_id, bool turnflag, int dst_line_id);
	bool can_move(int gid);
	bool can_marge(int gid_src, int gid_dst);

	void think(int src_piece_id, int src_line_id, bool turnflag, int dst_line_id, double &best_score, tuple<int, int, bool, int> &best);

public:
	void prepare();
	void select_line(complex<double> mousePoint);
	void release_line();
	void draw();
	void solve();		//選択された辺selected_edgesと最もマッチする置き方で置く
};