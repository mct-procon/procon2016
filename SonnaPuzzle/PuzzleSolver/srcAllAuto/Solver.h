
//SolverBase�ɋ@�\��ǉ�����B

#pragma once
#include "SolverBase.h"
#include "MargePoly.h"

class Solver : public SolverBase {
private:
	//�ړ�
	int moved_piece_id;
	Poly backup_piece;
	void move(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line);	//�s�[�X���ړ�
	void restore();			//�s�[�X�����̈ʒu�ɖ߂�

	//�]��, �i�����蔻��̌v�Z�ʂ�, �O���������邱�Ƃ�, �s�[�X�̌��{���炢���Ƃ����Ƃ��ł��܂��j
	double evaluation(Poly &poly1, Poly &poly2, bool is_poly2_piece);	//�]�� O(���_��1 * ���_��2), �萔�d��

	//�u�ړ����]�����߂��v�̈�A���������֐�
	double action_score(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line);

	//���p�`�̌���
	MargePoly marge_poly_obj;
	bool marge_poly(int src_poly_id, int dst_poly_id, bool dst_is_piece);		//2�̑��p�`���������遨��������true, ���s����false��Ԃ��B

	//�x�X�g�ȑg�ł�������, �������g�ݍ��킹���Ȃ��Ƃ�0��Ԃ�
	bool connect();

public:
	//�O���[�o���ϐ���pieces, wakus��ω������Ȃ���A���������Ă����B
	void solve();
};