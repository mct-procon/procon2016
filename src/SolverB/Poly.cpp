//���p�`�N���X�̎���

#include "Poly.h"

//������
Poly::Poly() {
}

Poly::Poly(Point point0, vector<Point> point, bool is_piece_) {
	this->point0 = point0;
	this->point = point;
	turn_cnt = 0;
	is_putted = false;
	this->is_piece_ = is_piece_;
}

//���_��
int Poly::size() {
	return point.size();
}

//���_id�̐�Έʒu
Point Poly::get_point(int id) {
	return point0 + point[(id + size()) % size()];
}

//���_id���_s�Ƃ������A���_id -> id + 1���Ȃ��L�������Ɠ_s -> e���Ȃ��L�����������������ɂȂ�悤�ɁA���p�`���ړ�����B
void Poly::move(int id, Point s, Point e) {
	Point a = get_point(id);
	Point b = get_point(id + 1);

	Point mul = (e - s) / (b - a);
	mul /= abs(mul);
	for (int i = 0; i < size(); i++) {
		point[i] *= mul;
	}

	point0 += s - get_point(id);
}

//��]�x�N�g��, ��]�̒��S, ���s�ړ��ʂ��w�肵�Ĉړ� (��] -> ���s�ړ��̏��Ŏ��s)
void Poly::move2(Point mul, Point center, Point trans) {
	int i;
	for (i = 0; i < size(); i++) point[i] = get_point(i) - center;
	for (i = 0; i < size(); i++) point[i] *= mul;
	for (i = 0; i < size(); i++) point[i] += center - point0;
	point0 += trans;
}

//Poly�� (���_0��y���W����Ƃ���) ���]���A���_��̌������C������B
void Poly::turn() {
	int i;
	vector<Point> tmp(size());
	for (i = 0; i < point.size(); i++) tmp[i] = conj(point[i]);
	for (i = 0; i < point.size(); i++) point[i] = tmp[point.size() - 1 - i];
	turn_cnt++;
}

//�����w�肵�Ĕ��]
void Poly::turn2(double center_imag) {
	int i;
	vector<Point> tmp(size());

	for (i = 0; i < point.size(); i++) tmp[i] = get_point(i) - Point(0, center_imag);
	for (i = 0; i < point.size(); i++) tmp[i] = conj(tmp[i]);
	for (i = 0; i < point.size(); i++) tmp[i] += Point(0, center_imag) - point0;
	for (i = 0; i < point.size(); i++) point[i] = tmp[point.size() - 1 - i];
	turn_cnt++;
}

//�ڐG����
bool Poly::is_hit(Poly &poly) {
	int i, j;

	for (i = 0; i < size(); i++) {
		Line a = Line(get_point(i), get_point(i + 1));
		for (j = 0; j < poly.size(); j++) {
			Line b = Line(poly.get_point(j), poly.get_point(j + 1));
			if (a.ishit(b)) { return true; }
		}
	}
	return false;
}

//poly���܂ނ��H(�O������F�ڐG���Ă��Ȃ�)
bool Poly::is_contain(Poly &poly) {
	Point p = poly.get_point(0);
	Line hl = Line(p, Point(10000, 10000));
	int cnt = 0;

	for (int i = 0; i < size(); i++) {
		Line line = Line(get_point(i), get_point(i + 1));
		if (line.ishit_half(hl)) {
			cnt++;
		}
	}
	if (cnt % 2 == 1) return true;
	return false;
}

//poly�Ɋ܂܂�邩�H(�O������F�ڐG���Ă��Ȃ��j
bool Poly::is_contained(Poly &poly) {
	Point p = get_point(0);
	Line hl = Line(p, Point(10000, 10000));
	int cnt = 0;

	for (int i = 0; i < poly.size(); i++) {
		Line line = Line(poly.get_point(i), poly.get_point(i + 1));
		if (line.ishit_half(hl)) {
			cnt++;
		}
	}
	if (cnt % 2 == 1) return true;
	return false;
}

//���p�`�̕����t���ʐς�Ԃ�.
//Im������Re�������Ɏ�������A���_�񂪎��v���Ȃ畉�̒l��Ԃ��A�����v���Ȃ琳�̒l��Ԃ��B
double Poly::area() {
	double ret = 0;

	for (int i = 0; i < size(); i++) {
		double x1 = get_point(i).real();
		double y1 = get_point(i).imag();
		double x2 = get_point(i + 1).real();
		double y2 = get_point(i + 1).imag();
		ret += x1 * y2 - x2 * y1;
	}
	ret *= 0.5;
	return ret;
}

//���]��ԂȂ�true��Ԃ�. �g�p��F�g�����̈ʒu�ɖ߂�����
bool Poly::is_turned() {
	return (turn_cnt % 2 == 1);
}

//���_��̌����������I�ɕύX����. ���_0�̈ʒu�͕ύX���Ȃ�.
void Poly::point_reverse() {
	vector<Point> tmp;
	tmp = point;
	for (int i = 0; i < size(); i++) point[i] = tmp[(size() - i) % size()];
}

//���p�`�̃^�C�v��Ԃ�
bool Poly::is_piece() {
	return is_piece_;
}
