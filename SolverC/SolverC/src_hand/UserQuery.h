
//���[�U�̓��͂��󂯕t���āA�\���o�[�Ɍv�Z���������������߂�B
//�ϐ���read_only

#pragma once
#include "DxLib.h"
#include "STL.h"
#include "Poly.h"

class UserQuery {
private:
	//�I�����
	bool is_exist_src;
	bool is_exist_dst;
	
	//�}�E�X�N���b�N���
	int prev_mouseInput;
	int now_mouseInput;
	
	//�L�[�{�[�h�N���b�N��ԁi�G���^�[�����܂ő�����j
	char bkey[256];
	char key[256];

	//�}�E�X�ƍł��߂��ӂ�Ԃ� (dist��艓�����(false, -1, -1)��Ԃ�)
	tuple<bool, int, int> get_nearest_line(int mouseX, int mouseY, double dist);

	//�E�N���b�N�̃C�x���g�F���p�`�̔��]
	void right_click(int mouseX, int mouseY);

	//���N���b�N�̃C�x���g�F�o����(src), ������(dst)�̕ӂ̑I��
	void left_click(int mouseX, int mouseY);

	//������
	void init();

	//���̕`��
	void DrawArrow(int x1, int y1, int x2, int y2, int color, int thickness);

public:
	//���p�`�̂����������L��, �\���o�[�֓n��
	int src_poly_id; int src_line_id;
	bool dst_is_piece; int dst_poly_id; int dst_line_id;

	int set_query();	//�v���Z�X���b�Z�[�W��Ԃ�
};