//�����N���X
 
#pragma once
#include "Point.h"
#include "STL.h"

class Line {
private:
	double cross(Point &u, Point &v);			//�x�N�g��u, v�̊O�ς̑傫����Ԃ�
	Point get_cross_point(Line &line);			//��_�v�Z
	Point get_unit_norm();						//s -> e�������v���̑��p�`�̒��_��i, i + 1�ł���Ƃ��A���p�`�ɂ��āu�O�����ȒP�ʖ@���x�N�g���v��Ԃ��B

	double dot(Point &u, Point &v);

public:
	Point s, e;									//�n�_, �I�_�B�ȉ��̊֐��Q�́A�n�_�ƏI�_�̍��W���������Ă��A���������삷��B

	Line ();
	Line (Point s, Point e);
	bool ishit(Line &line);						//���̐����Ƃ̌�������. ���E��͓������Ă���(true)�Ƃ݂Ȃ��B
	void cut_line(Line &line1, Line &line2);	//line1, line2�Ƃ̌�_P, Q�����߁A�����͈̔͂�[P, Q]�ɂ���B
	void move_line(double dist);				//�������u�O�����̒P�ʖ@���x�N�g���v�̕�����dist�����������Bdist�����̏ꍇ�͋t�����ɓ����B
	bool ishit_half(Line &half_line);			//������half_line�Ƃ̌�������, ���E��͓������Ă���(true)�Ƃ݂Ȃ��B

	double dist(Point point);
	
	bool operator==(const Line &r) const { return s == r.s && e == r.e; }
};