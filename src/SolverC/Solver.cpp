//�����ƃ}�b�`����s�[�X��T���I�Ƃ����������s���B�Z�Z�͐l���w��B
//�g�𓮂����Ȃ��I�Ƃ������j�Ŏ����B

#include "Solver.h"
using namespace std;

/*HalfLine�N���X*/
Point HalfLine::angle_vector() {
	return (e - s) / abs(e - s);
}

double HalfLine::eval_function(double angle_vector_imag, double range) {
	//return exp(-(angle_vector_imag) * (angle_vector_imag) / (2 * range * range));
	if (abs(angle_vector_imag) > range) return 0;
	return 1;
}

HalfLine::HalfLine(Point s, Point e) {
	this->s = s;
	this->e = e;
}

double HalfLine::evaluation(HalfLine &hl, double range, double dist) {
	if (abs(hl.s - s) > dist) return 0;
	Point ang = hl.angle_vector() / angle_vector();
	return eval_function(ang.imag(), range);
}

Point HalfLine::start_point() {
	return s;
}

Point HalfLine::end_point() {
	return e;
}

/*Solver�N���X*/

//�s�[�Xsrc_piece_id, �����, ����ɕt�����鑽�p�`���ړ�����B�ړ���� [���]���邩�H, ��]�x�N�g��, ���s�ړ���] ��Ԃ�
tuple<bool, Point, Point> Solver::move(int src_piece_id, int src_line_id, bool turnflag, int dst_line_id) {
	int i;
	vector<Poly*> polys;

	for (i = 0; i < pieces.size(); i++) {
		if (pieces_gid[i] == pieces_gid[src_piece_id] && i != src_piece_id) {
			polys.push_back(&pieces[i]);
		}
	}
	for (i = 0; i < wakus.size(); i++) {
		if (wakus_gid[i] == pieces_gid[src_piece_id]) {
			polys.push_back(&wakus[i]);
		}
	}

	//���]
	bool is_turn = (pieces[src_piece_id].is_turned() != turnflag);
	if (is_turn) { pieces[src_piece_id].turn2(0); }

	//��]
	Point src_s = pieces[src_piece_id].get_point(src_line_id);
	Point src_e = pieces[src_piece_id].get_point(src_line_id + 1);
	Point dst_s = selected_lines[dst_line_id].e;
	Point dst_e = selected_lines[dst_line_id].s;
	Point mul = (dst_e - dst_s) / (src_e - src_s); mul /= abs(mul);
	pieces[src_piece_id].move2(mul, Point(0, 0), Point(0, 0));
	
	//���s�ړ� (�ړ��ʂ́A��L�̉�]�́u���S�v�Ɉˑ��j
	Point trans = dst_s - pieces[src_piece_id].get_point(src_line_id);
	pieces[src_piece_id].move2(Point(1, 0), Point(0, 0), trans);

	//�����菇�ňړ������]�̒��S�Ɖ�]�̒��S�𓝈ꂷ�邱�ƂŁA�ȒP�Ɉړ��ł���
	for (i = 0; i < polys.size(); i++) {
		if (is_turn) { polys[i]->turn2(0); }
		polys[i]->move2(mul, Point(0, 0), trans);
	}

	return tuple<bool, Point, Point>(is_turn, mul, trans);
}

//�ړ������w�� -> move()�̋t�ϊ�������i�������̈ړ����ŃO���[�vgid��move()������̏��, ���Ƃ���ƁAmove()����O�ɃO���[�vgid�͉����ɂ������������߂�j
void Solver::move_reverse(int gid, bool is_turn, Point mul, Point trans) {
	int i;
	vector<Poly*> polys;

	for (i = 0; i < pieces.size(); i++) {
		if (pieces_gid[i] == gid) {
			polys.push_back(&pieces[i]);
		}
	}
	for (i = 0; i < wakus.size(); i++) {
		if (wakus_gid[i] == gid) {
			polys.push_back(&wakus[i]);
		}
	}

	//��]�̒��S, ���]�̒��S��move()�֐��Ɠ��ꂳ���邱�ƁI
	for (i = 0; i < polys.size(); i++) {
		//���s�ړ�
		polys[i]->move2(Point(1, 0), Point(0, 0), -trans);
		//��]
		polys[i]->move2(conj(mul), Point(0, 0), Point(0, 0));
		//���]
		if (is_turn) polys[i]->turn2(0);
	}
}

//�s�[�X�̈ʒu���Œ肳��Ă���Ƃ��A���̃}�b�`���]������B�傫���l�ɂȂ�قǗǂ��]���B
double Solver::evaluation(int gid) {
	int i, j, k;
	double eval = 0;
	
	vector<HalfLine> lines1, lines2;

	for (i = 0; i < selected_lines.size(); i++) {
		Point s = selected_lines[i].s;
		Point e = selected_lines[i].e;
		lines1.push_back(HalfLine(s, e));
		lines1.push_back(HalfLine(e, s));
	}
	for (i = 0; i < pieces.size(); i++) {
		if (pieces_gid[i] != gid) continue;
		for (j = 0; j < pieces[i].size(); j++) {
			Line line = pieces[i].get_line(j);
			lines2.push_back(HalfLine(line.s, line.e));
			lines2.push_back(HalfLine(line.e, line.s));
		}
	}

	for (i = 0; i < lines1.size(); i++) {
		for (j = 0; j < lines2.size(); j++) {
			eval += lines1[i].evaluation(lines2[j], 0.07, 10);
		}
	}
	return eval;
}

//���������]�������ɖ߂�
double Solver::evaluation(int src_piece_id, int src_line_id, bool turnflag, int dst_line_id) {
	int i;

	tuple<bool, Point, Point> move_info = move(src_piece_id, src_line_id, turnflag, dst_line_id);
	double eval = evaluation(pieces_gid[src_piece_id]);
	move_reverse(pieces_gid[src_piece_id], get<0>(move_info), get<1>(move_info), get<2>(move_info));
	return eval;
}

//�ړ��\���𔻒�
bool Solver::can_move(int gid) {
	for (int i = 0; i < selected_lines.size(); i++) {
		if (selected_lines_gid[i] == gid) {
			return false;
		}
	}
	for (int i = 0; i < wakus_gid.size(); i++) {
		if (wakus_gid[i] == gid) {
			return false;
		}
	}
	return true;
}

//�u���邩����
bool Solver::can_marge(int gid_src, int gid_dst) {
	int i, j;
	vector<Poly> _wakus;
	vector<Poly> _pieces;

	for (i = 0; i < wakus.size(); i++) {
		if (wakus_gid[i] == gid_dst) {
			_wakus.push_back(wakus[i]);
		}
	}
	for (i = 0; i < pieces.size(); i++) {
		if (pieces_gid[i] == gid_dst || pieces_gid[i] == gid_src) {
			_pieces.push_back(get_scaledown_poly(pieces[i]));
		}
	}

	if (_wakus.size() > 0) {
		//�ǂ��炩�̃O���[�v�ɘg������ꍇ
		//�ǂ̃s�[�X������g�̒��ɓ����Ă��邩�H�𒲂ׂ�
		for (i = 0; i < _pieces.size(); i++) {
			for (j = 0; j < _wakus.size(); j++) {
				if (!_wakus[j].is_hit(_pieces[i]) && _wakus[j].is_contain(_pieces[i])) {
					break;
				}
			}
			if (j == _wakus.size()) {
				return false;
			}
		}
		return true;
	}
	else {
		//�s�[�X�����̏ꍇ
		//�s�[�X�΂ɂ��āA�݂��Ɍ����E����Ȃ����H�𒲂ׂ�
		for (i = 0; i < _pieces.size(); i++) {
			for (j = i + 1; j < _pieces.size(); j++) {
				if (_pieces[i].is_hit(_pieces[j]) || _pieces[i].is_contain(_pieces[j]) || _pieces[i].is_contained(_pieces[j])) {
					return false;
				}
			}
		}
		return true;
	}
	//�_�~�[�̖߂�l
	return false;
}

//����p����֐�
void Solver::think(int src_piece_id, int src_line_id, bool turnflag, int dst_line_id, double &best_score, tuple<int, int, bool, int> &best) {
	double score = evaluation(src_piece_id, src_line_id, false, dst_line_id);
	if (best_score < score) {
		//�}�[�W�\���𔻒�
		tuple<bool, Point, Point> move_info = move(src_piece_id, src_line_id, turnflag, dst_line_id);
		bool flag = can_marge(pieces_gid[src_piece_id], selected_lines_gid[dst_line_id]);
		move_reverse(pieces_gid[src_piece_id], get<0>(move_info), get<1>(move_info), get<2>(move_info));
		if (flag) {
			//�L�^
			best_score = score;
			best = tuple<int, int, bool, int>(src_piece_id, src_line_id, turnflag, dst_line_id);
		}
	}
}


void Solver::prepare() {
	int id = 0;
	int i;

	for (i = 0; i < pieces.size(); i++) pieces_gid.push_back(id++);
	for (i = 0; i < wakus.size(); i++) wakus_gid.push_back(id++);
}

void Solver::select_line(complex<double> mousePoint) {
	int i, tyoten;
	vector<Line> lines;
	vector<int> lines_gid;

	mousePoint /= scale;	//�`����W(800 * 600)���f�[�^���W(x0.45)�ɕϊ�

	for (i = 0; i < pieces.size(); i++) {
		for (tyoten = 0; tyoten < pieces[i].size(); tyoten++) {
			lines.push_back(pieces[i].get_line(tyoten));
			lines_gid.push_back(pieces_gid[i]);
		}
	}

	for (i = 0; i < wakus.size(); i++) {
		for (tyoten = 0; tyoten < wakus[i].size(); tyoten++) {
			lines.push_back(wakus[i].get_line(tyoten));
			lines_gid.push_back(wakus_gid[i]);
		}
	}

	double min_dist = 1145141919;
	int best = -1;

	for (i = 0; i < lines.size(); i++) {
		double dist = lines[i].dist(mousePoint);
		if (dist < min_dist) {
			min_dist = dist;
			best = i;
		}
	}

	//lines[best]�̑I����Ԃ�ύX���Ȃ��ꍇ
	if (selected_lines.size() > 0 && selected_lines_gid[0] != lines_gid[best]) {
		return;
	}
	if (min_dist > 20) {
		return;
	}

	//lines[best]�̑I����Ԃ�ύX����
	vector<Line>::iterator it = find(selected_lines.begin(), selected_lines.end(), lines[best]);
	if (it != selected_lines.end()) {
		int pos = (it - selected_lines.begin()) / sizeof(Line);
		selected_lines.erase(it);
		selected_lines_gid.erase(selected_lines_gid.begin() + pos);
	}
	else {
		selected_lines.push_back(lines[best]);
		selected_lines_gid.push_back(lines_gid[best]);
	}
}

void Solver::release_line() {
	selected_lines.clear();
	selected_lines_gid.clear();
}

void Solver::draw() {
	SolverBase::draw(true);
	
	for (int i = 0; i < selected_lines.size(); i++) {
		Point start = selected_lines[i].s * scale;
		Point end   = selected_lines[i].e * scale;
		DrawLine((int)start.real(), (int)start.imag(), (int)end.real(), (int)end.imag(), GetColor(180, 180, 20), 3);
	}
}

//�I�����ꂽ��selected_edges�ƍł��}�b�`����u�����Œu��
void Solver::solve() {
	if (selected_lines.size() <= 1) { return; }

	double best_score = -100000;
	tuple<int, int, bool, int> best;

	for (int src_piece_id = 0; src_piece_id < pieces.size(); src_piece_id++) {
		if (!can_move(pieces_gid[src_piece_id])) { continue; }
		for (int dst_line_id = 0; dst_line_id < selected_lines.size(); dst_line_id++) {
			for (int src_line_id = 0; src_line_id < pieces[src_piece_id].size(); src_line_id++) {
				think(src_piece_id, src_line_id, true, dst_line_id, best_score, best);
				think(src_piece_id, src_line_id, false, dst_line_id, best_score, best);
			}
		}
	}

	if (best_score > -100000) {
		//�ړ�
		move(get<0>(best), get<1>(best), get<2>(best), get<3>(best));

		//�f�o�b�O
		printfDx("%f\n", best_score);

		//�O���[�v�̍X�V
		int src_gid = pieces_gid[get<0>(best)];
		int dst_gid = selected_lines_gid[get<3>(best)];

		for (int i = 0; i < pieces.size(); i++) {
			if (pieces_gid[i] == src_gid) {
				pieces_gid[i] = dst_gid;
			}
		}

		for (int i = 0; i < wakus.size(); i++) {
			if (wakus_gid[i] == src_gid) {
				wakus_gid[i] = dst_gid;
			}
		}
	}
}