#include "DxLib.h"
#include "Solver.h"
#include "Extern.h"

Solver solver;
vector<vector<Poly> > results_pieces;
vector<vector<Poly> > results_wakus;

//���̔Ֆʂ��L�^����i�S�����ł����g��Ȃ��̂ŁA���܂�d�v�ł͂Ȃ��@�\�j
void capture() {
	results_pieces.push_back(pieces);
	results_wakus.push_back(wakus);
}

//�I���Ȃ�false��Ԃ�, ���U���g��\��
bool game_loop() {
	static int frame = 0;
	static int speed = 1;

	if (CheckHitKey(KEY_INPUT_F)) speed = 3;
	if (CheckHitKey(KEY_INPUT_D)) speed = 1;
	if (CheckHitKey(KEY_INPUT_S)) speed = 0;
	if (CheckHitKey(KEY_INPUT_A)) speed = -1;
	frame += speed;

	int id = (frame / 60) % (results_pieces.size() + 4);
	if (id < 0) id = 0;
	if (id >= results_pieces.size()) id = results_pieces.size() - 1;

	//�`��ݒ�
	update_draw_option();
	if (results_pieces.size() > 0) {
		pieces = results_pieces[id];
		wakus = results_wakus[id];
	}
	//�`��
	draw();
	return ProcessMessage() == 0;
}

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 666, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	solver.input("NewProblem/sample_scannerhand.txt", 7, 4);	//�����`�󉻂ɂ���Ē��_���W��x�s�N�Z���قǂ����Ȃ�A��2������x�ɂ��Ă��������B�ix<=5�ɂ������j
	solver.�]���Ȓ��_����菜��(10);						//�����l��傫������قǁA���_����菜����₷���Ȃ�܂��B
	solver.���p�`���g��(0, 0);
	solver.solve();											//��������

	// while(����ʂ�\��ʂɔ��f,���b�Z�[�W����,��ʃN���A, ESC�ŏI��)
	init_draw_option();
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		if (game_loop() == false) break;
	}
	DxLib_End();
	return 0;
}