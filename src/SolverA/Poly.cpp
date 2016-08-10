//���p�`�N���X�̎���

#include "Poly.h"

//������
Poly::Poly() {
}

Poly::Poly(Point point0, vector<Point> point) {
	this->point0 = point0;
	this->point = point;
}

//���_����Ԃ�
int Poly::size() {
	return point.size();
}

//���_id�̐�΍��W��Ԃ�
Point Poly::get_point(int id) {
	return point0 + point[(id + size()) % size()];
}

//���_id�̊p�x[��]��Ԃ�. �l���[0, 360)
double Poly::get_angle_deg(int id) {
	Point a = get_point(id - 1) - get_point(id);
	Point b = get_point(id + 1) - get_point(id);

	double rad = atan2((b / a).imag(), (b / a).real());
	double deg = rad * 180.0 / 3.14159265358979;
	while (deg < 0) { deg += 360; }
	while (deg >= 360) { deg -= 360; }
	return deg;
}

//���_id���_s�Ƃ������A���_id -> id + 1���Ȃ��L�������Ɠ_s -> e���Ȃ��L�����������������ɂȂ�悤�ɁA���p�`���ړ�����B
void Poly::move(int id, Point s, Point e) {
	Point a = get_point(id);
	Point b = get_point(id + 1);

	//���_0�𒆐S�ɉ�]. �x�N�g��a -> b���x�N�g��s -> e�Ɠ��������ɂȂ�悤�Ɂu�S���_���v��]����B
	Point mul = (e - s) / (b - a);
	mul /= abs(mul);
	for (int i = 0; i < size(); i++) {
		point[i] *= mul;
	}

	//���s�ړ�
	point0 += s - get_point(id);
}