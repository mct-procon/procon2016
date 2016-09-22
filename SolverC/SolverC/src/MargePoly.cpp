//�O���t����Ċ撣��
#include "MargePoly.h"
#include "DxLib.h"

void MargePoly::make_all_pl(Poly &src_poly, Poly &dst_poly) {
	all_p.clear();
	all_l.clear();
	for (int i = 0; i < src_poly.size(); i++) {
		all_p.push_back(src_poly.points[i]);
		all_l.push_back(Line(src_poly.points[i], src_poly.points[i + 1]));
	}
	for (int i = 0; i < dst_poly.size(); i++) {
		all_p.push_back(dst_poly.points[i]);
		all_l.push_back(Line(dst_poly.points[i], dst_poly.points[i + 1]));
	}
}


//�S���_�ɃO���[�v�ԍ������A�e�O���[�v�̏ꏊ�i����\�_�̍��W�j���L�^����
void MargePoly::make_group() {
	int i, j, gid = 0;

	points.clear();
	for (i = 0; i < all_p.size(); i++) group_id[i] = -1;
	for (i = 0; i < all_p.size(); i++) {
		if (group_id[i] >= 0) continue;
		
		group_id[i] = gid;
		for (j = i + 1; j < all_p.size(); j++) {
			if (group_id[j] >= 0) continue;
			if (abs(all_p[i] - all_p[j]) <= dist_error) {
				group_id[j] = gid;
			}
		}
		points.push_back(all_p[i]);		//points[gid] = all_p[i]
		gid++;
	}
}


//�e�����ɂ��āA���̐����Ƃ̋�����dist_error�ȉ��̒��_�i�O���[�v����̂��́j��S�Č��o���A
//�i���_���W, �O���[�v�ԍ�)�̃y�A��[x���W -> y���W]�Ń\�[�g����B
//���̌�A�O�����×~�ɕӃ��X�g ( = �אڍs�� )�ɓ���Ă����B
void MargePoly::make_graph() {
	int i, j, k;

	for (i = 0; i < points.size(); i++) for (j = 0; j < points.size(); j++) table[i][j] = false;

	for (i = 0; i < all_l.size(); i++) {
		//�_���X�g����
		vector<pair<Point, int> > p_list;
		for (j = 0; j < points.size(); j++) {	//�O���[�vj�̑�\�_�i��\�_j�j
			if (all_l[i].dist(points[j]) <= dist_error) {
				//��\�_j�͕�all_l[i]��ɏ���Ă���I
				p_list.push_back(pair<Point, int>(points[j], j));
			}
		}
		//�o�u���\�[�g
		for (j = 0; j < (int)p_list.size() - 1; j++) {
			for (k = (int)p_list.size() - 1; k > j; k--) {
				Point lp = p_list[k-1].first;
				Point rp = p_list[k].first;
				if (lp.real() > rp.real() || (lp.real() == rp.real() && lp.imag() > rp.imag())) {
					swap(p_list[k-1], p_list[k]);
				}
			}
		}
		//�Ӓǉ�
		for (j = 0; j < (int)p_list.size() - 1; j++) {
			int u = p_list[j].second;
			int v = p_list[j+1].second;
			table[u][v] = true;
			table[v][u] = true;
		}
	}
}


////�g�命�p�`��Ԃ��B
//dist > 0�̂Ƃ��c�s�[�X�̏ꍇ�͓����蔻��g���O���ɂł���B�g�̏ꍇ�͓����蔻��g�������ɂł���B
Poly MargePoly::scaleup_poly(Poly &poly) {
	int i;
	int n = poly.size();
	vector<Line> lines(n);
	vector<Point> points(n + 1);

	//���p�`�����ɐ���������
	for (i = 0; i < n; i++) {
		lines[i] = Line(poly.points[i], poly.points[i + 1]);
		lines[i].move(dist_error);
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


//�}�[�W��������Ȃ��_�i��\�_�j��id����Ԃ�
int MargePoly::get_startPoint_id(Poly &src_poly, Poly &dst_poly, bool dst_is_piece) {
	if (dst_is_piece) {
		for (int i = 0; i < points.size(); i++) { if (abs(points[i].imag() - min(src_poly.ly, dst_poly.ly)) <= dist_error) return i; }
	}
	else {
		//�g�命�p�`�𓾂�
		Poly poly = scaleup_poly(src_poly);
		//�g�命�p�`poly�̓����ɂȂ����_��I��
		for (int i = 0; i < points.size(); i++) {
			if (!poly.is_cover_point(points[i])) return i;
		}
	}
	return -1;
}


//���p�`�̐����i������, ���̒i�K�ł͏璷�ȕӂ��܂�)
//�}�[�W��̑��p�`�Ɋ܂܂��_��1�I��(points[start_id]��I��)�����̓_�͑S��������3�_�ڈȍ~���×~�ɂƂ遨���[�v���ł���܂ŌJ��Ԃ��B
//�ʐϔ�r�ɂ���Č������݂��Ȃ������肵����A���_��̌��������������𔻒肷��BOK�Ȃ�}�[�W�I���B
vector<Point> MargePoly::get_cycle(int start_id, bool is_clockwise, double area) {
	vector<bool> visited;
	int i, j;
	
	visited.resize(points.size());

	for (i = 0; i < points.size(); i++) {
		if (!table[start_id][i]) continue;

		//�_start_id -> i�ƖK��Ă݂�
		Point prev = points[start_id];
		Point now = points[i];
		int now_id = i;

		vector<Point> cycle;
		for (j = 0; j < points.size(); j++) visited[j] = false;

		cycle.push_back(points[start_id]); visited[start_id] = true;
		cycle.push_back(points[i]);
			
		while (!visited[now_id]) {
			//�t���O�𗧂Ă�
			visited[now_id] = true;
			//���̏ꏊ�����߂�
			double tmp_ang = 114514;		//���v���œ��������Ȃ�min��, �����v���ŊO�������Ȃ�min�� -> min���I�I
			int next_id = -1;
			for (j = 0; j < points.size(); j++) {
				if (!table[now_id][j] || points[j] == prev) continue;

				Point vec = (points[j] - now) / (prev - now);
				double ang = atan2(vec.imag(), vec.real());
				if (ang < 0) ang += 2 * 3.14159265358979;
				if (tmp_ang > ang) { tmp_ang = ang; next_id = j; }
			}
			if (next_id == -1) { vector<Point> null_points; printfDx("�}�[�W�����G���[�F�T�C�N����������܂���\n"); return null_points; }
			//�ړ�����
			prev = now;
			now = points[next_id];
			now_id = next_id;
			cycle.push_back(now);
		}

		if (now_id != start_id) { continue; }	//�n�_�ɖ߂�Ȃ������̂ŕs��
		//�ʐόv�Z
		Poly poly; poly.points = cycle;
		if (!is_clockwise) { if (poly.area() < 0) continue; }	//�}�[�W����ƃs�[�X�ɂȂ�̂ɁA���v���Ȃ̂�NG
		else { if (poly.area() > 0) continue; }					//�}�[�W����Ƙg�ɂȂ�̂ɁA�����v���Ȃ̂�NG
		if (abs((abs(poly.area()) - area)) > dist_error * 1000) { continue; }	//�ʐς�����Ȃ��̂�NG
		//�}�[�W�\�I�I
		return cycle;
	}
	//�}�[�W�s�i�}�[�W����Ƌ�̈�ɂȂ��Ă��܂��j
	vector<Point> null_points;
	return null_points;
}


//�}�[�W�\�Ȃ�true��Ԃ�, �s�\�Ȃ�false��Ԃ�
//dist_error, angle_error_deg�́Amarge_poly�ɓn���l�ł͂Ȃ��A�]���֐�evaluation�ŗp�����Ă���l�ɂ���B
bool MargePoly::can_marge(double dist_error, double angle_error_deg, Poly &src_poly, Poly &dst_poly, bool dst_is_piece) {
	vector<int> ids;

	for (int i = 0; i < src_poly.size(); i++) {
		for (int j = 0; j < dst_poly.size(); j++) {
			Line line1 = Line(src_poly.points[i], src_poly.points[i + 1]);
			Line line2 = Line(dst_poly.points[j], dst_poly.points[j + 1]);
			double score = line1.machi_score(line2, dist_error, angle_error_deg);
			if (score == 1 || score == 4) {	//�����������ꍇ
				ids.push_back(i);
				break;
			}
		}
	}
	sort(ids.begin(), ids.end());
	for (int i = 0; i < (int)ids.size(); i++) {	//ids[i]����X�^�[�g
		bool flag = true;
		for (int j = 0; j < (int)ids.size() - 1; j++) {	//ids[i+j]��ids[i+j+1]�̔�r
			int u = (i + j) % ids.size();
			int v = (i + j + 1) % ids.size();
			u = (u < i) * src_poly.size() + ids[u];
			v = (v < i) * src_poly.size() + ids[v];
			if (v - u >= 2) {
				flag = false;
				break;
			}
		}
		if (flag) return true;
	}
	return false;
}


//���p�`�̃}�[�W
//dist_error�c���钸�_i���狗��dist_error�ȉ��ɂ��钸�_���O���[�v������Ȃǁc�B�]���֐��ŗp���Ă���dist_error����1.5�{���炢�傫�������悢�B
//angle_error�c�璷�ȓ_����菜���ۂɗp����B�傫������ƒ��_���������A����������Ə璷�ȓ_���c��B�]���֐��ŗp���Ă���angle_error_deg�Ɠ������炢�ɂ���Ɩ���B
Poly MargePoly::marge_poly(double dist_error, double angle_error_deg, Poly &src_poly, Poly &dst_poly, bool dst_is_piece) {
	Poly null_poly;

	this->dist_error = dist_error;
	make_all_pl(src_poly, dst_poly);
	make_group();
	make_graph(); 
	
	int start_id = get_startPoint_id(src_poly, dst_poly, dst_is_piece);
	if (start_id < 0) { return null_poly; }			//�J�n�_�����݂��Ȃ� (��F�g�ɍŌ�̃s�[�X�𖄂߂钼�O�Ƃ��j
	double area = (dst_is_piece ? abs(src_poly.area()) + abs(dst_poly.area()) : abs(dst_poly.area()) - abs(src_poly.area()));	
	vector<Point> cycle = get_cycle(start_id, !dst_is_piece, area);
	if (cycle.size() == 0) { return null_poly; }	//�}�[�W���ďI��

	//�璷�ȓ_����菜��
	vector<Point> new_cycle;
	new_cycle.push_back(cycle[0]);
	for (int i = 0; i < (int)cycle.size() - 2; i++) {
		Point vec = (cycle[i + 1] - cycle[i]) / (cycle[i + 2] - cycle[i + 1]);
		vec /= abs(vec);
		if (abs(vec.imag()) > sin(angle_error_deg * 3.1415926 / 180.0)) {	//�_j + 1�ŋȂ����Ă���
			new_cycle.push_back(cycle[i + 1]);
		}
	}
	new_cycle.push_back(new_cycle[0]);

	//�}�[�W�������p�`��Ԃ�
	vector<Line> lines;
	for (int i = 0; i < src_poly.lines.size(); i++) lines.push_back(src_poly.lines[i]);
	for (int i = 0; i < dst_poly.lines.size(); i++) lines.push_back(dst_poly.lines[i]);
	return Poly(new_cycle, lines);
}
