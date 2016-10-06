
//SolverBase�ɋ@�\��ǉ�����B

#pragma once
#include "SolverBase.h"
#include "MargePoly.h"

class Solver : public SolverBase {
private:
	//�ړ�
	int moved_piece_id;
	Poly backup_piece;
	void move(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id);	//�s�[�X���ړ� (�o�b�N�A�b�v�����)
	void restore();									//�s�[�X���ړ��O�̈ʒu�ɖ߂�

	//�]��
	double evaluation(Poly &poly1, Poly &poly2, bool is_poly2_piece);	//�]�� O(���_��1 * ���_��2), �萔�d��
	
	//�u�ړ����]�����߂��v�̈�A���������֐�
	double action_score(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id);

	//���p�`�̌���
	MargePoly marge_poly;

	//�x�X�g�ȑg�ł�������, �������g�ݍ��킹���Ȃ��Ƃ�0��Ԃ�
	bool connect();

public:
	void solve();
};