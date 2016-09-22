//Line�N���X�̎���
//�Q�l�����Fhttp://www.prefield.com/algorithm/
#include "Line.h"

//�x�N�g��u�ƃx�N�g��v�̊O�ς̕����t���T�C�Y = u.real * v.imag - u.imag * v.real��Ԃ�. ���Ȃ݂ɁAcross(u, v) = -cross(v, u)�����藧�B
//conj(u)��u�̋��𕡑f��u.real() - u.imag(), imag(x)��x.imag()�Ɠ����B
double Line::cross(Point &u, Point &v) {
	return imag(conj(u) * v);
}

//��_�v�Z
Point Line::cross_point(Line &line) {
	double A = cross(e - s, line.e - line.s);	//�����̒[�_�𒸓_�Ƃ��镽�s�l�ӌ`�̖ʐ�(>=0)
	double B = cross(e - s, e - line.s);		//s, e, line.s�𒸓_�Ƃ���O�p�`�̖ʐ�(>=0)
	return line.s + B / A * (line.e - line.s);	//��_��Ԃ�
}

//s -> e�������v���̑��p�`�̒��_��i, i + 1�ł���Ƃ��A���p�`�ɂ��āu�O�����ȒP�ʖ@���x�N�g���v��Ԃ��B
//���Ȃ킿�ARe������, Im�������Ɏ�����Ƃ��As -> e�����v����90����]�����x�N�g����傫��1�ɂ��ĕԂ��B
Point Line::unit_norm() {
	Point norm = complex<double>(0, 1) * (s - e);
	return norm / abs(norm);
}

//2�̃x�N�g��u, v�̓��ς�Ԃ�
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




/*�X�V*/
//line1, line2�Ƃ̌�_P, Q�����߁A�����͈̔͂�[P, Q]�ɂ���B
void Line::cut(Line &line1, Line &line2) {
	s = cross_point(line1);
	e = cross_point(line2);
}

//�������u�O�����ȒP�ʖ@���x�N�g���v�̕�����dist�����������Bdist�����̏ꍇ�͋t�����ɓ����B
void Line::move(double dist) {
	Point trans = unit_norm();
	trans *= dist;
	s += trans;
	e += trans;
}

//���������_O�𒆐S�Ƃ��āAangle������]�iangle���|���邾���j
void Line::rotate(Point angle) {
	s *= angle;
	e *= angle;
}

//trans�������s�ړ�
void Line::transrate(Point trans) {
	s += trans;
	e += trans;
}

//y = 0�����Ƃ��Ĕ��]
void Line::turn() {
	s = conj(s);
	e = conj(e);
}




/*�v�Z*/
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

//�_point�������this�ւ̋���
double Line::dist(Point point) {
	if (dot(point - s, e - s) <= 0) { return abs(point - s); }
	if (dot(point - e, s - e) <= 0) { return abs(point - e); }
	return abs(cross(e - s, point - s) / abs(e - s));
}

//����line�Ƃ̈�v�x
//0�c��_�����L���Ă��Ȃ�
//4�c��������v
//2�c�������t
//1�c����������
//�����̕���������ۂɂ��p���Ă���̂ŁA�����̖߂�l��ς���ƁAmarge_poly()�Ɗp�x�̕]�����o�O��܂��B
//�����̕�����Ԃ��֐��ƃX�R�A�֐��𕪂��A�K�؂Ȋ֐�����t����́A�N������Ăق����B
double Line::machi_score(Line line, double dist_error, double angle_error_deg) {
	if (abs(s - line.e) <= dist_error || abs(e - line.s) <= dist_error) swap(line.s, line.e);
	if (abs(s - line.s) <= dist_error && abs(e - line.e) <= dist_error) return 4;
	if (abs(s - line.s) > dist_error && abs(e - line.e) > dist_error) return 0;

	Point angle = (e - s) / (line.e - line.s);
	angle /= abs(angle);
	if (abs(angle.imag()) > sin(angle_error_deg * 3.1415926 / 180.0)) return 0;
	if (angle.real() < 0) return 2;
	return 1;
}