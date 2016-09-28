//��{�@�\

#pragma once
#include "Line.h"
#include "Poly.h"
#include "DxLib.h"

class SolverBase {
protected:
	double dist;			//�����蔻��̊ɂ�
	double angle_error_deg;	//�p�x�̌덷�͈͂� (2�`3cm�̕ӂɈ͂܂ꂽ���_�ɂ�����) �}angle_error_deg[��]���炢�B�p�x�̕]���ɗp����B

	Poly input_poly(ifstream &ifs, bool is_clockwise);
	Poly scaledown_poly(Poly &poly, double dist);		//�����蔻��p�̗֊s�̎擾

public:
	void input(string filename, double dist, double angle_error_deg);									//����
	void �]���Ȓ��_����菜��(double dist_error);
};
