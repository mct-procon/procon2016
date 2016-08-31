#include "DxLib.h"
#include "Solver.h"
#include "InputUI.h"

InputUI input;
Solver solver;

void game_loop() {

	input.update();

	//static int drug_state = 0;
	//Point start;

	//グループの移動
	/*if (input.isDrugStart()) {
		drug_state = 1;
		start = input.getMousePoint();
	}
	if (drug_state && input.isDrugEnd()) {
		drug_state = 0;
		Point goal = input.getMousePoint();
		solver.drug(start, goal);
	}*/

	if (input.isClick()) {
		/*辺選択*/
		solver.select_line(input.getMousePoint());
	}
	if (input.isPush(KEY_INPUT_RETURN)) {
		/*マッチング計算→辺選択のクリア*/
		solver.solve();
		solver.release_line();;
	}
	/*表示*/
	solver.draw();
}

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 600, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	solver.input("Problem/sample.txt", 5);
	solver.prepare();

	// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ESCで終了)
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		game_loop();
	}
	DxLib_End();
	return 0;
}