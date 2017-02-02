//�Q�ƌ^(Poly&�Ȃ�)�̎d�l�c�|�C���^�ϐ���*��t�������̂Ɠ����ł��B�ʖ���t����`�Ƃ������o��OK.

#include "Solver.h"
#include "Extern.h"

/*�Q�̑��p�`�̂���������Ԃ�*/
//�蓮
MoveNode Solver::get_hand_moving(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line)
{
	return MoveNode(src_turnflag, src_poly_id, src_line_id, dst_is_piece, dst_poly_id, dst_line_id, is_reverse_line);
}

//������
vector<MoveNode> Solver::get_semiAuto_moving(bool is_turn_ok, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line)
{
	int i, j, k;
	vector<MoveNode> ret;

	for (i = 0; i <= (is_turn_ok); i++) {
		for (j = 0; j < pieces.size(); j++) {
			for (k = 0; k < pieces[j].size(); k++) {
				ret.push_back(MoveNode(i, j, k, dst_is_piece, dst_poly_id, dst_line_id, is_reverse_line));
			}
		}
	}
	return ret;
}

//����
void Solver::set_auto_moving(bool is_turn_ok)
{
	int i, j, k;

	auto_moving.clear();

	for (i = 0; i < wakus.size(); i++) {
		for (j = 0; j < wakus[i].size(); j++) {
			for (k = 0; k <= 0; k++) {
				vector<MoveNode> res = get_semiAuto_moving(is_turn_ok, false, i, j, k);
				for (int l = 0; l < res.size(); l++) { auto_moving.push_back(res[l]); }
			}
		} 
	}

	for (i = 0; i < pieces.size(); i++) {
		for (j = i + 1; j < pieces[i].size(); j++) {
			vector<MoveNode> res = get_semiAuto_moving(is_turn_ok, true, i, j, k);
			for (int l = 0; l < res.size(); l++) { auto_moving.push_back(res[l]); }
		}
	}
	return;
}


//�s�[�Xsrc_poly_id�𓮂��� (��src���dst�ɂ�������C���[�W)
//reverse�cdst�ӂ��Ђ�����Ԃ����ǂ����H
void Solver::move(MoveNode & move_node)
{
	Poly &src = pieces[move_node.src_poly_id];
	Poly &dst = (move_node.dst_is_piece ? pieces[move_node.dst_poly_id] : wakus[move_node.dst_poly_id]);

	backup_piece = src;
	moved_piece_id = move_node.src_poly_id;

	if (src.is_turn != move_node.src_turnflag) { src.turn(); }

	if (move_node.is_reverse_line) {
		src.move_reverse(move_node.src_line_id, dst.points[move_node.dst_line_id], dst.points[move_node.dst_line_id + 1]);	//�I�_�����킹��
	}
	else {
		src.move(move_node.src_line_id, dst.points[move_node.dst_line_id + 1], dst.points[move_node.dst_line_id]);
	}
}

//�s�[�X�����̈ʒu�ɖ߂�
void Solver::restore()
{
	pieces[moved_piece_id] = backup_piece;
}


double Solver::evaluation_line(Poly &poly1, Poly &poly2) {
	SolverParameter &param = parameter; 
	
	double score = 0;

	vector<double> angle_error_poly1(poly1.size());
	vector<double> angle_error_poly2(poly2.size());
	double dist_error = param.dist_error();

	for (int i = 0; i < poly1.size(); i++) {
		angle_error_poly1[i] = param.angle_error(abs(poly1.points[i + 1] - poly1.points[i]));
	}
	for (int i = 0; i < poly2.size(); i++) {
		angle_error_poly2[i] = param.angle_error(abs(poly2.points[i + 1] - poly2.points[i]));
	}

	//�ӂ̕]��
	for (int i = 0; i < poly1.size(); i++) {
		Line line1 = Line(poly1.points[i], poly1.points[i + 1]);
		for (int j = 0; j < poly2.size(); j++) {
			Line line2 = Line(poly2.points[j], poly2.points[j + 1]);
			double angle_error = angle_error_poly1[i] + angle_error_poly2[j];
			angle_error /= 1.41421356;

			double res = line1.match_score(line2, dist_error, angle_error);
			if (res == 4) { score += 4; }
			if (res == 2) { score += 2; }	//180��
			if (res == 1) { score += 1; }
		}
	}
	return score;
}

//�p�x�̕]��
double Solver::evaluation_angle(Poly & poly1, Poly & poly2)
{
	SolverParameter &param = parameter;

	vector<double> angle_error_poly1(poly1.size());
	vector<double> angle_error_poly2(poly2.size());
	double dist_error = param.dist_error();

	for (int i = 0; i < poly1.size(); i++) {
		angle_error_poly1[i] = param.angle_error(abs(poly1.points[i + 1] - poly1.points[i]));
	}
	for (int i = 0; i < poly2.size(); i++) {
		angle_error_poly2[i] = param.angle_error(abs(poly2.points[i + 1] - poly2.points[i]));
	}

	double score = 0;

	//�p�̕]���i360��)
	for (int i = 0; i < poly1.size(); i++) {
		for (int j = 0; j < poly2.size(); j++) {
			//�ppoly1.points[i], �ppoly2.points[j]����v���Ă���΃X�R�A���Z
			if (norm(poly1.points[i] - poly2.points[j]) > dist_error * dist_error) continue;
		
			int i_1 = i - 1; if (i_1 < 0) i_1 += poly1.size();
			int j_1 = j - 1; if (j_1 < 0) j_1 += poly2.size();
			
			double angle_error = angle_error_poly1[i_1]
				+ angle_error_poly1[i]
			+ angle_error_poly2[j_1]
			+ angle_error_poly2[j];
			angle_error /= 2;

			if (angle_error > 10) { continue; }		//10�x������Ă���M�p�Ȃ�Ȃ��I

			double angle = poly1.angle(i) + poly2.angle(j);
			double error = abs(360 - angle);
			if (error <= angle_error) {
				score += 3;
				//score -= exp(-abs(abs(poly1.angle(i) - eval_obj.get_most_exist_angle())) / 5.0) * 1.5;		//�ŕp�l�̓X�R�A����
			}
		}
	}

	return score;
}


//�]���֐� (�ڐA���������j
double Solver::evaluation(Poly & poly1, Poly & poly2, bool is_poly2_piece)
{
	SolverParameter &param = parameter;

	//�����蔻��
	{
		Poly &src = poly1;
		Poly &dst = poly2;

		double dist_error = (is_poly2_piece) ? parameter.dist_error() : parameter.dist_error_waku();
		Poly scaledown_dst = dst.scaledown_poly( dist_error );
		if (src.is_hit(scaledown_dst) || src.is_cover(scaledown_dst)) return false;
		if (is_poly2_piece && scaledown_dst.is_cover(src)) return false;
	}
	return evaluation_angle(poly1, poly2) + evaluation_line(poly1, poly2);
}


//�u�ړ����]�����߂��v�̈�A���������֐�
double Solver::action_score(MoveNode & move_node)
{
	/*Point a, b, c, d;
	if (move_node.dst_is_piece) {
		a = pieces[move_node.dst_poly_id].points[move_node.dst_line_id];
		b = pieces[move_node.dst_poly_id].points[move_node.dst_line_id + 1];
	}
	else {
		a = wakus[move_node.dst_poly_id].points[move_node.dst_line_id];
		b = wakus[move_node.dst_poly_id].points[move_node.dst_line_id + 1];
	}
	c = pieces[move_node.src_poly_id].points[move_node.src_line_id];
	d = pieces[move_node.src_poly_id].points[move_node.src_line_id + 1];

	if (abs(abs(a - b) - abs(c - d)) > 20) { return -114514; }*/

	move(move_node);

	double score;
	if (move_node.dst_is_piece) {
		score = evaluation(pieces[move_node.src_poly_id], pieces[move_node.dst_poly_id], true);
	}
	else {
		score = evaluation(pieces[move_node.src_poly_id], wakus[move_node.dst_poly_id], false);
	}
	restore();
	return score;
}


//2�̑��p�`�����������邩�������肷��
bool Solver::can_marge(int src_poly_id, int dst_poly_id, bool dst_is_piece)
{
	if (!is_do_marge) return false;

	Poly &src = pieces[src_poly_id];
	Poly &dst = (dst_is_piece ? pieces[dst_poly_id] : wakus[dst_poly_id]);

	double dist_error = (dst_is_piece) ? parameter.dist_error() : parameter.dist_error_waku();
	Poly scaledown_dst = dst.scaledown_poly( dist_error );
	if (src.is_hit(scaledown_dst) || src.is_cover(scaledown_dst)) return false;
	if (dst_is_piece && scaledown_dst.is_cover(src)) return false;
	if (!marge_poly_obj.can_marge(dist_error, src, dst, dst_is_piece, wakus)) return false;

	pair<bool, vector<Poly>> res = marge_poly_obj.marge_poly(dist_error, src, dst, dst_is_piece);
	if (res.first == false) return false;

	double max_area = 0;
	Poly poly = res.second[0];	//�}�[�W��̑��p�`
	for (int i = 0; i < res.second.size(); i++) {
		if (max_area < abs(res.second[i].area())) {
			max_area = abs(res.second[i].area());
			poly = res.second[i];
		}
	}
	
	//(�}�[�W�㑽�p�`��)�ő�p + (���̃s�[�X��)�ŏ��p > 360���Ȃ�}����, �������A�ő�p > 350���Ȃ�}���肵�Ȃ��B
	for (int i = 0; i < poly.size(); i++) {
		double angle = poly.angle(i);
		if (angle > 350) { continue; }
		if (angle + eval_obj.get_min_angle() > 360) { return false; }
	}

	return true;
}


//2�̑��p�` (dst_is_piece == true�̏ꍇ�́A�s�[�X[src_poly_id]�ƃs�[�X[dst_poly_id]�j����������
//��������true��Ԃ�, ���s����false��Ԃ�
bool Solver::marge_poly(int src_poly_id, int dst_poly_id, bool dst_is_piece)
{
	//���p�`�̃}�[�W
	if (!is_do_marge) return false;
	
	Poly &src = pieces[src_poly_id];
	Poly &dst = (dst_is_piece ? pieces[dst_poly_id] : wakus[dst_poly_id]);
	double dist_error = (dst_is_piece) ? parameter.dist_error() : parameter.dist_error_waku();

	pair<bool, vector<Poly>> res = marge_poly_obj.marge_poly(dist_error, src, dst, dst_is_piece);
	if (res.first == false) return false;

	vector<Poly> marged_polys = res.second;
	if (dst_is_piece) {
		for (int i = 0; i < marged_polys.size(); i++) pieces.push_back(marged_polys[i]);
		pieces[src_poly_id].points.clear();
		pieces[dst_poly_id].points.clear();
	}
	else {
		for (int i = 0; i < marged_polys.size(); i++) wakus.push_back(marged_polys[i]);
		pieces[src_poly_id].points.clear();
		wakus[dst_poly_id].points.clear(); wakus[dst_poly_id].lines.clear();
	}
	return true;
}


//�����ł�������
vector<MoveNode> Solver::connect_auto(bool is_turn_ok, int max_moveList_size)
{
	int i, j, k;	//���p�`�ԍ�, �Ӕԍ�, �ӂ̎n�_���m���������邩�I�_���m���������邩�iis_reverse_line�j
	
	typedef tuple<bool, bool, int, int, bool> DstType;	//is_turn_ok, is_piece, poly_id, line_id, is_reverse_line
	map<DstType, vector<MoveNode> > scores;				//�e�ӂɂ���, (���̕ӂɂ�����src�ӂ�score)�̑g������
	map<DstType, vector<MoveNode> >::iterator it;

	for (i = 0; i < wakus.size(); i++) {
		for (j = 0; j < wakus[i].size(); j++) {
			vector<MoveNode> res = connect_semiAuto(is_turn_ok, false, i, j, false, max_moveList_size);
			DstType type = DstType(is_turn_ok, false, i, j, false);
			for (int m = 0; m < res.size(); m++) { scores[type].push_back(res[m]); }

			res = connect_semiAuto(is_turn_ok, false, i, j, true, max_moveList_size);
			type = DstType(is_turn_ok, false, i, j, true);
			for (int m = 0; m < res.size(); m++) { scores[type].push_back(res[m]); }
		}
	}

	for (i = 0; i < pieces.size(); i++) {
		for (j = 0; j < pieces[i].size(); j++) {
			vector<MoveNode> res = connect_semiAuto(is_turn_ok, true, i, j, false, max_moveList_size);
			DstType type = DstType(is_turn_ok, true, i, j, false);
			for (int m = 0; m < res.size(); m++) { scores[type].push_back(res[m]); }

			res = connect_semiAuto(is_turn_ok, true, i, j, true, max_moveList_size);
			type = DstType(is_turn_ok, true, i, j, true);
			for (int m = 0; m < res.size(); m++) { scores[type].push_back(res[m]); }
		}
	}

	//�I��
	double max_score = -114514;
	for (it = scores.begin(); it != scores.end(); it++) {
		vector<MoveNode> &table = (*it).second;

		for (i = 0; i < table.size(); i++) {
			max_score = max(max_score, table[i].score);
		}
	}

	//�u�S�]���l�̍ő�l�v���������������������. ���̒��Łu���j�[�N�Ȃ��́v��]������
	typedef pair<int, DstType> P;	//max_score�ɂȂ��Ă���̂̌�, dst��
	vector<P> dst_list;

	for (it = scores.begin(); it != scores.end(); it++) {
		vector<MoveNode> &table = (*it).second;

		int max_score_cnt = 0;

		for (i = 0; i < table.size(); i++) {
			if (table[i].score < max_score) {
				continue;
			}
			max_score_cnt++;
		}
		if (max_score_cnt == 0) { continue; }

		dst_list.push_back(P(max_score_cnt, (*it).first));
	}

	if (dst_list.size() == 0) { return vector<MoveNode>(); }
	sort(dst_list.begin(), dst_list.end());

	//printfDx("�ō��]�� = %d [�ʂ�]\n", dst_list.size());

	DstType best_dst = dst_list[0].second;	//���̕ӂɂ������ӂ�T���Ƃ悢
	vector<MoveNode> table = (*scores.find(best_dst)).second;
	sort(table.begin(), table.end());		//score���Ƀ\�[�g

	vector<MoveNode> res;
	for (i = 0; i < min(table.size(), max_moveList_size); i++) {
		res.push_back(table[i]);
	}
	return res;
}


//�������ł������� (�ӂƕӂ̂��������́u�]���l�v���Ԃ��j
vector<MoveNode> Solver::connect_semiAuto(bool is_turn_ok, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line, int max_moveList_size)
{
	vector<MoveNode> res = get_semiAuto_moving(is_turn_ok, dst_is_piece, dst_poly_id, dst_line_id, is_reverse_line);
	vector<pair<double, MoveNode> > move_list;

	eval_obj.make_histgram(wakus, pieces);

	for (int i = 0; i < res.size(); i++) {
		MoveNode move_node = res[i];
		double score = action_score(move_node);
		if (score > 0) {
			move_list.push_back(pair<double, MoveNode>(score, move_node));
		}
	}
	sort(move_list.begin(), move_list.end(), greater<pair<double, MoveNode>>());

	//�}�[�W���� (����p�Ȃ�) �����Ă݂āA���v�Ȃ��̂���ʂ���(���Xmax_moveList_size��)���
	vector<MoveNode> ret;
	for (int i = 0; i < move_list.size(); i++) {
		if (ret.size() >= max_moveList_size) break;

		MoveNode move_node = move_list[i].second;
		move(move_node);
		if (can_marge(move_node.src_poly_id, move_node.dst_poly_id, move_node.dst_is_piece)) {
			ret.push_back(move_node);
			ret.rbegin()->score = move_list[i].first;		//�]���l�̏��������
		}
		restore();
	}
	
	return ret;
}


//�蓮�ł�������, ���������Ȃ��Ƃ���false��Ԃ����A�s�[�X�͈ړ����Ă���
bool Solver::connect_hand(MoveNode move_node)
{
	move(move_node);

	//Poly src = pieces[move_node.src_poly_id];
	//Poly dst = (move_node.dst_is_piece) ? pieces[move_node.dst_poly_id] : wakus[move_node.dst_poly_id];

	//printfDx("score = %.1f\n", evaluation(src, dst, move_node.dst_is_piece));

	if (!marge_poly(move_node.src_poly_id, move_node.dst_poly_id, move_node.dst_is_piece)) { return false; }
	return true;
}
