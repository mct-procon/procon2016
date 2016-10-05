//�O���[�o���ϐ��̒�`
#include "Extern.h"
#include "DxLib.h"


Backup backup;			//�o�b�N�A�b�v���(Main)��[���ɖ߂����H|�߂�, �������Ȃ�](UserQuery)���s�[�X�𓮂���(Solver), �̏���
vector<Poly> wakus;
vector<Poly> pieces;

bool isDrawInfo;	//�ڍ׏��̕\���E��\��
Point center;		//�`��̒��S���W
double scale;		//�`��̔{��
double windowSizeX;
double windowSizeY;

bool is_do_marge = true;	//false�Ȃ�A�����I�Ƀ}�[�W���������ۂ���

//�`����̏�����
void init_draw_option() {
	isDrawInfo = true;
	center = Point(0, 0);
	scale = 1.0;
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
	if (CheckHitKey(KEY_INPUT_Z)) { scale -= 0.007; }
	if (CheckHitKey(KEY_INPUT_X)) { scale += 0.007; }

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
		scale += rot * 0.028;
	}
}


//�`����W�ɕϊ�
Point to_draw_point(Point point) {
	point -= center;
	point *= scale;
	point += Point(windowSizeX / 2, windowSizeY / 2);
	return point;
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
	if (isDrawInfo) {
		for (i = 0; i < wakus.size(); i++) {
			if (wakus[i].size() <= 0) continue;
			s = Point(0, 0); for (j = 0; j < wakus[i].size(); j++) s += wakus[i].points[j]; s /= wakus[i].size();
			//�ϊ�
			s = to_draw_point( s );
			//�\��
			DrawFormatString((int)s.real(), (int)s.imag(), GetColor(255, 0, 0), "%d", i);
		}

		for (i = 0; i < pieces.size(); i++) {
			if (pieces[i].size() <= 0) continue;
			s = Point(0, 0); for (j = 0; j < pieces[i].size(); j++) s += pieces[i].points[j]; s /= pieces[i].size();
			//�ϊ�
			s = to_draw_point( s );
			//�\��
			DrawFormatString((int)s.real(), (int)s.imag(), GetColor(0, 0, 255), "%d", i);
		}
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

	//���_��
	/*for (i = 0; i < pieces.size(); i++) {
		if (pieces[i].size() <= 0) continue;
		s = Point(0, 0); for (j = 0; j < pieces[i].size(); j++) s += pieces[i].points[j]; s /= pieces[i].size();
		//�ϊ�
		s -= center; s *= scale; s += trans;
		s += 20;
		//�\��
		DrawFormatString((int)s.real(), (int)s.imag(), GetColor(0, 0, 0), "%d", pieces[i].size());
	}*/
}
