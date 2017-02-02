//全範囲で読み書きできる変数

#pragma once
#include "Poly.h"
#include "stdafx.h"
#include "Backup.h"
#include "Solver.h"

extern Backup backup;		//バックアップ取る(Main)→[元に戻すか？|戻す, 何もしない](UserQuery)→ピースを動かす(Solver), の順番
extern vector<Poly> pieces;	//Solver, UserQueryで書き込み
extern vector<Poly> wakus;	//Solverで書き込み
extern vector<Poly> init_wakus;
extern vector<Poly> out_pieces;

extern Solver solver;

extern Point center;		//Solver, UserQueryで書き込み
extern double scale;		//Solver, UserQueryで書き込み
extern double windowSizeX;
extern double windowSizeY;
extern bool isDrawInfo;
extern bool is_do_marge;
extern string read_file_name;

void init_draw_option();
void update_draw_option();
Point to_draw_point(Point point);
void draw();