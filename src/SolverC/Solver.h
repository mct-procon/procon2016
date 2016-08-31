#pragma once
#include "SolverBase.h"

//������
class HalfLine {
private:
	Point s, e;				//s���n�_, s -> e������

	Point angle_vector();											//�������̕���(s -> e)��傫��1�̃x�N�g���ŕԂ��B
	double eval_function(double angle_vector_imag, double range);	//�Q�̔������̊p�x������A�����x���v�Z����֐��@�i�P�ʃX�e�b�v�֐� �Ƃ� ���K���z ���ǂ����j

public:
	HalfLine();
	HalfLine(Point s, Point e);									//������
	double evaluation(HalfLine &hl, double range, double dist);	//������hl�Ƃ̌����x(0�`1), range(>0)�͊p�x�̋��e�덷, dist(>0)�͈ʒu�̋��e�덷. range��0.02, dist��5���炢�œ������Ɨǂ��B

	Point start_point();
	Point end_point();
	bool operator==(const HalfLine &r) const { return s == r.s && e == r.e; }
};

//�\���o�[
//gid = group id(�O���[�v�ԍ�)�̗�, �����O���[�v�ԍ��̃s�[�X��1�̉�Ƃ��ē�����
class Solver : public SolverBase {
private:
	vector<Line> selected_lines;
	vector<int>  selected_lines_gid;	//selected_lines_gid[i] = �I���i�̃O���[�v�ԍ�
	vector<int> pieces_gid;				//pieces_gid[i] = �s�[�Xi�̃O���[�v�ԍ�
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
	void solve();		//�I�����ꂽ��selected_edges�ƍł��}�b�`����u�����Œu��
};