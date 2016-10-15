#include "DxLib.h"
#include "UserQuery.h"
#include "Solver.h"
#include "Extern.h"
#include "Windows.h"
#include "Show.h"

UserQuery user_query;
Show show;

//�I���Ȃ�false��Ԃ�
bool game_loop() {
	solver.print("answer.txt");

	backup.push(State(pieces, wakus));
	if (user_query.set_query() != 0) return false;

	//dst�ӂ�������Ύ���
	if (user_query.is_exist_dst == false) {
		vector<MoveNode> how_to_move = solver.connect_auto(false, 10);
		
		if (how_to_move.size() > 0) {
			if (show.select_result(&solver, &how_to_move) != 0) return false;
			//solver.connect_hand(how_to_move[0]);
		}
	}
	//����ȊO�ŁAsrc�ӂ�������Δ�����
	else if (user_query.is_exist_src == false) {
		vector<MoveNode> how_to_move = solver.connect_semiAuto(false, user_query.dst_is_piece, user_query.dst_poly_id, user_query.dst_line_id, user_query.is_reverse_line, 5);

		if (how_to_move.size() > 0) {
			if (show.select_result(&solver, &how_to_move) != 0) return false;
			//solver.connect_hand(how_to_move[0]);
		}
	}
	//����ȊO�Ȃ�A�蓮
	else {
		solver.connect_hand(MoveNode(false, user_query.src_poly_id, user_query.src_line_id, user_query.dst_is_piece, user_query.dst_poly_id, user_query.dst_line_id, user_query.is_reverse_line));
	}
	return ProcessMessage() == 0;
}

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR arg,int) {
	SetGraphMode(800, 666, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	solver.input("NewProblem/sample_scannerhand.txt");
	//DxLib_End();
	//return 0;

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