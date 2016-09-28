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
	Poly scalechange_poly(Poly &poly, double dist);
	int get_startPoint_id(Poly &src_poly, Poly &dst_poly, bool dst_is_piece);	//�}�[�W��������Ȃ��_(��\�_)��id����Ԃ�
	vector<Point> get_cycle(int start_id, bool is_clockwise, double area);		//���p�`�̐��� (������, ���̒i�K�ł͏璷�ȕӂ��܂ށj

	//can_marge�̕⏕�֐�, http://www.prefield.com/algorithm/ �̕��ʊ􉽂��Q�l�ɂ���
	double cross(Point &a, Point &b);				//�O��
	double dot(Point &a, Point &b);					//����
	int ccw(Point a, Point b, Point c);				//�_�̐i�s����
	Poly convex_hull(vector<Point> points);			//�ʖ@
	double convex_diameter(vector<Point> points);	//�ʑ��p�`�̒��a

	bool has_no_holl(vector<int> ids, Poly &poly);

public:
	bool can_marge(double dist_error, double angle_error_deg, Poly &src_poly, Poly &dst_poly, bool dst_is_piece, vector<Poly> &wakus);		//�����ł�����false, �i�}�[�W�����, �ǂ̘g�ɂ�����Ȃ��s�[�X���ł���ꍇ��false�j
	Poly marge_poly(double dist_error, double angle_error_deg, Poly &src_poly, Poly &dst_poly, bool dst_is_piece);							//���p�`�̃}�[�W
};