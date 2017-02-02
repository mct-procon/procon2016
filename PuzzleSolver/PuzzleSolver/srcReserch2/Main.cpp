#include "DxLib.h"
#include "UserQuery.h"
#include "Solver.h"
#include "Extern.h"
#include "Windows.h"
#include "Show.h"

UserQuery user_query;
Show show;

//終了ならfalseを返す
bool game_loop() {
	solver.print("answer.txt");

	backup.push(State(pieces, wakus));
	int status = user_query.set_query();

	if (status == 0) return false;	//プロセスメッセージ異常
	if (status == KEY_INPUT_ESCAPE) return false;	//エスケープ

	//dst辺が無ければ自動
	if (user_query.is_exist_dst == false) {
		vector<MoveNode> how_to_move = solver.connect_auto(false, 10);
		
		if (how_to_move.size() > 0) {
			//if (show.select_result(&solver, &how_to_move) != 0) return false;
			solver.connect_hand(how_to_move[0]);
		}
	}
	//それ以外で、src辺が無ければ半自動
	else if (user_query.is_exist_src == false) {
		vector<MoveNode> how_to_move = solver.connect_semiAuto(false, user_query.dst_is_piece, user_query.dst_poly_id, user_query.dst_line_id, user_query.is_reverse_line, 5);

		if (how_to_move.size() > 0) {
			//if (show.select_result(&solver, &how_to_move) != 0) return false;
			solver.connect_hand(how_to_move[0]);
		}
	}
	//それ以外なら、手動
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

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR arg,int) {
	SetGraphMode(800, 666, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	vector<string> fnameList;
	for (int i = 1; i <= 6; i++) fnameList.push_back("Problem/Waku_Square/Triangle/Less5Piece/NoOption/case" + toString(i) + ".txt");
	for (int i = 1; i <= 2; i++) fnameList.push_back("Problem/Waku_Square/Triangle/Less10Piece/NoOption/case" + toString(i) + ".txt");

	for (int i = 0; i < fnameList.size(); i++) {
		read_file_name = fnameList[i];
		solver.input(read_file_name);

		solver.余分な多角形を取り除く(100);
		solver.余分な頂点を取り除く(10);							//引数値を大きくするほど、頂点が取り除かれやすくなります。

		// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ESCで終了)
		init_draw_option();
		while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0) {
			if (game_loop() == false) break;
		}
		if (ProcessMessage() != 0) { break; }

		//問題データの消去(Solverクラスの初期化は不要)
		init_wakus.clear();
		out_pieces.clear();
		backup.clear();
		wakus.clear();
		pieces.clear();
	}

	DxLib_End();
	return 0;
}