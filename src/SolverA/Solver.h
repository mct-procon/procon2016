//�񓚃v���O�����ŋ��ʂ��Ďg���f�[�^, �֐����W�߂܂����B
//�s�[�X�̈ړ����i�񓚁j�́Aisputted[], piece[]�����������邱�Ƃōs���܂��B

#pragma once
#include "Poly.h"
#include "Line.h"
#include "Vertex.h"

class Solver {
private:
	Poly get_scaledown_poly(Poly &poly, double dist);	//�upoly�̏k�����p�`�v�Ԃ�

protected:
	vector<Poly> waku;			//�g�̏W��
	vector<Poly> piece;			//�s�[�X�̏W��
	bool isputted[50];			//isputted[id] = piece[id]�̈ʒu���m�肵����true
	vector<Line> putted_line;	//�ʒu���m�肵���ӂ̏W��
	int timer;					//�v���p

	void move(Vertex src, Vertex dst);					//���_src�𒸓_dst�ɂ�������悤�ɁA�ړ�����
	bool isoverlap(Poly &poly, double dist);			//�����蔻��Bdist��傫������قǁu������Ȃ��Ȃ�v

public:
	virtual void solve() = 0;			//���������܂��B�isolve�̒��g��Solver�̎q�N���X�ŏ����܂��j
	void draw(bool isdraw_didnot_put);	//�񓚂�\�����܂��B
	Solver();							//�����������܂��B
	void input(string filename);		//�t�@�C��filename���������͂��܂��B
	void print(string filename);		//�t�@�C��filename�։񓚂��o�͂��܂��B
};