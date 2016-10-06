#include "DxLib.h"
#include "Solver.h"
#include "WinAPIExt.h"

Solver solver;

vector<Solver> results;

void capture(Solver &shot) {
	results.push_back(shot);
}

void draw(Point center, double scale) {
	static int id = 0;
	static int frame = 0;
	static int speed = 1;

	if (CheckHitKey(KEY_INPUT_A)) speed = -1;
	if (CheckHitKey(KEY_INPUT_S)) speed = 0;
	if (CheckHitKey(KEY_INPUT_D)) speed = 1;
	if (CheckHitKey(KEY_INPUT_F)) speed = 3;
	frame += speed;
	id = (frame / 60) % (results.size() + 4);
	if (id >= results.size()) id = results.size() - 1;

	if (results.size() > 0) results[id].draw(center, scale, 800, 666);
	else solver.draw(center, scale, 800, 666);
}

void game_loop() {
	static Point center = Point(0, 0);	//���S�̐�΍��W
	static double scale = 1;			//�g�嗦
	
	//�\���𓮂���
	if (CheckHitKey(KEY_INPUT_UP)) { center += Point(0, -3) / scale; }
	if (CheckHitKey(KEY_INPUT_DOWN)) { center += Point(0, 3) / scale; }
	if (CheckHitKey(KEY_INPUT_LEFT)) { center += Point(-3, 0) / scale; }
	if (CheckHitKey(KEY_INPUT_RIGHT)) { center += Point(3, 0) / scale; }
	if (CheckHitKey(KEY_INPUT_Z)) { scale -= 0.007; }
	if (CheckHitKey(KEY_INPUT_X)) { scale += 0.007; }

	//�\���𓮂����i�}�E�X���� : �h���b�N�ɂ�镽�s�ړ�, ���{�^���̉�]�ɂ��g��k���j
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

	//�\������
	draw(center, scale);
}

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 800, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	//solver.input("\\NewProblem\\sample.txt", 5);                 //�����`�󉻂ɂ���Ē��_���W��x�s�N�Z���قǂ����Ȃ�A��2������x�ɂ��Ă��������B�ix<=5�ɂ������j
	HWND hwnd = GetMainWindowHandle();
	while(true){
		tstring filestr = WinAPIExt::ShowFileOpenDialog(hwnd);
		if (filestr == "\0") {
			DxLib_End();
			return 0;
		}
		if(solver.inputTstr(filestr, 5)) break;
		else MessageBox(hwnd, TEXT("�t�@�C���̓ǂݍ��݃G���[���������܂���"), TEXT("ERROR"), MB_OK);
	}
	solver.�]���Ȓ��_����菜��(10);
	solver.solve();

	// while(����ʂ�\��ʂɔ��f,���b�Z�[�W����,��ʃN���A, ESC�ŏI��)
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		game_loop();
	}
	DxLib_End();
	return 0;
}