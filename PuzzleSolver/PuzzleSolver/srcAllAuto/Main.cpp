#include "DxLib.h"
#include "Solver.h"
#include "Extern.h"

Solver solver;
vector<vector<Poly> > results_pieces;
vector<vector<Poly> > results_wakus;

//今の盤面を記録する（全自動でしか使わないので、あまり重要ではない機能）
void capture() {
	results_pieces.push_back(pieces);
	results_wakus.push_back(wakus);
}

//終了ならfalseを返す, リザルトを表示
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

	//描画設定
	update_draw_option();
	if (results_pieces.size() > 0) {
		pieces = results_pieces[id];
		wakus = results_wakus[id];
	}
	//描画
	draw();
	return ProcessMessage() == 0;
}

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 666, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	solver.input("NewProblem/sample_scannerhand.txt", 7, 4);	//もし形状化によって頂点座標がxピクセルほどずれるなら、第2引数をxにしてください。（x<=5にしたい）
	solver.余分な頂点を取り除く(10);						//引数値を大きくするほど、頂点が取り除かれやすくなります。
	solver.多角形を拡大(0, 0);
	solver.solve();											//問題を解く

	// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ESCで終了)
	init_draw_option();
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		if (game_loop() == false) break;
	}
	DxLib_End();
	return 0;
}