#include "DxLib.h"
#include "UserQuery.h"
#include "Solver.h"
#include "Extern.h"

UserQuery user_query;
Solver solver;

//終了ならfalseを返す
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

	solver.input("NewProblem/sample_scanner.txt", 7, 4.5);//もし形状化によって頂点座標がxピクセルほどずれるなら、第2引数をxにしてください。（x<=5にしたい）
	solver.余分な頂点を取り除く(10);							//引数値を大きくするほど、頂点が取り除かれやすくなります。
	solver.多角形を拡大(0, 0);

	// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ESCで終了)
	init_draw_option();
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		if (game_loop() == false) break;
	}
	DxLib_End();
	return 0;
}