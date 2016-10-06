//���p�`�̌����̎������ӊO�Əd�������̂ŁA�N���X�ɂ��܂����B
#pragma once
#include "Poly.h"

class MargePoly {
private:
	double dist_error;			//permition of dist error for marge point
	vector<Point> points;		//points[i]   = ���_i�i���O���[�vi�j�̍��W, 1000���_�ȉ����ƐM���Ă�()
	vector<Line> lines;			//lines[i] = all_l[i]���O���[�v���ɍ��킹�ďC����������
	bool table[1000][1000];		//table[i][j] = (points[i] -> points[j]�����ڂȂ����Ă���, �������Ƀ}�[�W��̗̈悪����A�E���Ƀ}�[�W��̗̈悪�Ȃ�)��true, �����O���t

	void point_marge(Poly &src_poly, Poly &dst_poly);											//dst_poly�̕ӂⒸ�_�ɍ��킹��src_poly��ό`���� -> ���̌�Asrc_poly�ɍ��킹��dst_poly��ό`����
	void make_group(Poly &src_poly, Poly &dst_poly);											//points, lines�̐ݒ� (unique���邾��)
	bool can_connect(Point s, Point e, Poly &src_poly, Poly &dst_poly, bool dst_is_piece);		//��s->e��\��邩����
	void make_graph(Poly &src_poly, Poly &dst_poly, bool dst_is_piece);							//table�̐��� (�L���ӂ�\�邽�߂�, Poly�̈������K�v�j
	vector<Point> get_cycle(int start_id);														//�u�����v���́v���p�`�̐��� (������, ���̒i�K�ł͏璷�ȕӂ��܂ށj

	bool has_no_holl(vector<int> ids, Poly &poly);

	Poly to_poly(vector<Point> cycle, Poly &src_poly, Poly &dst_poly, double angle_error_deg, bool dst_is_piece);		//�T�C�N������璷�ȓ_�������đ��p�`���쐬

public:
	bool can_marge(double dist_error, double angle_error_deg, Poly &src_poly, Poly &dst_poly, bool dst_is_piece, vector<Poly> &wakus);		//�����ł�����false, �i�}�[�W�����, �ǂ̘g�ɂ�����Ȃ��s�[�X���ł���ꍇ��false�j
	pair<bool, vector<Poly> > marge_poly(double dist_error, double angle_error_deg, Poly &src_poly, Poly &dst_poly, bool dst_is_piece);		//���p�`�̃}�[�W, �}�[�W���s����false��Ԃ�
};