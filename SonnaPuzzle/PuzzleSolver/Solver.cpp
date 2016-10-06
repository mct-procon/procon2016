#include "Solver.h"

void capture(Solver &shot);

//�s�[�Xsrc_poly_id�𓮂��� (��src���dst�ɂ�������C���[�W)
void Solver::move(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id) {
	Poly &src = pieces[src_poly_id];
	Poly &dst = (dst_is_piece ? pieces[dst_poly_id] : wakus[dst_poly_id]);
	
	backup_piece = src;
	moved_piece_id = src_poly_id;

	if (src.is_turn != src_turnflag) { src.turn(); }
	src.move(src_line_id, dst.points[dst_line_id + 1], dst.points[dst_line_id]);
}


//�s�[�X�𓮂����O�̈ʒu�ɖ߂�
void Solver::restore() {
	pieces[moved_piece_id] = backup_piece;
}


//2���p�`��]��
double Solver::evaluation(Poly &poly1, Poly &poly2, bool is_poly2_piece) {

	if (poly2.is_hit(scaledown_poly(poly1))) { return -114514; }
	if (!is_poly2_piece) { if (poly1.is_cover(scaledown_poly(poly2)) && !poly2.is_cover(scaledown_poly(poly1))) { return -114514; } }

	double dist_error = dist;
	double angle_error = 3;
	double score = 0;

	//�ӂ̕]��
	for (int i = 0; i < poly1.size(); i++) {
		for (int j = 0; j < poly2.size(); j++) {
			Line line1 = Line(poly1.points[i], poly1.points[i + 1]);
			Line line2 = Line(poly2.points[j], poly2.points[j + 1]);
			score += line1.machi_score(line2, dist_error, angle_error);
		}
	}

	//�p�̕]���i360��)
	for (int i = 0; i < poly1.size(); i++) {
		for (int j = 0; j < poly2.size(); j++) {
			//�ppoly1.points[i], �ppoly2.points[j]����v���Ă���΃X�R�A���Z
			if (abs(poly1.points[i] - poly2.points[j]) > dist_error) continue;
			Line line1a = Line(poly1.points[i], poly1.points[i + 1]);
			Line line1b = Line(poly1.points[i], poly1.points[(i - 1 + poly1.size()) % poly1.size()]);
			Line line2b = Line(poly2.points[j], poly2.points[j + 1]);
			Line line2a = Line(poly2.points[j], poly2.points[(j - 1 + poly2.size()) % poly2.size()]);
			double score_a = line1a.machi_score(line2a, dist_error, angle_error); if (!(score_a == 1 || score_a == 4)) continue;	//��������v���Ă��邩�𔻒�
			double score_b = line1b.machi_score(line2b, dist_error, angle_error); if (!(score_b == 1 || score_b == 4)) continue;	//��������v���Ă��邩�𔻒�
			score += 3;
			break;
		}
	}
	return score;
}


//�ړ����]������߂�, �̈�A���������
double Solver::action_score(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id) {
	move(src_turnflag, src_poly_id, src_line_id, dst_is_piece, dst_poly_id, dst_line_id);

	double score;
	if (dst_is_piece) score = evaluation(pieces[src_poly_id], pieces[dst_poly_id], true);
	else score = evaluation(pieces[src_poly_id], wakus[dst_poly_id], false);

	restore();
	return score;
}


//�x�X�g�ȑg�ł�������, �������g�ݍ��킹���Ȃ��Ƃ�false��Ԃ�
//�藝�F�p�Y���������ł���˂����A,B(A,B�͕ʂ̑��p�`�ɑ���)�ɂ��āA�uA.length()==B.length()�����藧���A���AA��B���������邱�ƂŃp�Y���������ł���v�B
//�܂�c��������ӂ̒������������Ƃ������A�������Ă݂�΂悢�B�i����ŕ��όv�Z�ʂ�����j
bool Solver::connect() {
	int turnflag, i, j, k, l;
	typedef tuple<double, bool, int, int, bool, int, int> T;
	vector<T> score_move;
	double score;

	for (i = 0; i < pieces.size(); i++) {
		for (turnflag = 0; turnflag <= 1; turnflag++) {
			for (j = 0; j < pieces[i].size(); j++) {
				double length_error = dist;
				double src_edge_length = abs(pieces[i].points[j + 1] - pieces[i].points[j]);
			
				for (k = 0; k < wakus.size(); k++) {
					for (l = 0; l < wakus[k].size(); l++) {
						if (abs(src_edge_length - abs(wakus[k].points[l + 1] - wakus[k].points[l])) > length_error) { continue; }	//��������ӂ̒������������Ȃ����, ��蒼��
						score = action_score((turnflag == 1), i, j, false, k, l); if (score > 0) { score_move.push_back(T(score, (turnflag == 1), i, j, false, k, l)); }
					}
				}
				for (k = 0; k < pieces.size(); k++) {
					if (k == i) continue;
					for (l = 0; l < pieces[k].size(); l++) {
						if (abs(src_edge_length - abs(pieces[k].points[l + 1] - pieces[k].points[l])) > length_error) { continue; }	//��������ӂ̒������������Ȃ����, ��蒼��
						score = action_score((turnflag == 1), i, j, true, k, l); if (score > 0) { score_move.push_back(T(score, (turnflag == 1), i, j, true, k, l)); }
					}
				}
			}
		}
	}

	if (score_move.size() == 0) {	
		for (i = 0; i < pieces.size(); i++) {
			for (turnflag = 0; turnflag <= 1; turnflag++) {
				for (j = 0; j < pieces[i].size(); j++) {			
					for (k = 0; k < wakus.size(); k++) {
						for (l = 0; l < wakus[k].size(); l++) {
							score = action_score((turnflag == 1), i, j, false, k, l); if (score > 0) { score_move.push_back(T(score, (turnflag == 1), i, j, false, k, l)); }
						}
					}
					for (k = 0; k < pieces.size(); k++) {
						if (k == i) continue;
						for (l = 0; l < pieces[k].size(); l++) {
							score = action_score((turnflag == 1), i, j, true, k, l); if (score > 0) { score_move.push_back(T(score, (turnflag == 1), i, j, true, k, l)); }
						}
					}
				}
			}
		}
	}

	//��ԃX�R�A�������g�ݍ��킹�ł�������
	if (score_move.size() == 0) return false;
	sort(score_move.begin(), score_move.end(), greater<T>());

	for (i = 0; i < score_move.size(); i++) {
		move(get<1>(score_move[i]), get<2>(score_move[i]), get<3>(score_move[i]), get<4>(score_move[i]), get<5>(score_move[i]), get<6>(score_move[i]));

		//���p�`�̃}�[�W
		int src_id = get<2>(score_move[i]);
		int dst_id = get<5>(score_move[i]);
		bool dst_is_piece = get<4>(score_move[i]);
		Poly &src = pieces[src_id];
		Poly &dst = (dst_is_piece ? pieces[dst_id] : wakus[dst_id]);
	
		//�]���֐�evaluation�ŗp�����Ă��鋖�e�덷�̒l
		double dist_error = dist;
		double angle_error = 3;

		if (!marge_poly.can_marge(dist_error, angle_error, src, dst, dst_is_piece)) { restore(); continue; }
		Poly marged_poly = marge_poly.marge_poly(dist_error * 1.5, angle_error, src, dst, dst_is_piece);
		if (marged_poly.size() <= 0) { if (i + 1 < score_move.size()) restore(); continue; }

		//�ǉ�, �폜( = �Ƃ肠�������_����0�ɂ���, �]���r��Ԃɂ��� )
		if (dst_is_piece) {
			pieces.push_back(marged_poly);
			pieces[src_id].points.clear();
			pieces[dst_id].points.clear();
		}
		else {
			wakus.push_back(marged_poly);
			pieces[src_id].points.clear();
			wakus[dst_id].points.clear();
		}
		return true;
	}
	return false;
}

//����
void Solver::solve() {
	capture(*this);
	while (connect()) { 
		capture(*this); 
	}
	capture(*this);
}