
//ユーザの入力を受け付けて、ソルバーに計算させたい情報を決める。
//変数はread_only

#pragma once
#include "DxLib.h"
#include "stdafx.h"
#include "Poly.h"

class UserQuery {
private:
	//マウスクリック状態
	int prev_mouseInput;
	int now_mouseInput;
	
	//キーボードクリック状態（エンター押すまで続ける）
	char bkey[256];
	char key[256];
	int bmouseX, bmouseY;

	//マウスと最も近い辺を返す (distより遠ければ(false, -1, -1)を返す)
	tuple<bool, int, int> get_nearest_line(int mouseX, int mouseY, double dist);

	//マウスと最も近い頂点を返す
	tuple<bool, int, int> get_nearest_point(int mouseX, int mouseY, double dist);

	//右クリックのイベント：多角形の反転, (頂点に近ければ頂点の削除）
	void right_click(int mouseX, int mouseY);

	//左クリックのイベント：出発辺(src), 到着辺(dst)の辺の選択
	void left_click(int mouseX, int mouseY);

	//中押しのイベント：ピースのドラッグ
	int drug_piece_id;
	void middle_push(int mouseX, int mouseY);

	//初期化
	void init();

	//矢印の描画
	void DrawArrow(int x1, int y1, int x2, int y2, int color, int thickness);

public:
	//選択状態
	bool is_exist_src;
	bool is_exist_dst;

	//多角形のくっつけ方を記憶, ソルバーへ渡す
	int src_poly_id; int src_line_id;
	bool dst_is_piece; int dst_poly_id; int dst_line_id;
	bool is_reverse_line;		//辺をひっくり返すか？

	int set_query();	//プロセスメッセージを返す
};