//��{�@�\

#pragma once
#include "Line.h"
#include "Poly.h"
#include "DxLib.h"

class SolverBase {
protected:
	double dist;			//�����蔻��̊ɂ�
	vector<Poly> pieces;	//�����v���̒��_��
	vector<Poly> wakus;		//���v���̒��_��

	Poly input_poly(ifstream &ifs, bool is_clockwise);
	Poly scaledown_poly(Poly &poly);		//�����蔻��p�̗֊s�̎擾

public:
	void input(string filename, double dist);											//����
	void �]���Ȓ��_����菜��(double dist_error);
	void draw(Point center, double scale, double windowSizeX, double windowSizeY);		//�\��
};
