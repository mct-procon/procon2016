//��{�@�\

#pragma once
#include "Line.h"
#include "Poly.h"
#include "DxLib.h"
#include "SolverParameter.h"

class SolverBase {
protected:
	Poly input_poly(ifstream &ifs, bool is_clockwise);
	void print_poly(ofstream &ofs, Poly poly);

public:
	SolverParameter parameter;							//�����蔻��, �]���֐�, �}�[�W�����ŗp����p�����[�^���܂Ƃ߂�����
	void input(string filename);					//����, �덷�p�����[�^�̒������s��
	void �]���ȑ��p�`����菜��(double min_area);	//�����ȑ��p�`����菜��
	void �]���Ȓ��_����菜��(double dist_error);	//input����ɂ̂݌Ă�ł悢
	void print(string filename);
};
