#include "DxLib.h"
#include "Solver.h"
#include "WinAPIExt.h"

Solver solver;

vector<Solver> results;

void capture(Solver &shot) {
	results.push_back(shot);
}

void draw(Point center, double scale) {
	static int id = 0;
	static int frame = 0;
	static int speed = 1;

	if (CheckHitKey(KEY_INPUT_A)) speed = -1;
	if (CheckHitKey(KEY_INPUT_S)) speed = 0;
	if (CheckHitKey(KEY_INPUT_D)) speed = 1;
	if (CheckHitKey(KEY_INPUT_F)) speed = 3;
	frame += speed;
	id = (frame / 60) % (results.size() + 4);
	if (id >= results.size()) id = results.size() - 1;

	if (results.size() > 0) results[id].draw(center, scale, 800, 666);
	else solver.draw(center, scale, 800, 666);
}

void game_loop() {
	static Point center = Point(0, 0);	//中心の絶対座標
	static double scale = 1;			//拡大率
	
	//表示を動かす
	if (CheckHitKey(KEY_INPUT_UP)) { center += Point(0, -3) / scale; }
	if (CheckHitKey(KEY_INPUT_DOWN)) { center += Point(0, 3) / scale; }
	if (CheckHitKey(KEY_INPUT_LEFT)) { center += Point(-3, 0) / scale; }
	if (CheckHitKey(KEY_INPUT_RIGHT)) { center += Point(3, 0) / scale; }
	if (CheckHitKey(KEY_INPUT_Z)) { scale -= 0.007; }
	if (CheckHitKey(KEY_INPUT_X)) { scale += 0.007; }

	//表示を動かす（マウス操作 : ドラックによる平行移動, 中ボタンの回転による拡大縮小）
	{
		static bool bclick, click = false;
		static int bmx, bmy;
		static int mx = 0, my = 0;
		bclick = click;
		click = (GetMouseInput() & MOUSE_INPUT_LEFT) > 0;
		bmx = mx; bmy = my;
		GetMousePoint(&mx, &my);
		if (bclick && click) {
			center -= Point(mx - bmx, my - bmy) / scale;
		}

		int rot = GetMouseWheelRotVol();	//マウスホイールの回転量（手前から奥に回すと負の値を返す）
		//奥から手前で拡大, 手前から奥で縮小
		scale += rot * 0.028;
	}

	//表示する
	draw(center, scale);
}

int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 800, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	//solver.input("\\NewProblem\\sample.txt", 5);                 //もし形状化によって頂点座標がxピクセルほどずれるなら、第2引数をxにしてください。（x<=5にしたい）
	HWND hwnd = GetMainWindowHandle();
	while(true){
		tstring filestr = WinAPIExt::ShowFileOpenDialog(hwnd);
		if (filestr == "\0") {
			DxLib_End();
			return 0;
		}
		if(solver.inputTstr(filestr, 5)) break;
		else MessageBox(hwnd, TEXT("ファイルの読み込みエラーが発生しました"), TEXT("ERROR"), MB_OK);
	}
	solver.余分な頂点を取り除く(10);
	solver.solve();

	// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ESCで終了)
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		game_loop();
	}
	DxLib_End();
	return 0;
}