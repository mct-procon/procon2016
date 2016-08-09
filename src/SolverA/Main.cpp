//7��13���Ɏ����J�n�����\���o�[�ł��B�g�̊O������u���_�v�ɒ��ڂ��ăs�[�X���������Ă����܂��B

#include "DxLib.h"
#include "MySolver.h"

vector<MySolver> solver_anime;

void capture(MySolver backup) {
	solver_anime.push_back(backup);
}

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 600, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	//����
	static MySolver solver;
	solver.input("Problem/TR3.txt");
	solver.solve();
	solver.print("Answer/TR3.txt");

	// while(����ʂ�\��ʂɔ��f,���b�Z�[�W����,��ʃN���A, ESC�ŏI��)
	int t = 0;
	int play_mode = 1;	//3�c�����Đ�, 1�c�Đ�, 0�c�X�g�b�v
	bool is_draw_didnot_put = false;

	char key[256] = {0};
	char bkey[256];

	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		//�p�Y���̕\��
		int id = (t / 60) % (solver_anime.size() + 4);
		if (id >= solver_anime.size()) { id = (int)solver_anime.size() - 1; }
		solver_anime[id].draw(is_draw_didnot_put);
		//�L�[�{�[�h�̍X�V
		for (int i = 0; i < 256; i++) bkey[i] = key[i];
		GetHitKeyStateAll(key);
		//��[�ǂ݁[
		DrawFormatString(150, 25, GetColor(255, 65, 154), "�Đ����[�h(A, S, D, F)�`(�t�Đ�, ��~, �ʏ�, ����)");
		DrawFormatString(150, 55, GetColor(0, 155, 155), "�\���ؑւ�(SPACE)");
		//�Đ��Ƃ��\���̐؂�ւ�
		if (key[KEY_INPUT_A]) { play_mode = -1; }
		if (key[KEY_INPUT_S]) { play_mode = 0; }
		if (key[KEY_INPUT_D]) { play_mode = 1; }
		if (key[KEY_INPUT_F]) { play_mode = 3; }
		if (!bkey[KEY_INPUT_SPACE] && key[KEY_INPUT_SPACE]) { is_draw_didnot_put = !is_draw_didnot_put; }
		
		t += play_mode;
	}
	DxLib_End();
	return 0;
}