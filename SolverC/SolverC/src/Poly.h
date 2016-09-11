//���p�`�N���X�̐錾

#pragma once
#include "Point.h"
#include "Line.h"

class Poly {
private:
	Point point0;				//���_0�̍��W�ipoint0.real(), point0.imag()�j
	vector<Point> point;		//���_0�ɑ΂��钸�_0, 1, 2, �c, n - 1�̍��W (�g�̏ꍇ�͎��v����, �s�[�X�̏ꍇ�͔����v����j
	bool is_piece_;				//�s�[�X�Ȃ�true, �g�Ȃ�false
	int turn_cnt;				//turn()�����s������

	void update();				//�O�ڒ����`�̏����X�V

public:
	bool is_putted;			//�����蔻��ɎQ������Ȃ�true�i�u���ꏊ���m�肵���Ȃ�true�j
	double lx, ly, rx, ry;	//Poly�̊O�ڒ����`. �����蔻�荂�����p. (�I�u�W�F�N�g�̊O���珑�����܂Ȃ����Ɓj

	Poly(Point point0, vector<Point> point, bool is_piece_);
	Poly();

	int size();						//���_��
	Point get_point(int id);		//�_�̐�Έʒu
	double get_angle_deg(int id);	//�p�x (�g�Ȃ�O�p���o��j

	void move(int id, Point s, Point e);				//�ړ�
	void move2(Point mul, Point center, Point trans);	//��]�x�N�g��, ��]�̒��S, ���s�ړ��ʂ��w�肵�Ĉړ��B�񓚐������̂ݎg�p�B
	void turn();										//���]
	void turn2(double center_imag);						//���] (�����w��). �񓚐������̂ݎg�p�B

	bool is_hit(Poly &poly);			//poly�Ɠ������Ă��邩�H
	bool is_contain(Poly &poly);		//poly���܂�ł��邩�H
	bool is_contained(Poly &poly);		//poly�Ɋ܂܂�Ă��邩�H

	double area();			//���p�`Poly�̕����t���ʐ� (���c�����v�܂��, ���c���v���j
	bool is_turned();		//turn_cnt����Ȃ�true��Ԃ�

	void point_reverse();	//���_��̌����������I�ɕύX����
	bool is_piece();		//�s�[�X���ǂ����𔻒�

	Line get_line(int id);
};
