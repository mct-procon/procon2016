
//SolverBase�ɋ@�\��ǉ�����B

#pragma once
#include "SolverBase.h"
#include "MargePoly.h"

class Solver : public SolverBase {
private:
	//���p�`�̌���
	MargePoly marge_poly;

public:
	//�������Ă�������, ��������̂������Ȃ�false��Ԃ�
	bool move(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id);	//�s�[�X���ړ�
};