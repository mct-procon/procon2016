//���[�U�̓��͂��󂯕t���āA�\���o�[�Ɍv�Z���������������߂�B
//�ϐ���read_only

#include "UserQuery.h"
#include "Extern.h"

//�}�E�X�ƍł��߂��ӂ�Ԃ� (�`����W�ŁAdist��艓�����(false, -1, -1)��Ԃ�)
tuple<bool, int, int> UserQuery::get_nearest_line(int mouseX, int mouseY, double dist) {
	int i, j;

	Point mouse_point = Point((double)mouseX, (double)mouseY);
	double min_dist = dist;
	tuple<bool, int, int> ret = tuple<bool, int, int>(false, -1, -1);

	for (i = 0; i < wakus.size(); i++) {
		if (wakus[i].size() <= 0) continue;		//�]���r�͖���
		for (j = 0; j < wakus[i].size(); j++) {
			Line line = Line(to_draw_point( wakus[i].points[j] ), to_draw_point( wakus[i].points[j + 1] ));	//�`����W�Řgi�̕�j���擾
			if (line.dist(mouse_point) < min_dist) {
				min_dist = line.dist(mouse_point);
				ret = tuple<bool, int, int>(false, i, j);
			}
		}
	}

	for (i = 0; i < pieces.size(); i++) {
		if (pieces[i].size() <= 0) continue;	//�]���r�͖���
		for (j = 0; j < pieces[i].size(); j++) {
			Line line = Line(to_draw_point( pieces[i].points[j] ), to_draw_point( pieces[i].points[j + 1] ));
			if (line.dist(mouse_point) < min_dist) {
				min_dist = line.dist(mouse_point);
				ret = tuple<bool, int, int>(true, i, j);
			}
		}
	}
	return ret;
}


//�}�E�X�ƍł��߂����_��Ԃ�
tuple<bool, int, int> UserQuery::get_nearest_point(int mouseX, int mouseY, double dist) {
	int i, j;

	Point mouse_point = Point((double)mouseX, (double)mouseY);
	double min_dist = dist;
	tuple<bool, int, int> ret = tuple<bool, int, int>(false, -1, -1);

	for (i = 0; i < wakus.size(); i++) {
		if (wakus[i].size() <= 0) continue;		//�]���r�͖���
		for (j = 0; j < wakus[i].size(); j++) {
			Point point = to_draw_point( wakus[i].points[j] ); //�`����W�Řgi�̒��_j���擾
			if (abs(point - mouse_point) < min_dist) {
				min_dist = abs(point - mouse_point);
				ret = tuple<bool, int, int>(false, i, j);
			}
		}
	}

	for (i = 0; i < pieces.size(); i++) {
		if (pieces[i].size() <= 0) continue;	//�]���r�͖���
		for (j = 0; j < pieces[i].size(); j++) {
			Point point = to_draw_point( pieces[i].points[j] );	//�`����W�Ńs�[�Xi�̒��_j���擾
			if (abs(point - mouse_point) < min_dist) {
				min_dist = abs(point - mouse_point);
				ret = tuple<bool, int, int>(true, i, j);
			}
		}
	}
	return ret;
}


//�E�N���b�N�̃C�x���g�F���p�`�̔��], (���_�ɋ߂���Β��_�̍폜�j
void UserQuery::right_click(int mouseX, int mouseY) {
	//���_�̍폜
	tuple<bool, int, int> point = get_nearest_point(mouseX, mouseY, 8);
	bool is_piece = get<0>(point);
	int poly_id = get<1>(point);
	int point_id = get<2>(point);
	
	if (poly_id >= 0) {
		//���_�̓\�蒼�������܂�
		Poly &poly = (is_piece) ? pieces[poly_id] : wakus[poly_id];
		vector<Point> points;
		vector<Line> lines;
		for (int i = 0; i < poly.size(); i++) {
			if (i == point_id) continue;
			points.push_back(poly.points[i]);
		}
		points.push_back(points[0]);
		lines = poly.lines;
		
		//�\�蒼���ɂ�(����������)������t���悤 -> �폜�\��̒��_����ł��߂��ӂ܂ł̋��� (�v�Z���W)
		double min_dist = 114514;
		for (int i = 0; i < (int)points.size() - 1; i++) {
			Line line = Line(points[i], points[i + 1]);
			min_dist = min(min_dist, line.dist(poly.points[point_id]));
		}
		if (min_dist <= 20) {
			poly = Poly(points, lines);
		}
		return;
	}

	//���]
	tuple<bool, int, int> line = get_nearest_line(mouseX, mouseY, 1000);
	if (get<0>(line)) {	//�s�[�X�̏ꍇ
		pieces[get<1>(line)].turn();
	}
}


//���N���b�N�̃C�x���g�F�o����(src), ������(dst)�̕ӂ̑I��
void UserQuery::left_click(int mouseX, int mouseY) {
	tuple<bool, int, int> res = get_nearest_line(mouseX, mouseY, 40);
	bool is_piece = get<0>(res);
	int poly_id = get<1>(res);
	int line_id = get<2>(res);

	if (poly_id < 0) return;

	//���ɓo�^���Ă���ӂ�I�������ꍇ
	if (is_exist_src && is_piece && src_poly_id == poly_id && src_line_id == line_id) {
		if (!is_reverse_line) {
			is_reverse_line = true;
		}
		else {
			is_exist_src = false;
		}
		return;
	}
	if (is_exist_dst && dst_is_piece == is_piece && dst_poly_id == poly_id && dst_line_id == line_id) {
		if (!is_reverse_line) {
			is_reverse_line = true;
		}
		else {
			is_exist_dst = false;
		}
		return;
	}

	//�V�K
	if (!is_exist_dst) {	//dstination�̓o�^
		dst_is_piece = is_piece;
		dst_poly_id = poly_id;
		dst_line_id = line_id;
		is_exist_dst = true;
		is_reverse_line = false;
	}
	else if (!is_exist_src) {		//source�̓o�^
		if (!is_piece) {
			//src���g, dst���s�[�X���Ă���������ˁB
			if (is_exist_dst && dst_is_piece) {
				//�V�K�̂�dst�ɂ��āAdst�ɂȂ��Ă���̂�src�ɂ��邩�`
				src_poly_id = dst_poly_id;
				src_line_id = dst_line_id;
				is_exist_src = true;
				dst_is_piece = false;
				dst_poly_id = poly_id;
				dst_line_id = line_id;
				is_exist_dst = true;
				is_reverse_line = false;
			}
			return;
		}
		src_poly_id = poly_id;
		src_line_id = line_id;
		is_exist_src = true;
		is_reverse_line = false;
	}
}


//���N���b�N�̃C�x���g�F�s�[�X�̃h���b�O
void UserQuery::middle_push(int mouseX, int mouseY) {
	complex<double> trans = complex<double>(mouseX - (double)bmouseX, mouseY - (double)bmouseY);	//�`����W�ɂ����镽�s�ړ���
	if (abs(trans) <= 2) { return; }

	tuple<bool, int, int> res = get_nearest_line(bmouseX, bmouseY, 60);
	bool is_piece = get<0>(res);
	int poly_id = get<1>(res);
	int line_id = get<2>(res);

	if (poly_id < 0 || !is_piece) return;

	//�s�[�Xpoly_id���h���b�O����
	trans /= scale;
	for (int i = 0; i < pieces[poly_id].points.size(); i++) pieces[poly_id].points[i] += trans;
	for (int i = 0; i < pieces[poly_id].lines.size(); i++) { pieces[poly_id].lines[i].s += trans; pieces[poly_id].lines[i].e += trans; }
	pieces[poly_id] = Poly(pieces[poly_id].points, pieces[poly_id].lines);
}


//���̃��Z�b�g
void UserQuery::init() {
	is_exist_src = false;
	is_exist_dst = false;
	is_reverse_line = false;
	prev_mouseInput = 0;
	now_mouseInput = 0;
}


//���̕`��
void UserQuery::DrawArrow(int x1, int y1, int x2, int y2, int color, int thickness) {
	double size = 15;	//���̐���ۂ̑傫��

	//�v�Z
	Point a = Point((double)x2 - x1, (double)y2 - y1);	a /= abs(a);		//�����x�N�g��
	Point b = Point((double)y1 - y2, (double)x2 - x1);	b /= abs(b);		//�@���x�N�g��
	Point C = Point((double)x2, (double)y2) - a * (sqrt(3.0) / 2 * size);	//��_
	Point tri1 = C + b * (-size / 2);
	Point tri2 = C + b * (size / 2);

	//�`��
	DrawLine(x1, y1, x2, y2, color, thickness);
	DrawTriangle((int)tri1.real(), (int)tri1.imag(), (int)tri2.real(), (int)tri2.imag(), x2, y2, color, TRUE);
}


//�v���Z�X���b�Z�[�W��Ԃ����C��
int UserQuery::set_query() {
	init();
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0) {
		memcpy(bkey, key, 256);
		GetHitKeyStateAll(key);
		if (!bkey[KEY_INPUT_RETURN] && key[KEY_INPUT_RETURN] && is_exist_src && is_exist_dst) break;	//�G���^�[�ŏI��

		prev_mouseInput = now_mouseInput;
		now_mouseInput = GetMouseInput();
		int mouseX, mouseY;
		GetMousePoint(&mouseX, &mouseY);

		if (!(prev_mouseInput & MOUSE_INPUT_RIGHT) && (now_mouseInput & MOUSE_INPUT_RIGHT)) right_click(mouseX, mouseY);
		if (!(prev_mouseInput & MOUSE_INPUT_LEFT) && (now_mouseInput & MOUSE_INPUT_LEFT)) left_click(mouseX, mouseY);
		if (now_mouseInput & MOUSE_INPUT_MIDDLE) middle_push(mouseX, mouseY);

		//BackSpace�L�[�������ƁA�肪�߂�
		if (!bkey[KEY_INPUT_BACK] && key[KEY_INPUT_BACK]) {
			State res = backup.pop();
			pieces = res.pieces;
			wakus = res.wakus;
		}

		//�X�y�[�X�L�[�Ń}�[�W�����̎��s���𑀍�
		if (!bkey[KEY_INPUT_SPACE] && key[KEY_INPUT_SPACE]) {
			is_do_marge = !is_do_marge;
		}
		DrawFormatString(100, 150, 0, "�}�[�W�����F%s", (is_do_marge) ? "����" : "����");

		//�`��
		update_draw_option();
		draw();
		if (is_exist_src) {
			Line line = Line(pieces[src_poly_id].points[src_line_id], pieces[src_poly_id].points[src_line_id + 1]);
			line.s = to_draw_point(line.s);
			line.e = to_draw_point(line.e);
			DrawFormatString((int)(line.s.real() + line.e.real()) / 2, (int)(line.s.imag() + line.e.imag()) / 2, GetColor(0, 255, 255), "src");
			if (is_reverse_line) {	//�n�_ = e
				DrawArrow((int)line.e.real(), (int)line.e.imag(), (int)line.s.real(), (int)line.s.imag(), GetColor(0, 255, 255), 4);	//�ړ����͐��F��
			}
			else {	//�n�_ = s
				DrawArrow((int)line.s.real(), (int)line.s.imag(), (int)line.e.real(), (int)line.e.imag(), GetColor(0, 255, 255), 4);	//�ړ����͐��F��
			}
		}
		if (is_exist_dst) {
			Line line;
			if (dst_is_piece) line = Line(pieces[dst_poly_id].points[dst_line_id], pieces[dst_poly_id].points[dst_line_id + 1]);
			else line = Line(wakus[dst_poly_id].points[dst_line_id], wakus[dst_poly_id].points[dst_line_id + 1]);
			line.s = to_draw_point(line.s);
			line.e = to_draw_point(line.e);
			DrawFormatString((int)(line.s.real() + line.e.real()) / 2, (int)(line.s.imag() + line.e.imag()) / 2, GetColor(255, 100, 0), "dst");
			if (is_reverse_line) {	//�n�_ = s
				DrawArrow((int)line.s.real(), (int)line.s.imag(), (int)line.e.real(), (int)line.e.imag(), GetColor(255, 100, 0), 4);	//�ړ���̓I�����W��
			}
			else {	//�n�_ = e
				DrawArrow((int)line.e.real(), (int)line.e.imag(), (int)line.s.real(), (int)line.s.imag(), GetColor(255, 100, 0), 4);	//�ړ���̓I�����W��
			}
		}
		//DrawFormatString(100, 100, 0, "pieceNum = %d\n", pieces.size());

		//�}�E�X���W�̋L�^
		bmouseX = mouseX;
		bmouseY = mouseY;
	}
	return ProcessMessage();
}
