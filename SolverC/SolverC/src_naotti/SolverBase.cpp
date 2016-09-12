#include "SolverBase.h"

//���p�`��ǂݍ���
Poly SolverBase::input_poly(ifstream &ifs, bool is_clockwise) {
	int n;
	vector<Point> points;	//���p�`�̒��_��
	vector<Line> lines;		//�\���������

	ifs >> n;
	for (int i = 0; i < n; i++) {
		double x, y;
		ifs >> x >> y;
		points.push_back(Point(x, y));
	}
	points.push_back(points[0]);

	for (int i = 0; i < n; i++) {
		lines.push_back(Line(points[i], points[i + 1]));
	}
	Poly ret = Poly(points, lines);

	if (is_clockwise && ret.area() > 0) ret.point_reverse();
	if (!is_clockwise && ret.area() < 0) ret.point_reverse();
	return ret;
}


//�����蔻��p�̗֊s��Ԃ��B
//dist > 0�̂Ƃ��c�s�[�X�̏ꍇ�͓����蔻��g�������ɂł���B�g�̏ꍇ�͓����蔻��g���O���ɂł���B
Poly SolverBase::scaledown_poly(Poly &poly) {
	int i;
	int n = poly.size();
	vector<Line> lines(n);
	vector<Point> points(n + 1);

	//���p�`�����ɐ���������
	for (i = 0; i < n; i++) {
		lines[i] = Line(poly.points[i], poly.points[i + 1]);
		lines[i].move(-dist);
	}
	for (i = 0; i < n; i++) {
		int id1 = i - 1; if (id1 < 0) id1 += n;
		int id2 = i + 1; if (id2 >= n) id2 -= n;
		lines[i].cut(lines[id1], lines[id2]);	//lines[i].s <- lines[id1]�Ƃ̌�_, lines[i].e <- lines[id2]�Ƃ̌�_
	}

	//���p�`���\��
	for (i = 0; i < n; i++) {
		points[i] = lines[0].s;
	}
	points[n] = points[0];
	return Poly(points, lines);		//��Q�����͉��ł��ǂ�
}


//���� (�V�����t�H�[�}�b�g�œǂ�)
void SolverBase::input(string filename, double dist) {
	this->dist = dist;

	ifstream ifs(filename); if (ifs.fail()) return;
	int poly_num;

	ifs >> poly_num;
	for (int i = 0; i < poly_num; i++) { wakus.push_back(input_poly(ifs, true)); }
	ifs >> poly_num;
	for (int i = 0; i < poly_num; i++) { pieces.push_back(input_poly(ifs, false)); }
	ifs.close();
}


//�\�� (���S���W, �g�嗦, ��ʃT�C�YX, ��ʃT�C�YY)
void SolverBase::draw(Point center, double scale, double windowSizeX, double windowSizeY) {
	int i, j;
	Point s, e;
	Point trans = Point(windowSizeX / 2, windowSizeY / 2);

	//��
	for (i = 0; i < wakus.size(); i++) {
		for (j = 0; j < wakus[i].lines.size(); j++) {
			s = wakus[i].lines[j].s;
			e = wakus[i].lines[j].e;
			//�ϊ�
			s -= center; s *= scale; s += trans;
			e -= center; e *= scale; e += trans;
			//�\��
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 0, 0), 2);
		}
	}

	for (i = 0; i < pieces.size(); i++) {
		for (j = 0; j < pieces[i].size(); j++) {
			s = pieces[i].lines[j].s;
			e = pieces[i].lines[j].e;
			//�ϊ�
			s -= center; s *= scale; s += trans;
			e -= center; e *= scale; e += trans;
			//�\��
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 255, 0), 2);
		}
	}

	//���_�ԍ� (�}�E�X���狗��50�ȓ��̂��̂����\������)
	int mx, my;
	GetMousePoint(&mx, &my);
	Point mouse = Point((double)mx, (double)my);

	for (i = 0; i < wakus.size(); i++) {
		for (j = 0; j < wakus[i].size(); j++) {
			s = wakus[i].points[j];
			//�ϊ�
			s -= center; s *= scale; s += trans;
			if (abs(mouse - s) > 50) continue;
			//�\��
			DrawFormatString((int)s.real(), (int)s.imag(), GetColor(255, 0, 0), "%d", j);
		}
	}

	for (i = 0; i < pieces.size(); i++) {
		for (j = 0; j < pieces[i].size(); j++) {
			s = pieces[i].points[j];
			//�ϊ�
			s -= center; s *= scale; s += trans;
			if (abs(mouse - s) > 50) continue;
			//�\��
			DrawFormatString((int)s.real(), (int)s.imag(), GetColor(0, 0, 255), "%d", j);
		}
	}
}
