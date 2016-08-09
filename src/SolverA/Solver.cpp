#include "Solver.h"
#include "DxLib.h"

//���_src�𒸓_dst�ɂ�������悤�ɁA�ړ�����
void Solver::move(Vertex src, Vertex dst) {
	Point s, e;
	if (dst.is_piece) {
		s = piece[dst.poly_id].get_point(dst.vertex_id + 1);
		e = piece[dst.poly_id].get_point(dst.vertex_id);
	}
	else {
		s = waku[dst.poly_id].get_point(dst.vertex_id);
		e = waku[dst.poly_id].get_point(dst.vertex_id + 1);
	}
	piece[src.poly_id].move(src.vertex_id, s, e);
}

//�upoly�̏k�����p�`�v��Ԃ��܂�. dist���傫���قǏk�����p�`�͏������B
Poly Solver::get_scaledown_poly (Poly &poly, double dist) {
	int i;
	int n = poly.size();
	vector<Line> line(n);
	Point point0;
	vector<Point> point(n);
	Poly small_poly;

	//���p�`�����ɐ���������
	for (i = 0; i < n; i++) {
		line[i] = Line(poly.get_point(i), poly.get_point(i + 1));
		line[i].move_line(-dist);
	}
	for (i = 0; i < n; i++) {
		int id1 = (i - 1 + n) % n;
		int id2 = (i + 1) % n;
		line[i].cut_line(line[id1], line[id2]);	//lines[i].s <- lines[id1]�Ƃ̌�_, lines[i].e <- lines[id2]�Ƃ̌�_
	}

	//���p�`���\��
	point0 = line[0].s;
	for (i = 0; i < n; i++) {
		point[i] = line[i].s - point0;
	}
	small_poly = Poly(point0, point);
	return small_poly;
}

//�����蔻��Bpoly�����ɒu�����s�[�X or �g�Əd�Ȃ��true�B
//dist = 0������, dist��傫������قǁu������Ȃ��Ȃ�v
//����version�ł��Bputted_line.size()��100�𒴂�����A�������������B
bool Solver::isoverlap(Poly &poly, double dist) {
	int i, j;
	Poly small_poly = get_scaledown_poly (poly, dist);	//�s�[�X���k�����邱�ƂŁA�C���M�����[�ȃP�[�X�̔�����h���B

	//���̌�������
	double lx = 10000, ly = -10000, rx = 10000, ry = -10000;
	vector<Line> line(poly.size());

	for (i = 0; i < small_poly.size(); i++) {
		Point p = small_poly.get_point(i);
		lx = min(lx, p.real());
		rx = max(rx, p.real());
		ly = min(ly, p.imag());
		ry = max(ry, p.imag());
		line[i] = Line(p, small_poly.get_point(i + 1));
	}

	for (i = 0; i < putted_line.size(); i++) {
		double min_x = min(putted_line[i].s.real(), putted_line[i].e.real()); if (min_x > rx) { continue; }
		double max_x = max(putted_line[i].s.real(), putted_line[i].e.real()); if (max_x < lx) { continue; }
		double min_y = min(putted_line[i].s.imag(), putted_line[i].e.imag()); if (min_y > ry) { continue; }
		double max_y = max(putted_line[i].s.imag(), putted_line[i].e.imag()); if (max_y < ly) { continue; }
		
		for (j = 0; j < small_poly.size(); j++) {
			if (putted_line[i].ishit(line[j])) {
				return true;
			}
		}
	}

	//�_�̓��O���� (small_poly�̑�\�_1�ɂ��Ĕ��肷��΂悢�j
	Point p = small_poly.get_point(0);
	Line hl = Line(p, Point(p.real(), 10000));
	int cnt = 0;

	//������hl�Ɠ�����putted_line[i]�̃J�E���g
	for (i = 0; i < putted_line.size(); i++) {
		if (hl.ishit(putted_line[i])) {
			cnt++;
		}
	}
	//�����񓖂�������A�g�̊O���s�[�X�̒��ɓ_������
	if (cnt % 2 == 0) {
		return true;
	}
	return false;
}

//�񓚂�\�����܂�
void Solver::draw(bool is_draw_didnot_put) {
	int i, j;
	double scale = 0.45;

	//�g
	for (i = 0; i < (int)waku.size(); i++) {
		for (j = 0; j < waku[i].size(); j++) {
			Point s = waku[i].get_point(j) * scale;
			Point e = waku[i].get_point(j + 1) * scale;
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 0, 0), 2);
			DrawFormatString((int)s.real(), (int)s.imag(), GetColor(255, 0, 0), "%d", j);
		}
	}
	
	//�s�[�X
	for (i = 0; i < (int)piece.size(); i++) {
		if (!isputted[i] && !is_draw_didnot_put) { continue; }
		for (j = 0; j < piece[i].size(); j++) {
			Point s = piece[i].get_point(j) * scale;
			Point e = piece[i].get_point(j + 1) * scale;

			if (isputted[i])
				DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 0, 255), 2);
			else
				DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 255, 0), 2);
		}
	}

	//�����蔻��g
	for (i = 0; i < (int)piece.size(); i++) {
		if (!isputted[i] && !is_draw_didnot_put) { continue; }
		Poly poly = get_scaledown_poly(piece[i], 5);
		for (j = 0; j < poly.size(); j++) {
			Point s = poly.get_point(j) * scale;
			Point e = poly.get_point(j + 1) * scale;
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(255, 0, 255), 2);
		}
	}

	//�X�R�A
	int score = 0;
	for (i = 0; i < (int)piece.size(); i++) {
		if (isputted[i]) {
			score++;
		}
	}
	DrawFormatString(500, 150, 0, "�u�����΂̐� = %d (/%d)", score, (int)piece.size());

	//�ʐσ��X�g
	vector<double> area; area.resize(piece.size());
	for (i = 0; i < (int)piece.size(); i++) {
		area[i] = 0;
		for (j = 0; j < piece[i].size(); j++) {
			double u = piece[i].get_point(j).real() * piece[i].get_point(j + 1).imag();
			double v = piece[i].get_point(j).imag() * piece[i].get_point(j + 1).real();
			double det = u - v;
			area[i] += det;
		}
		area[i] *= 0.5;
	}

	//���߂�����[%]
	double area_sum = 0, area_putted = 0;
	for (i = 0; i < (int)piece.size(); i++) {
		area_sum += area[i];
		if (isputted[i]) area_putted += area[i];
	}
	DrawFormatString(500, 180, 0, "���߂����� = %.1f[%%]", area_putted * 100.0 / area_sum);

	//�v��
	DrawFormatString(500, 210, 0, "�v���ɓ������� = %d[��]", timer);
}

//���������܂�
Solver::Solver() {
	for (int i = 0; i < 50; i++) {
		isputted[i] = false;
	}
}

//�ǂݍ��݂܂�
void Solver::input(string filename) {
	ifstream ifs(filename);
	if (ifs.fail()) { return; }

	int waku_num, piece_num;
	int point_num;
	int x[50], y[50];

	//�g
	ifs >> waku_num;
	for (int i = 0; i < waku_num; i++) {
		//���_��
		ifs >> point_num;
		//X���W
		for (int j = 0; j < point_num; j++) {
			ifs >> x[j];
		}
		//Y���W
		for (int j = 0; j < point_num; j++) {
			ifs >> y[j];
		}
		//�\�z
		Point point0;
		vector<Point> point;
		for (int j = 0; j < point_num; j++) {
			point.push_back(Point(x[j], y[j]));
		}
		//���΍��W�`���ɂ���
		point0 = point[0];
		for (int j = 0; j < point_num; j++) {
			point[j] = point[j] - point0;
		}
		//�g����ǉ�
		waku.push_back(Poly(point0, point));
	}

	//�s�[�X
	ifs >> piece_num;
	for (int i = 0; i < piece_num; i++) {
		//���_��
		ifs >> point_num;
		//X���W
		for (int j = 0; j < point_num; j++) {
			ifs >> x[j];
		}
		//Y���W
		for (int j = 0; j < point_num; j++) {
			ifs >> y[j];
		}
		//�\�z
		Point point0;
		vector<Point> point;
		for (int j = 0; j < point_num; j++) {
			point.push_back(Point(x[j], y[j]));
		}
		//���΍��W�`���ɂ���
		point0 = point[0];
		for (int j = 0; j < point_num; j++) {
			point[j] = point[j] - point0;
		}
		//�s�[�X��ǉ�
		piece.push_back(Poly(point0, point));
	}
}

//�񓚂��������݂܂�
void Solver::print(string filename) {
	ofstream ofs(filename);

	//�g��
	ofs << waku.size() << endl;
	for (int i = 0; i < (int)waku.size(); i++) {
		//���_��
		int n = waku[i].size();
		ofs << n << endl;
		//X���W
		ofs << waku[i].get_point(0).real();
		for (int j = 1; j < n; j++) {
			ofs << " " << waku[i].get_point(j).real();
		}
		ofs << endl;
		//Y���W
		ofs << waku[i].get_point(0).imag();
		for (int j = 1; j < n; j++) {
			ofs << " " << waku[i].get_point(j).imag();
		}
		ofs << endl;
		//���s
		ofs << endl;
	}

	//�s�[�X
	ofs << piece.size() << endl;
	for (int i = 0; i < (int)piece.size(); i++) {
		//���_��
		int n = piece[i].size();
		ofs << n << endl;
		//X���W
		ofs << piece[i].get_point(0).real();
		for (int j = 1; j < n; j++) {
			ofs << " " << piece[i].get_point(j).real();
		}
		ofs << endl;
		//Y���W
		ofs << piece[i].get_point(0).imag();
		for (int j = 1; j < n; j++) {
			ofs << " " << piece[i].get_point(j).imag();
		}
		ofs << endl;
		//���s
		ofs << endl;
	}	
}