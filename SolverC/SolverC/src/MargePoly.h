//���p�`�̌����̎������ӊO�Əd�������̂ŁA�N���X�ɂ��܂����B
#include "Poly.h"

class MargePoly {
private:
	double dist_error;					//�����̋��e�덷
	vector<Point> all_p;				//all_p�csrc_poly, dst_poly�̑S���_����ꂽ����
	vector<Line>  all_l;				//all_l�csrc_poly, dst_poly�̑S�ӂ���ꂽ����
	int group_id[10000];				//group_id[i] = all_p[i]�̃O���[�v�ԍ� 
	
	vector<Point> points;		//points[i]   = ���_i�i���O���[�vi�j�̍��W, 1000���_�ȉ����ƐM���Ă�()
	bool table[1000][1000];		//table[i][j] = points[i] -> points[j]�����ڂȂ����Ă���, �����O���t

	void make_all_pl(Poly &src_poly, Poly &dst_poly);		//all_p, all_l�����
	void make_group();										//group_id[i], points�̐ݒ�
	void make_graph();															//table�̐���
	Poly scaleup_poly(Poly &poly);
	int get_startPoint_id(Poly &src_poly, Poly &dst_poly, bool dst_is_piece);	//�}�[�W��������Ȃ��_(��\�_)��id����Ԃ�
	vector<Point> get_cycle(int start_id, bool is_clockwise, double area);		//���p�`�̐��� (������, ���̒i�K�ł͏璷�ȕӂ��܂ށj

public:
	bool can_marge(double dist_error, double angle_error_deg, Poly &src_poly, Poly &dst_poly, bool dst_is_piece);		//�����ł�����false
	Poly marge_poly(double dist_error, double angle_error_deg, Poly &src_poly, Poly &dst_poly, bool dst_is_piece);		//���p�`�̃}�[�W
};