//�����N���X
 
#pragma once
#include "Point.h"
#include "stdafx.h"

class Line {
private:
	//���C�u����
	double cross(Point &u, Point &v);			//�x�N�g��u, v�̊O�ς̑傫����Ԃ�
	Point unit_norm();							//s -> e�������v���̑��p�`�̒��_��i, i + 1�ł���Ƃ��A���p�`�ɂ��āu�O�����ȒP�ʖ@���x�N�g���v��Ԃ��B
	double dot(Point &u, Point &v);				//�x�N�g��u, v�̓��ς�Ԃ�
	int tag;									//[�^�O�����݂��邩(1�r�b�g), �s�[�X�ɓ\��ꂽ�V�[���̒l(7�r�b�g), ���C���̔ԍ�(7�r�b�g)]

public:
	Point s, e;									//�n�_, �I�_�B�ȉ��̊֐��Q�́A�n�_�ƏI�_�̍��W���������Ă��A���������삷��B

	Line ();
	Line (Point s, Point e);

	//�X�V
	void cut(Line &line1, Line &line2);			//line1, line2�Ƃ̌�_P, Q�����߁A�����͈̔͂�[P, Q]�ɂ���B
	void move(double dist);						//������unit_norm()�̕�����dist�����������Bdist�����̏ꍇ�͋t�����ɓ���.
	void rotate(Point angle);					//���������_O�𒆐S�Ƃ��āAangle������]�iangle���|���邾���j
	void transrate(Point trans);				//trans�������s�ړ�
	void turn(double center_y);					//y = center_y�𒆐S�ɔ��]

	//�v�Z
	double length();
	Point cross_point(Line &line);				//��_�v�Z
	bool ishit(Line &line);														//���̐����Ƃ̌�������.
	bool ishit_half(Line &half_line);											//������half_line�Ƃ̌�������.
	double dist(Point point);													//�_point�������this�܂ł̋���.
	double straight_dist(Point point);											//�_point���璼��this�܂ł̋���
	double match_score(Line line, double dist_error, double angle_error_deg);	//����line�Ƃ̈�v�x

	//���Z�q
	bool operator==(const Line &r) { return s == r.s && e == r.e; }

	//���o�͗p
	void set_tag(int poly_id, int line_id);
	bool is_exist_tag();
	int poly_id();
	int line_id();
};