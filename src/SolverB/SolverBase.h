//��{�@�\

#pragma once
#include "Line.h"
#include "Poly.h"
#include "DxLib.h"

class SolverBase {
private:
	double dist;		//�����蔻��̊ɂ��idist���傫���قǒu���₷���Ȃ�. dist > 0�Ƃ��邱��. dist = 5�����傤�Ǘǂ�����. dist�l�̃Z�b�g���ł���̂�input()�֐��̂�.�j

	void draw_poly(Poly poly, int red, int green, int blue, double scale);
	Poly read_poly(ifstream &ifs, bool is_piece);
	void print_poly(ofstream &ofs, Poly poly);

	Poly get_scaledown_poly(Poly poly);
	vector<int> get_contain_piece_id(Poly waku);

protected:
	vector<Poly> pieces;	//�����v���̒��_��
	vector<Poly> wakus;		//���v���̒��_��
	vector<Poly> wakusIni;	//���v���̒��_��

	void move(Poly &src, int src_id, bool src_turnmode, Poly &dst, int dst_id, bool dst_turnmode);
	bool canput(Poly poly);		//�����蔻��Bdist=0�Ō���, dist���傫���قǒu���₷���Ȃ�
	void make_answer();

public:
	virtual void solve() = 0;
	void draw(bool is_draw_didnot_put);
	void input(string filename, double dist);
	void print(string filename);
};