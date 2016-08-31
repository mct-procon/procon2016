//Line�N���X�̎���
//�Q�l�����Fhttp://www.prefield.com/algorithm/

#include "Line.h"

//�x�N�g��u�ƃx�N�g��v�̊O�ς̕����t���T�C�Y = u.real * v.imag - u.imag * v.real��Ԃ�. ���Ȃ݂ɁAcross(u, v) = -cross(v, u)�����藧�B
//conj(u)��u�̋��𕡑f��u.real() - u.imag(), imag(x)��x.imag()�Ɠ����B
double Line::cross(Point &u, Point &v) {
	return imag(conj(u) * v);
}

//��_�v�Z
Point Line::get_cross_point(Line &line) {
	double A = cross(e - s, line.e - line.s);	//�����̒[�_�𒸓_�Ƃ��镽�s�l�ӌ`�̖ʐ�(>=0)
	double B = cross(e - s, e - line.s);		//s, e, line.s�𒸓_�Ƃ���O�p�`�̖ʐ�(>=0)
	return line.s + B / A * (line.e - line.s);			//��_��Ԃ�
}

//s -> e�������v���̑��p�`�̒��_��i, i + 1�ł���Ƃ��A���p�`�ɂ��āu�O�����ȒP�ʖ@���x�N�g���v��Ԃ��B
//���Ȃ킿�ARe������, Im�������Ɏ�����Ƃ��As -> e�����v����90����]�����x�N�g����傫��1�ɂ��ĕԂ��B
Point Line::get_unit_norm() {
	Point norm = complex<double>(0, 1) * (s - e);
	return norm / abs(norm);
}

//2�̃x�N�g���̓��ς�Ԃ�
double Line::dot(Point &u, Point &v) {
	return u.real() * v.real() + u.imag() * v.imag();
}

//��̏�����
Line::Line() {
}

//�܂Ƃ��ȏ�����
Line::Line(Point s, Point e) {
	this->s = s;
	this->e = e;
}

//�����蔻�� (T�����������Ă���Ƃ݂Ȃ�)
bool Line::ishit(Line &line) {
	double u, v;

	u = cross(e - s, line.s - s);
	v = cross(e - s, line.e - s);
	if (max(u, v) < 0 || min(u, v) > 0) { return false; }
	u = cross(line.e - line.s, s - line.s);
	v = cross(line.e - line.s, e - line.s);
	if (max(u, v) < 0 || min(u, v) > 0) { return false; }
	return true;
}

//line1, line2�Ƃ̌�_P, Q�����߁A�����͈̔͂�[P, Q]�ɂ���B
void Line::cut_line(Line &line1, Line &line2) {
	s = get_cross_point(line1);
	e = get_cross_point(line2);
}

//�������u�O�����ȒP�ʖ@���x�N�g���v�̕�����dist�����������Bdist�����̏ꍇ�͋t�����ɓ����B
void Line::move_line(double dist) {
	Point trans = get_unit_norm();
	trans *= dist;
	s += trans;
	e += trans;
}

//������half_line�Ƃ̓����蔻��
bool Line::ishit_half(Line &half_line) {
	double u, v;

	u = cross(half_line.e - half_line.s, s - half_line.s);
	v = cross(half_line.e - half_line.s, e - half_line.s);
	if (max(u, v) < 0 || min(u, v) > 0) { return false; }
	u = cross(e - s, half_line.s - s);
	v = cross(e - s, half_line.e - s);
	if ((max(u, v) < 0 || min(u, v) > 0) && abs(u) < abs(v)) { return false; }
	return true;
}

//�_�Ɛ����̋���
double Line::dist(Point point) {
	if (dot(point - s, e - s) <= 0) { return abs(point - s); }
	if (dot(point - e, s - e) <= 0) { return abs(point - e); }
	return abs(cross(e - s, point - s) / abs(e - s));
}
