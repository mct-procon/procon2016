//�O���[�o���ϐ��̒�`
#include "Extern.h"
#include "DxLib.h"


Backup backup;				//�o�b�N�A�b�v���(Main)��[���ɖ߂����H|�߂�, �������Ȃ�](UserQuery)���s�[�X�𓮂���(Solver), �̏���
vector<Poly> wakus;
vector<Poly> pieces;

vector<Poly> init_wakus;
vector<Poly> out_pieces;

Solver solver;		//�R���g���[���p

bool isDrawInfo;	//�ڍ׏��̕\���E��\��
Point center;		//�`��̒��S���W
double scale;		//�`��̔{��
double windowSizeX;
double windowSizeY;

bool is_do_marge = true;	//false�Ȃ�A�����I�Ƀ}�[�W���������ۂ���
string read_file_name;

//�`����̏�����
void init_draw_option() {
	isDrawInfo = true;
	center = Point(0, 0);
	scale = 10;
	windowSizeX = 800;
	windowSizeY = 666;
}


//�`����̕ύX
void update_draw_option() {
	//�\���𓮂��� (�\���L�[�ŕ��s�ړ�, Z��X�ŏk���E�g��j
	if (CheckHitKey(KEY_INPUT_UP)) { center += Point(0, -3) / scale; }
	if (CheckHitKey(KEY_INPUT_DOWN)) { center += Point(0, 3) / scale; }
	if (CheckHitKey(KEY_INPUT_LEFT)) { center += Point(-3, 0) / scale; }
	if (CheckHitKey(KEY_INPUT_RIGHT)) { center += Point(3, 0) / scale; }
	if (CheckHitKey(KEY_INPUT_Z)) { scale -= 0.014; }
	if (CheckHitKey(KEY_INPUT_X)) { scale += 0.014; }

	//�\���𓮂����i�}�E�X���� : �h���b�N�ŕ��s�ړ�, �}�E�X�z�C�[���Ŋg��k���j
	{
		static bool bclick, click = false;
		static int bmx, bmy;
		static int mx = 0, my = 0;
		bclick = click;
		click = (GetMouseInput() & MOUSE_INPUT_LEFT) > 0;
		bmx = mx; bmy = my;
		GetMousePoint(&mx, &my);
		if (bclick && click) {
			center -= Point(mx - bmx, my - bmy) / scale;
		}

		int rot = GetMouseWheelRotVol();	//�}�E�X�z�C�[���̉�]�ʁi��O���牜�ɉ񂷂ƕ��̒l��Ԃ��j
		//�������O�Ŋg��, ��O���牜�ŏk��
		scale += rot * 0.0056;
	}
}


//�`����W�ɕϊ�
Point to_draw_point(Point point) {
	point -= center;
	point *= scale;
	point += Point(windowSizeX / 2, windowSizeY / 2);
	return point;
}

//����
vector<double> get_line_length_list() {
	vector<double> lengths;
	int i, j;

	for (i = 0; i < wakus.size(); i++) {
		if (wakus[i].size() <= 0 && wakus[i].lines.size() <= 0) continue;
		for (j = 0; j < wakus[i].size(); j++) {
			Line line = Line(wakus[i].points[j + 1], wakus[i].points[j]);
			lengths.push_back(line.length());
		}
	}
	for (i = 0; i < pieces.size(); i++) {
		if (pieces[i].size() <= 0) continue;
		for (j = 0; j < pieces[i].size(); j++) {
			Line line = Line(pieces[i].points[j + 1], pieces[i].points[j]);
			lengths.push_back(line.length());
		}
	}
	sort(lengths.begin(), lengths.end());
	
	//�d���i�߂������j����菜��
	double range = solver.parameter.dist_error(), now = -114;
	vector<double> ret;
	for (i = 0; i < lengths.size(); i++) {
		if (now + range >= lengths[i]) { continue; }
		now = lengths[i];
		ret.push_back(now);
	}
	return ret;
}

//�`��
void draw() {
	int i, j;
	Point s, e;

	//�� (�]���r�͖���)
	for (i = 0; i < wakus.size(); i++) {
		if (wakus[i].size() <= 0 && wakus[i].lines.size() <= 0) continue;
		for (j = 0; j < wakus[i].lines.size(); j++) {
			//�ϊ�
			s = to_draw_point( wakus[i].lines[j].s );
			e = to_draw_point( wakus[i].lines[j].e );
			//�\��
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 255, 0), 2);
		}
	}
	for (i = 0; i < pieces.size(); i++) {
		if (pieces[i].size() <= 0) continue;
		for (j = 0; j < pieces[i].lines.size(); j++) {
			//�ϊ�
			s = to_draw_point( pieces[i].lines[j].s );
			e = to_draw_point( pieces[i].lines[j].e );
			//�\��
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 255, 0), 2);
		}
	}

	//���p�`�̔ԍ� (�]���r�͖���)
	//�^�O�ԍ�i�̃s�[�X
	for (i = 0; i < out_pieces.size(); i++) {
		if (out_pieces[i].size() <= 0) { continue; }		//�]���r

		double base_x = 0;
		double base_y = 0;
		for (int j = 0; j < out_pieces[i].size(); j++) {
			Point pos = to_draw_point(out_pieces[i].points[j]);
			base_x += pos.real();
			base_y += pos.imag();
		}
		base_x /= out_pieces[i].size();
		base_y /= out_pieces[i].size();

		DrawFormatString((int)base_x, (int)base_y, GetColor(0, 0, 255), "%d", i);
	}

	//���_��i��, �ԁj
	for (i = 0; i < wakus.size(); i++) {
		for (j = 0; j < wakus[i].size(); j++) {
			//�ϊ�
			s = to_draw_point( wakus[i].points[j] );
			e = to_draw_point( wakus[i].points[j + 1] );
			//�\��
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(255, 0, 255), 2);
			if (isDrawInfo) DrawFormatString((int)s.real(), (int)s.imag(), GetColor(255, 0, 255), "%d", j);
		}
	}

	for (i = 0; i < pieces.size(); i++) {
		for (j = 0; j < pieces[i].size(); j++) {
			//�ϊ�
			s = to_draw_point( pieces[i].points[j] );
			e = to_draw_point( pieces[i].points[j + 1] );
			//�\��
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(255, 0, 0), 2);
			if (isDrawInfo) DrawFormatString((int)s.real(), (int)s.imag(), GetColor(255, 0, 0), "%d", j);
		}
	}

	/*if (isDrawInfo) {
		//�ӂ̒����̏��������߂�(�����Ȍ덷�͓��ꎋ�j
		vector<double> lengths = get_line_length_list();

		//�ӂ̒����̏�����\��
		for (i = 0; i < wakus.size(); i++) {
			if (wakus[i].size() <= 0 && wakus[i].lines.size() <= 0) continue;
			for (j = 0; j < wakus[i].size(); j++) {
				Line line = Line(wakus[i].points[j + 1], wakus[i].points[j]);
				//�ϊ�
				s = to_draw_point(line.s);
				e = to_draw_point(line.e);
				int id = upper_bound(lengths.begin(), lengths.end(), line.length()) - lengths.begin(); id--;
				int lx = s.real() + e.real(); lx /= 2;
				int ly = s.imag() + e.imag(); ly /= 2;
				//�\��
				DrawBox(lx, ly, lx + 25, ly + 17, GetColor(255, 255, 0), FALSE);
				DrawFormatString(lx, ly, 0, "%d", id);
			}
		}
		for (i = 0; i < pieces.size(); i++) {
			if (pieces[i].size() <= 0) continue;
			for (j = 0; j < pieces[i].size(); j++) {
				Line line = Line(pieces[i].points[j + 1], pieces[i].points[j]);
				//�ϊ�
				s = to_draw_point(line.s);
				e = to_draw_point(line.e);
				int id = upper_bound(lengths.begin(), lengths.end(), line.length()) - lengths.begin(); id--;
				int lx = s.real() + e.real(); lx /= 2;
				int ly = s.imag() + e.imag(); ly /= 2;
				//�\��
				DrawBox(lx, ly, lx + 25, ly + 17, GetColor(255, 255, 0), FALSE);
				DrawFormatString(lx, ly, 0, "%d", id);
			}
		}

		int all_corn_num = 0;
		for (int i = 0; i < wakus.size(); i++) if (wakus[i].size() >= 0) all_corn_num += wakus[i].size();
		for (int i = 0; i < pieces.size(); i++) if (pieces[i].size() >= 0) all_corn_num += pieces[i].size();

		//�ӂ̒����̎�ސ� / �S���_�̌�
		//DrawFormatString(500, 500, 0, "�ӂ̒����̎�ސ� / �S���_�̌� = %f", lengths.size() / (double)all_corn_num);

		//�t�@�C����
		DrawFormatString(500, 100, 0, "%s", read_file_name.c_str());
	}*/
}
