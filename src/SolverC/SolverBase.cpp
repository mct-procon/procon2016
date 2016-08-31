#include "SolverBase.h"

//���p�`�̕`��
void SolverBase::draw_poly(Poly poly, int red, int green, int blue, double scale) {
	for (int j = 0; j < poly.size(); j++) {
		Point s = poly.get_point(j) * scale;
		Point e = poly.get_point(j + 1) * scale;
		DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(red, green, blue), 2);
		//DrawFormatString((int)s.real(), (int)s.imag(), 0, "%d", j);
	}
}

//���p�`��ǂݍ���
Poly SolverBase::read_poly(ifstream &ifs, bool is_piece) {
	Point point0;
	vector<Point> points;

	//���_��
	int tyotenNum;
	ifs >> tyotenNum;

	//x���W -> y���W
	vector<double> x, y;
	x.resize(tyotenNum);
	y.resize(tyotenNum);
	for (int i = 0; i < tyotenNum; i++) ifs >> x[i];
	for (int i = 0; i < tyotenNum; i++) ifs >> y[i];

	//���_��̐���
	for (int i = 0; i < tyotenNum; i++) points.push_back(Point(x[i], y[i]));
	point0 = points[0];
	for (int i = 0; i < tyotenNum; i++) points[i] -= point0;

	//���p�`��Ԃ�
	return Poly(point0, points, is_piece);
}

//���p�`����������
void SolverBase::print_poly(ofstream &ofs, Poly poly) {
	//���_��
	ofs << poly.size() << endl;

	//x���W
	ofs << poly.get_point(0).real();
	for (int i = 1; i < poly.size(); i++) ofs << poly.get_point(i).real() << endl;
	ofs << endl;

	//y���W
	ofs << poly.get_point(0).imag();
	for (int i = 1; i < poly.size(); i++) ofs << poly.get_point(i).imag() << endl;
	ofs << endl;

	//���s
	ofs << endl;

}


//�����蔻��p�̗֊s��Ԃ��B
//dist > 0�̂Ƃ��c�s�[�X�̏ꍇ�͓����蔻��g�������ɂł���B�g�̏ꍇ�͓����蔻��g���O���ɂł���B
Poly SolverBase::get_scaledown_poly(Poly poly) {
	int i;
	int n = poly.size();
	vector<Line> line(n);
	Point point0;
	vector<Point> point(n);

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
	return Poly(point0, point, poly.is_piece());
}

//���p�`waku�̓����ɒu����Ă���s�[�X�̔ԍ���Ԃ�
vector<int> SolverBase::get_contain_piece_id(Poly waku) {
	int id;
	vector<int> ret;

	waku = get_scaledown_poly(waku);		//�����蔻��g����

	for (id = 0; id < pieces.size(); id++) {
		if (!waku.is_hit(pieces[id]) && waku.is_contain(pieces[id]) && pieces[id].is_putted) {
			ret.push_back(id);
		}
	}
	return ret;
}


//���p�`src�𓮂����B
//turnmode�c���p�`�̔��]���. ������ԂƔ�ׂĔ��]���Ă����Ԃɂ������Ƃ���true�ɂ���B
void SolverBase::move(Poly &src, int src_id, bool src_turnmode, Poly &dst, int dst_id, bool dst_turnmode) {
	if (src.is_turned() != src_turnmode) src.turn();
	if (dst.is_turned() != dst_turnmode) dst.turn();
	Point s = dst.get_point(dst_id);
	Point e = dst.get_point(dst_id - 1);
	src.move(src_id, s, e);
}

//poly��u���邩�H
//is_piece�cpoly���s�[�X�Ȃ�true, �g�Ȃ�false
bool SolverBase::canput(Poly poly) {
	int i;

	poly = get_scaledown_poly(poly);		//�����蔻��g����

	for (i = 0; i < pieces.size(); i++) if (pieces[i].is_putted && poly.is_hit(pieces[i])) return false;		//�s�[�X�ƐڐG������_��
	for (i = 0; i < wakus.size(); i++) if (wakus[i].is_putted && poly.is_hit(wakus[i])) return false;			//�g�ƐڐG������_��

	for (i = 0; i < wakus.size(); i++) if (wakus[i].is_putted && poly.is_contain(wakus[i])) return false;		//�g���܂񂾂�_��
	for (i = 0; i < pieces.size(); i++) if (pieces[i].is_putted && poly.is_contained(pieces[i])) return false;	//�s�[�X�Ɋ܂܂ꂽ��_��

	if (poly.is_piece()) {
		for (i = 0; i < pieces.size(); i++) if (pieces[i].is_putted && poly.is_contain(pieces[i])) return false;	//�s�[�X���܂񂾂�_��
		return true;
	}
	else {
		for (i = 0; i < wakus.size(); i++) if (wakus[i].is_putted && poly.is_contained(wakus[i])) return false;		//�g�Ɋ܂܂ꂽ��_��
		return true;
	}
	return false;	//�_�~�[�̖߂�l
}

//�����𐶐�����B�i���������g�����ɖ߂��B�g���̃s�[�X���ꏏ�ɓ������B�j
void SolverBase::make_answer() {
	int id, i;

	for (id = 0; id < wakus.size(); id++) {
		vector<int> ids = get_contain_piece_id(wakus[id]);
		//�g�̔��]
		bool is_turn = wakus[id].is_turned();
		if (is_turn) wakus[id].turn2(0);
		//�g�̉�]
		Point src_s = wakus[id].get_point(0);
		Point src_e = wakus[id].get_point(1);
		Point dst_s = wakusIni[id].get_point(0);
		Point dst_e = wakusIni[id].get_point(1);
		Point mul = (dst_e - dst_s) / (src_e - src_s); mul /= abs(mul);
		wakus[id].move2(mul, Point(0, 0), Point(0, 0));
		//�g�̕��s�ړ�
		Point trans = wakusIni[id].get_point(0) - wakus[id].get_point(0);
		wakus[id].move2(Point(1, 0), Point(0, 0), trans);
		wakus[id].is_putted = true;

		//�s�[�X�̈ړ�
		for (i = 0; i < ids.size(); i++) {
			if (is_turn) pieces[ids[i]].turn2(0);
			pieces[ids[i]].move2(mul, Point(0, 0), trans);
		}
	}
}


//������
SolverBase::SolverBase() {
	scale = 0.45;
}

//�`��
void SolverBase::draw(bool is_draw_didnot_put) {
	int i;

	//�g
	for (i = 0; i < wakus.size(); i++) { 
		draw_poly(wakus[i], 0, 0, 0, scale);
	}

	//�s�[�X
	for (i = 0; i < pieces.size(); i++) {
		if (pieces[i].is_putted) {
			draw_poly(pieces[i], 0, 0, 255, scale);
		}
	}

	//�u���ĂȂ��s�[�X
	for (i = 0; i < pieces.size(); i++) {
		if (!pieces[i].is_putted && is_draw_didnot_put) {
			draw_poly(pieces[i], 0, 255, 0, scale);
		}
	}

	//�����蔻��g
	/*for (i = 0; i < wakus.size(); i++) {
		if (!wakus[i].is_putted && !is_draw_didnot_put) { continue; }
		draw_poly(get_scaledown_poly(wakus[i]), 255, 0, 255, scale);
	}
	for (i = 0; i < pieces.size(); i++) {
		if (!pieces[i].is_putted && !is_draw_didnot_put) { continue; }
		draw_poly(get_scaledown_poly(pieces[i]), 255, 0, 255, scale);
	}*/

	//�X�R�A
	int score = 0;
	for (i = 0; i < pieces.size(); i++) { score += pieces[i].is_putted; }
	DrawFormatString(500, 150, 0, "�u�����΂̐� = %d (/%d)", score, (int)pieces.size());

	//���߂�����[%]
	double area_sum = 0, area_putted = 0;
	for (i = 0; i < pieces.size(); i++) {
		area_sum += pieces[i].area();
		area_putted += pieces[i].is_putted * pieces[i].area();
	}
	DrawFormatString(500, 180, 0, "���߂����� = %.1f[%%]", area_putted * 100.0 / area_sum);
}

//����
void SolverBase::input(string filename, double dist) {
	ifstream ifs(filename); if (ifs.fail()) return;
	int num;

	this->dist = dist;

	//�g
	ifs >> num;
	for (int i = 0; i < num; i++) wakus.push_back(read_poly(ifs, false));

	//�s�[�X
	ifs >> num;
	for (int i = 0; i < num; i++) pieces.push_back(read_poly(ifs, true));

	//�g�̒��_������v���ɂ���
	for (int i = 0; i < wakus.size(); i++) {
		if (wakus[i].area() > 0) {
			wakus[i].point_reverse();
		}
	}

	//�s�[�X�̒��_��𔽎��v���ɂ���
	for (int i = 0; i < pieces.size(); i++) {
		if (pieces[i].area() < 0) {
			pieces[i].point_reverse();
		}
	}

	//�����g
	for (int i = 0; i < wakus.size(); i++) wakusIni.push_back(wakus[i]);
	ifs.close();
}

//�o��
void SolverBase::print(string filename) {
	ofstream ofs(filename); if (ofs.fail()) return;

	//�g
	ofs << wakus.size() << endl;
	for (int i = 0; i < wakus.size(); i++) print_poly(ofs, wakus[i]);
	//�s�[�X
	ofs << pieces.size() << endl;
	for (int i = 0; i < pieces.size(); i++) print_poly(ofs, pieces[i]);
	ofs.close();
}
