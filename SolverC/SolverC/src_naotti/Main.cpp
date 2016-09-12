#include "DxLib.h"
#include "SolverBase.h"
#include "InputUI.h"

InputUI input;
SolverBase solver;

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

	//�\������
	solver.draw(center, scale, 800, 600);
}

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 600, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	solver.input("NewProblem/Trans1.txt", 5);

	// while(����ʂ�\��ʂɔ��f,���b�Z�[�W����,��ʃN���A, ESC�ŏI��)
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		game_loop();
	}
	DxLib_End();
	return 0;
}