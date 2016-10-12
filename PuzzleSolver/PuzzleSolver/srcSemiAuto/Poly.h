//���p�`�N���X�̐錾

#pragma once
#include "Point.h"
#include "Line.h"

class Poly {
private:
	void update_rect();			//�O�ڒ����`�̏����X�V

public:
	vector<Point> points;		//���_0�ɑ΂��钸�_0, 1, 2, �c, n - 1, 0�̍��W (�g�̏ꍇ�͎��v����, �s�[�X�̏ꍇ�͔����v����j
	vector<Line> lines;			//�\������� (���s��)
	bool is_turn;				//�ŏ����猩��, �Ђ�����Ԃ�����ԂȂ�true��Ԃ�.
	double lx, ly, rx, ry;		//Poly�̊O�ڒ����`. �����蔻�荂�����p.

	//������
	Poly();
	Poly(vector<Point> points, vector<Line> lines);

	//�X�V
	void point_reverse();								//���_��̌����������I�ɕύX����
	void move(int id, Point s, Point e);				//�ړ��i������s->e��point[id]->point[id+1]����������)
	void move_reverse(int id, Point s, Point e);		//�ړ� (������e->s��point[id+1]->point[id]����������)
	void turn();										//���] (��΍��W�ɂ�����, y = point[0].imag()�𒆐S)
	
	//�擾
	int size();						//���_�� (points.size() - 1)
	double area();					//���p�`Poly�̕����t���ʐ� (���c�����v�܂��, ���c���v���j
	double angle(int point_id);

	bool is_hit(Poly &poly);		//poly�Ɠ������Ă��邩�H
	bool is_cover_point(Point &p);	//�_p���܂�ł��邩�H
	bool is_cover(Poly &poly);		//poly���܂�ł��邩�H�iis_hit(poly) == false�̂Ƃ��͐��������ʂ�Ԃ��j
};
