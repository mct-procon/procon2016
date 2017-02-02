#include "DxLib.h"
#include "UserQuery.h"
#include "Solver.h"
#include "Extern.h"
#include "Windows.h"
#include "Show.h"

UserQuery user_query;
Show show;
vector<double> score_array;	//評価値Array

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

			score_array.push_back(how_to_move[0].score);
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
	ofs << ((flag) ? "不正解×" : "正解〇") << endl;
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

	fnameList.push_back("NewProblem/sample/9piece_small.txt");
	fnameList.push_back("NewProblem/TR_prob22.txt");
	fnameList.push_back("Problem/Research/LuckyPuzzle.txt");
	fnameList.push_back("Problem/Research/Seisyonagon.txt");
	fnameList.push_back("Problem/Research/Tangram.txt");
	fnameList.push_back("Problem/Research/ManyParallelogram.txt");
	fnameList.push_back("Problem/Research/ManyRect.txt");
	fnameList.push_back("Problem/Research/ManySquare.txt");
	for (int i = 1; i < 10; i++) fnameList.push_back("NewProblem/TR_prob0" + toString(i) + ".txt");
	for (int i = 10; i < 26; i++) fnameList.push_back("NewProblem/TR_prob" + toString(i) + ".txt");

	for (int i = 0; i < fnameList.size(); i++) {
		read_file_name = fnameList[i];
		solver.input(read_file_name);

		solver.余分な多角形を取り除く(100);
		solver.余分な頂点を取り除く(5);							//引数値を大きくするほど、頂点が取り除かれやすくなります。

		// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ESCで終了)
		init_draw_option();
		while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0) {
			if (game_loop() == false) break;
		}
		if (ProcessMessage() != 0) { break; }

		//評価値Arrayを保存
		save_score_array("score_arrays.txt");

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