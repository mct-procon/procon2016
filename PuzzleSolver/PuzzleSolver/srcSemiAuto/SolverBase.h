//��{�@�\

#pragma once
#include "Line.h"
#include "Poly.h"
#include "DxLib.h"
#include "SolverParameter.h"

class SolverBase {
protected:
	Poly input_poly(ifstream &ifs, bool is_clockwise);
	Poly scaledown_poly(Poly &poly, double dist);		//�����蔻��p�̗֊s�̎擾
	SolverParameter parameter;							//�����蔻��, �]���֐�, �}�[�W�����ŗp����p�����[�^���܂Ƃ߂�����
	void print_poly(ofstream &ofs, Poly poly);

public:
	void input(string filename);				//����, �덷�p�����[�^�̒������s��
	void �]���Ȓ��_����菜��(double dist_error);
	void ���p�`���g��(double dist_piece, double dist_waku);
	void print(string filename);
};
