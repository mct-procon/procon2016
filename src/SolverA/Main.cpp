//7月13日に実装開始したソルバーです。枠の外側から「頂点」に注目してピースをくっつけていきます。

#include "DxLib.h"
#include "Solver.h"

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 600, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	//解く
	static Solver solver;
	solver.input("Problem/TR1.txt");
	solver.solve();
	solver.print("Answer/TR1.txt");

	// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ESCで終了)
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		solver.draw(true);
	}
	DxLib_End();
	return 0;
}