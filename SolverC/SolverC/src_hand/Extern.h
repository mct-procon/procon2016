//�S�͈͂œǂݏ����ł���ϐ�

#pragma once
#include "Poly.h"
#include "STL.h"
#include "Backup.h"

extern Backup backup;		//�o�b�N�A�b�v���(Main)��[���ɖ߂����H|�߂�, �������Ȃ�](UserQuery)���s�[�X�𓮂���(Solver), �̏���
extern vector<Poly> pieces;	//Solver, UserQuery�ŏ�������
extern vector<Poly> wakus;	//Solver�ŏ�������

extern Point center;		//Solver, UserQuery�ŏ�������
extern double scale;		//Solver, UserQuery�ŏ�������
extern double windowSizeX;
extern double windowSizeY;

void init_draw_option();
void update_draw_option();
Point to_draw_point(Point point);
void draw();