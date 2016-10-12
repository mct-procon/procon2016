
//SolverBase�ɋ@�\��ǉ�����B

#pragma once
#include "SolverBase.h"
#include "MargePoly.h"
#include "SolverParameter.h"
#include "MoveNode.h"
#include "Evaluation.h"

class Solver : public SolverBase {
private:
	//�ړ��̎d����S�񋓂���֐�
	MoveNode get_hand_moving(bool src_turnflag, int src_poly_id, int src_line_id, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line);
	vector<MoveNode> get_semiAuto_moving(bool is_turn_ok, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line);
	vector<MoveNode> auto_moving; 
	void set_auto_moving(bool is_turn_ok);

	//�ړ�
	int moved_piece_id;
	Poly backup_piece;

	//�]��, �i�����蔻��̌v�Z�ʂ�, �O���������邱�Ƃ�, �s�[�X�̌��{���炢���Ƃ����Ƃ��ł��܂��j
	Evaluation eval_obj;								//conncet���Ăяo�����тɏ�����
	double evaluation_line(Poly &poly1, Poly &poly2);	//�ӂ̕]�� O(���_��1 * ���_��2), �萔�d��
	double evaluation_angle(Poly &poly1, Poly &poly2);	//���_�̕]��
	double evaluation(Poly &poly1, Poly &poly2, bool is_poly2_piece);

	//�u�ړ����]�����߂��v�̈�A���������֐�
	double action_score(MoveNode &move_node);

	//���p�`�̌���
	MargePoly marge_poly_obj;
	bool can_marge(int src_poly_id, int dst_poly_id, bool dst_is_piece);		//2�̑��p�`�����������邩�������肷��
	bool marge_poly(int src_poly_id, int dst_poly_id, bool dst_is_piece);		//2�̑��p�`���������遨��������true, ���s����false��Ԃ��B

public:
	//�����������u�悳���ȏ��ԂŁv���X�g�A�b�v����B���X�g�̍ő咷��max_moveList_size. ���]�����ŉ��������ꍇ�́Ais_turn_ok��false���w�肷�邱�ƁB
	vector<MoveNode> connect_auto(bool is_turn_ok, int max_moveList_size);
	vector<MoveNode> connect_semiAuto(bool is_turn_ok, bool dst_is_piece, int dst_poly_id, int dst_line_id, bool is_reverse_line, int max_moveList_size);

	//�蓮�ł�������B��������true, ���s����false��Ԃ��B
	bool connect_hand(MoveNode move_node);

	//���U���g�I��p�Ƃ���move, restore���ǂ���
	void move(MoveNode &move_node);	//�s�[�X���ړ�
	void restore();					//�s�[�X�����̈ʒu�ɖ߂�
};