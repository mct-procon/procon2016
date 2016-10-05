#include "DxLib.h"
#include "UserQuery.h"
#include "Solver.h"
#include "Extern.h"

UserQuery user_query;
Solver solver;

//�I���Ȃ�false��Ԃ�
bool game_loop() {
	backup.push(State(pieces, wakus));
	if (user_query.set_query() != 0) return false;
	solver.move(false, user_query.src_poly_id, user_query.src_line_id, user_query.dst_is_piece, user_query.dst_poly_id, user_query.dst_line_id, user_query.is_reverse_line);
	return ProcessMessage() == 0;
}

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 666, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	solver.input("NewProblem/sample_scanner.txt", 7, 4.5);//�����`�󉻂ɂ���Ē��_���W��x�s�N�Z���قǂ����Ȃ�A��2������x�ɂ��Ă��������B�ix<=5�ɂ������j
	solver.�]���Ȓ��_����菜��(10);							//�����l��傫������قǁA���_����菜����₷���Ȃ�܂��B
	solver.���p�`���g��(0, 0);

	// while(����ʂ�\��ʂɔ��f,���b�Z�[�W����,��ʃN���A, ESC�ŏI��)
	init_draw_option();
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		if (game_loop() == false) break;
	}
	DxLib_End();
	return 0;
}