#include "MySolver.h"

void capture(MySolver backup);

//���_piece_v�ƒ��_dst���������Ă��邩�H�𔻒� (�������Ă����true)
bool MySolver::is_combi_vertex(Vertex piece_v, Vertex dst, double error_dist, double error_angle) {
	if (dst.is_piece) {
		//�s�[�X�p���m
		Point p = piece[piece_v.poly_id].get_point(piece_v.vertex_id);
		Point q = piece[dst.poly_id].get_point(dst.vertex_id);
		if (abs(p - q) > error_dist) return false;

		double ang_p = piece[piece_v.poly_id].get_angle_deg(piece_v.vertex_id);	
		double ang_q = piece[dst.poly_id].get_angle_deg(dst.vertex_id);

		if (360 - error_angle <= ang_p + ang_q && ang_p + ang_q <= 360 + error_angle) return true;
		if (180 - error_angle <= ang_p + ang_q && ang_p + ang_q <= 180 + error_angle) return true;
		return false;
	}
	//�s�[�X�p�Ƙg�p
	Point p = piece[piece_v.poly_id].get_point(piece_v.vertex_id);
	Point q = waku[dst.poly_id].get_point(dst.vertex_id);
	if (abs(p - q) > error_dist) return false;

	double ang_p = piece[piece_v.poly_id].get_angle_deg(piece_v.vertex_id);
	double ang_q = waku[dst.poly_id].get_angle_deg(dst.vertex_id);
	if (abs(ang_p - ang_q) > error_angle) return false;
	return true;
}

//���_src (�������p�`) �̌����x���v�Z���܂��B�Q�Ȃ��������̂ŕ��R�[�h�ł��B�֐������������B
//src�̓s�[�X�̒��_�ł���B
int MySolver::get_combi_level(Vertex src, double error_dist, double error_angle) {
	int i, j;
	int ret = 0;

	Point p = piece[src.poly_id].get_point(src.vertex_id);
	double ang_p = piece[src.poly_id].get_angle_deg(src.vertex_id);

	//���_�̌����x�i�ʒu�Ɗp�x�������Ȃ�)
	for (i = 0; i < waku.size(); i++) {
		int best_score = 0;

		for (j = 0; j < waku[i].size(); j++) {
			if (!is_combi_vertex(src, Vertex(false, i, j), error_dist, error_angle)) { continue; }
			
			//���_p�ƒ��_q���������Ă���i�\���������j�̂ŁA�s�[�Xsrc.poly_id�Ƙgi�̌����x��{�i�I�Ɍv�Z���܂�
			int id1 = src.vertex_id;
			int id2 = j;
			int score = 0;
			do {
				score++;
				id1 = (id1 + 1) % piece[src.poly_id].size();
				id2 = (id2 + 1) % waku[i].size();
			} while (is_combi_vertex(Vertex(true, src.poly_id, id1), Vertex(false, i, id2), error_dist, error_angle));
			if (best_score < score) {
				best_score = score;		//�s�[�Xsrc.poly_id �� �gi�̌����x�́A�ł����]���Ȃ��̂Ƃ���
			}
		}
		ret += best_score;
	}

	for (i = 0; i < piece.size(); i++) {
		if (!isputted[i]) continue;
		int best_score = 0;

		for (j = 0; j < piece[i].size(); j++) {
			if (!is_combi_vertex(src, Vertex(true, i, j), error_dist, error_angle)) { continue; }

			//���_p�ƒ��_q���������Ă���i�\���������j�̂ŁA�s�[�Xsrc.poly_id�ƃs�[�Xi�̌����x��{�i�I�Ɍv�Z���܂�
			int id1 = src.vertex_id;
			int id2 = j;
			int score = 0;
			do {
				score++;
				id1 = (id1 + 1) % piece[src.poly_id].size();
				id2 = (id2 - 1 + piece[i].size()) % piece[i].size();
			} while (is_combi_vertex(Vertex(true, src.poly_id, id1), Vertex(true, i, id2), error_dist, error_angle));
			if (best_score < score) {
				best_score = score;		//�s�[�Xsrc.poly_id �� �s�[�Xi�̌����x�́A�ł����]���Ȃ��̂Ƃ���
			}
		}
		ret += best_score;
	}

	return ret;
}

//���_dst�ƍł��������钸�_src��Ԃ��܂�
Vertex MySolver::get_best_vertex(Vertex dst) {
	int best_score = -1;
	Vertex src = Vertex(false, -1, 0);
	int i, j;
	double error_dist = 1;		//�����x�p�̋��e�ʒu�덷
	double error_angle = 1;		//�����x�p�̋��e�p�x�덷
	double dist = 5;			//�����蔻��̂�邳

	for (i = 0; i < piece.size(); i++) {
		if (isputted[i]) { continue; }
		for (j = 0; j < piece[i].size(); j++) {
			//�u�s�[�Xi�̒��_j�v��dst�ɍ��킹��
			move(Vertex(true, i, j), dst);
			//�����蔻�������
			if (isoverlap(piece[i], dist)) { continue; }
			//�s�[�Xi�̌����x���v�Z����
			int score = get_combi_level(Vertex(true, i, j), error_dist, error_angle);
			//�����x���ő�Ȃ�A�őP�̒��_���X�V����
			if (best_score < score) {
				best_score = score;
				src = Vertex(true, i, j);
			}
		}
	}
	return src;
}

//���������܂�
void MySolver::solve() {
	int i, j;

	//��ƂȂ钸�_�W���̏�����
	for (i = 0; i < (int)waku.size(); i++) {
		for (j = 0; j < waku[i].size(); j++) {
			standard_point.push(Vertex(false, i, j));
		}
	}
	//�ʒu���m�肵�Ă���ӂ̏W���̏�����
	for (i = 0; i < (int)waku.size(); i++) {
		for (j = 0; j < waku[i].size(); j++) {
			putted_line.push_back(Line(waku[i].get_point(j), waku[i].get_point(j + 1)));
		}
	}

	while (!standard_point.empty()) {
		//��ƂȂ钸�_dst����I��
		Vertex dst = standard_point.front();
		//���_dst���u��ƂȂ钸�_�̏W���v����폜����
		standard_point.pop();
		//���_dst�ƃy�A�ɂȂ钸�_src��T��
		Vertex src = get_best_vertex(dst);
		if (src.poly_id < 0) { continue; }
		capture(*this);

		//���_src�����݂���΁A���_dst�ƒ��_src���������悤�ɂ���
		move(src, dst);
		//���_src�������p�`(�s�[�X)���u�ʒu���m�肵�����p�`�v�ɂ���
		int id = src.poly_id;
		isputted[id] = true;
		//���_src�������p�`�i�s�[�X�j�̑S���_���u��ƂȂ钸�_�̏W���v�ɓ����
		for (i = 0; i < piece[id].size(); i++) {
			standard_point.push(Vertex(true, id, i));
		}
		//���_src�������p�`�i�s�[�X�j�̑S�ӂ��u�ʒu���m�肵�Ă���ӂ̏W���v�ɓ����
		for (i = 0; i < piece[id].size(); i++) {
			putted_line.push_back(Line(piece[id].get_point(i), piece[id].get_point(i + 1)));
		}
	}
	capture(*this);
}