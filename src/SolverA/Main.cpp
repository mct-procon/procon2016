//7月13日に実装開始したソルバーです。枠の外側から「頂点」に注目してピースをくっつけていきます。

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

	//解く
	static MySolver solver;
	solver.input("Problem/TR3.txt");
	solver.solve();
	solver.print("Answer/TR3.txt");

	// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ESCで終了)
	int t = 0;
	int play_mode = 1;	//3…高速再生, 1…再生, 0…ストップ
	bool is_draw_didnot_put = false;

	char key[256] = {0};
	char bkey[256];

	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		//パズルの表示
		int id = (t / 60) % (solver_anime.size() + 4);
		if (id >= solver_anime.size()) { id = (int)solver_anime.size() - 1; }
		solver_anime[id].draw(is_draw_didnot_put);
		//キーボードの更新
		for (int i = 0; i < 256; i++) bkey[i] = key[i];
		GetHitKeyStateAll(key);
		//りーどみー
		DrawFormatString(150, 25, GetColor(255, 65, 154), "再生モード(A, S, D, F)～(逆再生, 停止, 通常, 高速)");
		DrawFormatString(150, 55, GetColor(0, 155, 155), "表示切替え(SPACE)");
		//再生とか表示の切り替え
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