//���p�`�N���X�̎���

#include "Poly.h"

//�O�ڒ����`�̏����X�V
void Poly::update_rect() {
	lx = rx = points[0].real();
	ly = ry = points[0].imag();
	for (int i = 1; i < size(); i++) {
		Point p = points[i];
		lx = min(lx, p.imag());
		ly = min(ly, p.imag());
		rx = max(rx, p.real());
		ry = max(ry, p.imag());
	}
}

//������
Poly::Poly() {
}

Poly::Poly(vector<Point> points, vector<Line> lines) {
	this->points = points;
	this->lines = lines;
	this->is_turn = false;
	update_rect();
}




/*�X�V*/
//���_��̌����������I�ɕύX����.
void Poly::point_reverse() {
	vector<Point> tmp;
	tmp = points;
	for (int i = 0; i < points.size(); i++) points[i] = tmp[points.size() - 1 - i];
}


//�ړ��i������points[id]->points[id+1]�𔼒���s->e�ɂ�������)
void Poly::move(int id, Point s, Point e) {
	Point mul = (e - s) / (points[id + 1] - points[id]);

	//���_��̉�]
	mul /= abs(mul);
	for (int i = 0; i < points.size(); i++) { points[i] *= mul; }

	//���_��̕��s�ړ�
	Point trans = s - points[id];
	for (int i = 0; i < points.size(); i++) { points[i] += trans; }

	//�\���ӂ̈ړ�
	for (int i = 0; i < lines.size(); i++) { lines[i].rotate(mul); lines[i].transrate(trans); }

	//�O�ڎl�p�`�̍Čv�Z
	update_rect();
}


//���]�iy=0����Ƃ��Ĕ��]���A���_��̌������C������B�j
void Poly::turn() {
	int i;
	vector<Point> tmp(points.size());
	
	//���_��̔��]
	for (i = 0; i < points.size(); i++) tmp[i] = conj(points[i]);
	for (i = 0; i < points.size(); i++) points[i] = tmp[points.size() - 1 - i];

	//�\���ӂ̔��]
	for (i = 0; i < lines.size(); i++) lines[i].turn();

	//���]�t���O�̍X�V
	is_turn = !is_turn;

	//�O�ڎl�p�`�̍Čv�Z
	update_rect();
}





/*�擾*/
//���_��
int Poly::size() {
	return (int)points.size() - 1;
}

//���p�`�̕����t���ʐς�Ԃ�.
//Im������Re�������Ɏ�������A���_�񂪎��v���Ȃ畉�̒l��Ԃ��A�����v���Ȃ琳�̒l��Ԃ��B
double Poly::area() {
	double ret = 0;

	for (int i = 0; i < size(); i++) {
		double x1 = points[i].real();
		double y1 = points[i].imag();
		double x2 = points[i + 1].real();
		double y2 = points[i + 1].imag();
		ret += x1 * y2 - x2 * y1;
	}
	ret *= 0.5;
	return ret;
}


//�ӂ��ڐG���Ă���Ȃ�true, �ڐG���Ă��Ȃ��Ȃ�false��Ԃ�.
bool Poly::is_hit(Poly &poly) {
	int i, j;

	if (lx > poly.rx || rx < poly.lx || ly > poly.ry || ry < poly.ly) { return false; }

	for (i = 0; i < size(); i++) {
		Line a = Line(points[i], points[i + 1]);
		for (j = 0; j < poly.size(); j++) {
			Line b = Line(poly.points[j], poly.points[j + 1]);
			if (a.ishit(b)) { return true; }
		}
	}
	return false;
}

//poly���܂�ł���Ȃ�true��Ԃ��B�i�g�p���̏����F�ڐG���Ă��Ȃ�)
bool Poly::is_cover(Poly &poly) {
	if (poly.lx < lx || poly.rx > rx || poly.ly < ly || poly.ry > ry) { return false; }

	Point p = poly.points[0];
	Line hl = Line(p, Point(10000, 0));
	int cnt = 0;

	for (int i = 0; i < size(); i++) {
		Line line = Line(points[i], points[i + 1]);
		if (line.ishit_half(hl)) {
			cnt++;
		}
	}
	if (cnt % 2 == 1) return true;
	return false;
}
