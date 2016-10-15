//グローバル変数の定義
#include "Extern.h"
#include "DxLib.h"


Backup backup;				//バックアップ取る(Main)→[元に戻すか？|戻す, 何もしない](UserQuery)→ピースを動かす(Solver), の順番
vector<Poly> wakus;
vector<Poly> pieces;

vector<Poly> init_wakus;
vector<Poly> out_pieces;

Solver solver;		//コントロール用

bool isDrawInfo;	//詳細情報の表示・非表示
Point center;		//描画の中心座標
double scale;		//描画の倍率
double windowSizeX;
double windowSizeY;

bool is_do_marge = true;	//falseなら、強制的にマージ処理を拒否する


//描画情報の初期化
void init_draw_option() {
	isDrawInfo = true;
	center = Point(0, 0);
	scale = 0.2;
	windowSizeX = 800;
	windowSizeY = 666;
}


//描画情報の変更
void update_draw_option() {
	//表示を動かす (十字キーで平行移動, ZとXで縮小・拡大）
	if (CheckHitKey(KEY_INPUT_UP)) { center += Point(0, -3) / scale; }
	if (CheckHitKey(KEY_INPUT_DOWN)) { center += Point(0, 3) / scale; }
	if (CheckHitKey(KEY_INPUT_LEFT)) { center += Point(-3, 0) / scale; }
	if (CheckHitKey(KEY_INPUT_RIGHT)) { center += Point(3, 0) / scale; }
	if (CheckHitKey(KEY_INPUT_Z)) { scale -= 0.0014; }
	if (CheckHitKey(KEY_INPUT_X)) { scale += 0.0014; }

	//表示を動かす（マウス操作 : ドラックで平行移動, マウスホイールで拡大縮小）
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
		scale += rot * 0.0056;
	}
}


//描画座標に変換
Point to_draw_point(Point point) {
	point -= center;
	point *= scale;
	point += Point(windowSizeX / 2, windowSizeY / 2);
	return point;
}


//描画
void draw() {
	int i, j;
	Point s, e;

	//辺 (ゾンビは無視)
	for (i = 0; i < wakus.size(); i++) {
		if (wakus[i].size() <= 0 && wakus[i].lines.size() <= 0) continue;
		for (j = 0; j < wakus[i].lines.size(); j++) {
			//変換
			s = to_draw_point( wakus[i].lines[j].s );
			e = to_draw_point( wakus[i].lines[j].e );
			//表示
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 255, 0), 2);
		}
	}
	for (i = 0; i < pieces.size(); i++) {
		if (pieces[i].size() <= 0) continue;
		for (j = 0; j < pieces[i].lines.size(); j++) {
			//変換
			s = to_draw_point( pieces[i].lines[j].s );
			e = to_draw_point( pieces[i].lines[j].e );
			//表示
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 255, 0), 2);
		}
	}

	//多角形の番号 (ゾンビは無視)
	if (isDrawInfo) {
		//タグ番号iのピース
		for (i = 0; i < out_pieces.size(); i++) {
			if (out_pieces[i].size() <= 0) { continue; }		//ゾンビ

			double base_x = 0;
			double base_y = 0;
			for (int j = 0; j < out_pieces[i].size(); j++) {
				Point pos = to_draw_point(out_pieces[i].points[j]);
				base_x += pos.real();
				base_y += pos.imag();
			}
			base_x /= out_pieces[i].size();
			base_y /= out_pieces[i].size();
			DrawFormatString((int)base_x, (int)base_y, GetColor(0, 0, 255), "%d", i);
		}
	}

	//頂点列（紫, 赤）
	for (i = 0; i < wakus.size(); i++) {
		for (j = 0; j < wakus[i].size(); j++) {
			//変換
			s = to_draw_point( wakus[i].points[j] );
			e = to_draw_point( wakus[i].points[j + 1] );
			//表示
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(255, 0, 255), 2);
			if (isDrawInfo) DrawFormatString((int)s.real(), (int)s.imag(), GetColor(255, 0, 255), "%d", j);
		}
	}

	for (i = 0; i < pieces.size(); i++) {
		for (j = 0; j < pieces[i].size(); j++) {
			//変換
			s = to_draw_point( pieces[i].points[j] );
			e = to_draw_point( pieces[i].points[j + 1] );
			//表示
			DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(255, 0, 0), 2);
			if (isDrawInfo) DrawFormatString((int)s.real(), (int)s.imag(), GetColor(255, 0, 0), "%d", j);
		}
	}
}
