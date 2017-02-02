#include "DxLib.h"
#include "UserQuery.h"
#include "Solver.h"
#include "Extern.h"
#include "Windows.h"
#include "Show.h"

UserQuery user_query;
Show show;
vector<double> score_array;	//�]���lArray

//�I���Ȃ�false��Ԃ�
bool game_loop() {
	solver.print("answer.txt");

	backup.push(State(pieces, wakus));
	int status = user_query.set_query();

	if (status == 0) return false;	//�v���Z�X���b�Z�[�W�ُ�
	if (status == KEY_INPUT_ESCAPE) return false;	//�G�X�P�[�v

	//dst�ӂ�������Ύ���
	if (user_query.is_exist_dst == false) {
		vector<MoveNode> how_to_move = solver.connect_auto(false, 10);

		if (how_to_move.size() > 0) {
			//if (show.select_result(&solver, &how_to_move) != 0) return false;
			solver.connect_hand(how_to_move[0]);

			score_array.push_back(how_to_move[0].score);
		}
	}
	//����ȊO�ŁAsrc�ӂ�������Δ�����
	else if (user_query.is_exist_src == false) {
		vector<MoveNode> how_to_move = solver.connect_semiAuto(false, user_query.dst_is_piece, user_query.dst_poly_id, user_query.dst_line_id, user_query.is_reverse_line, 5);

		if (how_to_move.size() > 0) {
			//if (show.select_result(&solver, &how_to_move) != 0) return false;
			solver.connect_hand(how_to_move[0]);
		}
	}
	//����ȊO�Ȃ�A�蓮
	else {
		solver.connect_hand(MoveNode(false, user_query.src_poly_id, user_query.src_line_id, user_query.dst_is_piece, user_query.dst_poly_id, user_query.dst_line_id, user_query.is_reverse_line));
	}
	return ProcessMessage() == 0;
}

string toString(int num) {
	string str;
	while (num > 0) { str += (num % 10) + '0'; num /= 10; }
	string ret;
	for (int i = (int)str.length() - 1; i >= 0; i--) ret += str[i];
	return ret;
}

void save_score_array(string fname) {
	ofstream ofs(fname, ios::app);
	
	ofs << read_file_name << endl;
	for (int i = 0; i < score_array.size(); i++) {
		ofs << score_array[i];
		if (i + 1 < score_array.size()) ofs << " ";
	}
	ofs << endl;

	bool flag = false;
	for (int i = 0; i < pieces.size(); i++) {
		if (pieces[i].size() > 0) { flag = true; break; }
	}
	ofs << ((flag) ? "�s�����~" : "�����Z") << endl;
	ofs << endl;

	ofs.close();
	score_array.clear();
}

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR arg,int) {
	SetGraphMode(900, 666, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	vector<string> fnameList;

	for (int i = 1; i < 10; i++) fnameList.push_back("NewProblem/TR_prob0" + toString(i) + ".txt");
	for (int i = 10; i < 26; i++) fnameList.push_back("NewProblem/TR_prob" + toString(i) + ".txt");
	fnameList.push_back("Problem/Research/LuckyPuzzle.txt");
	fnameList.push_back("Problem/Research/Seisyonagon.txt");
	fnameList.push_back("Problem/Research/Tangram.txt");
	fnameList.push_back("Problem/Research/ManyParallelogram.txt");
	fnameList.push_back("Problem/Research/ManyRect.txt");
	fnameList.push_back("Problem/Research/ManySquare.txt");

	for (int i = 0; i < fnameList.size(); i++) {
		read_file_name = fnameList[i];
		solver.input(read_file_name);

		solver.�]���ȑ��p�`����菜��(100);
		solver.�]���Ȓ��_����菜��(5);							//�����l��傫������قǁA���_����菜����₷���Ȃ�܂��B

		// while(����ʂ�\��ʂɔ��f,���b�Z�[�W����,��ʃN���A, ESC�ŏI��)
		init_draw_option();
		while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0) {
			if (game_loop() == false) break;
		}
		if (ProcessMessage() != 0) { break; }

		//�]���lArray��ۑ�
		save_score_array("score_arrays.txt");

		//���f�[�^�̏���(Solver�N���X�̏������͕s�v)
		init_wakus.clear();
		out_pieces.clear();
		backup.clear();
		wakus.clear();
		pieces.clear();
	}

	DxLib_End();
	return 0;
}