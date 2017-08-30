//���p�`�N���X�̎���

#include "Poly.h"

//�O�ڒ����`�̏����X�V
void Poly::update_rect() {
	lx = rx = points[0].real();
	ly = ry = points[0].imag();
	for (int i = 1; i < size(); i++) {
		Point p = points[i];
		lx = min(lx, p.real());
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


//�ړ� (������point[id+1]->point[id]�𔼒���s->e�ɂ�������
void Poly::move_reverse(int id, Point s, Point e) {
	Point mul = (e - s) / (points[id] - points[id + 1]);
	
	//���_��̉�]
	mul /= abs(mul);
	for (int i = 0; i < points.size(); i++) { points[i] *= mul; }

	//���_��̕��s�ړ�
	Point trans = s - points[id + 1];
	for (int i = 0; i < points.size(); i++) { points[i] += trans; }

	//�\���ӂ̈ړ�
	for (int i = 0; i < lines.size(); i++) { lines[i].rotate(mul); lines[i].transrate(trans); }

	//�O�ڎl�p�`�̍Čv�Z
	update_rect();
}


//���]�iy=points[0].imag()����Ƃ��Ĕ��]���A���_��̌������C������B�j
void Poly::turn() {
	int i;
	vector<Point> tmp(points.size());

	//���_��̔��]
	for (i = 0; i < points.size(); i++) tmp[i] = Point(points[i].real(), 2 * points[0].imag() - points[i].imag());
	for (i = 0; i < points.size(); i++) points[i] = tmp[points.size() - 1 - i];

	//�\���ӂ̔��]
	for (i = 0; i < lines.size(); i++) lines[i].turn(points[0].imag());

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


//�p�x�����ŕԂ��B�g�Ȃ�O�p�ŁA�s�[�X�Ȃ���t
double Poly::angle(int point_id)
{
	int a = point_id - 1; if (a < 0) a += size();
	int b = point_id;
	int c = point_id + 1;

	Point ang = (points[a] - points[b]) / (points[c] - points[b]);
	double ret = atan2(ang.imag(), ang.real()) * 180 / 3.1415926535;

	while (ret < 0) { ret += 360; }
	while (ret >= 360) { ret -= 360; }
	return ret;
}


//�����蔻��p�̗֊s��Ԃ��B
//dist > 0�̂Ƃ��c�s�[�X�̏ꍇ�͓����蔻��g�������ɂł���B�g�̏ꍇ�͓����蔻��g���O���ɂł���B
Poly Poly::scaledown_poly(double dist) {
	int i;
	int n = size();
	vector<Line> lines(n);
	vector<Point> points(n + 1);

	//���p�`�����ɐ���������
	for (i = 0; i < n; i++) {
		lines[i] = Line(this->points[i], this->points[i + 1]);
		lines[i].move(-dist);
	}
	for (i = 0; i < n; i++) {
		int id1 = i - 1; if (id1 < 0) id1 += n;
		int id2 = i + 1; if (id2 >= n) id2 -= n;
		lines[i].cut(lines[id1], lines[id2]);	//lines[i].s <- lines[id1]�Ƃ̌�_, lines[i].e <- lines[id2]�Ƃ̌�_
	}

	//���p�`���\��
	for (i = 0; i < n; i++) {
		points[i] = lines[i].s;
	}
	points[n] = points[0];
	return Poly(points, lines);		//��Q�����͉��ł��ǂ�
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

//���_p���܂�ł����true��Ԃ��B
bool Poly::is_cover_point(Point &p) {
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

//poly���܂�ł���Ȃ�true��Ԃ��B�i�g�p���̏����F�ڐG���Ă��Ȃ�)
bool Poly::is_cover(Poly &poly) {
	if (poly.lx < lx || poly.rx > rx || poly.ly < ly || poly.ry > ry) { return false; }
	return is_cover_point(poly.points[0]);
}