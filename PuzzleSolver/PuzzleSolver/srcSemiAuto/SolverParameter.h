//�����蔻���]���֐��ŗp����A�p�����[�^���`���܂��B
//���e�덷�ϐ�, �d�ݕϐ�, ���\���̂ŊǗ����܂�

#pragma once

class SolverParameter {
public:
	double dist_error_waku;		//�����蔻��F�g�Ƃ̓����蔻��͊ɂ߂ɂ�����
	double dist_error;			//�����蔻��, �]���֐�, �}�[�W�����F��������ƃp�Y�����������Ȃ����A�ɂ��ƕςȂ̂�����
	double angle_error;			//�]���֐�, �}�[�W�����F�p�x�a��360 - angle_error �` 360 + angle_error�̊ԂȂ���_
	double dist_error_tigit;	//�]���֐��F��萳�m�Ɉ�v���Ă����獂�]���i��������v�ӂ̕]���݂̂ɗp����j
	double angle_error_tigit;	//�]���֐��F��萳�m�Ɉ�v���Ă����獂�]��

	double weight_sameAngle;		//2�p����v���Ă����Ƃ��̉��_
	double weight_sameLine;			//2�ӂ���v���Ă����Ƃ��̉��_
	double weight_straight;			//2�ӂ��꒼����ɂ���Ƃ��̉��_
	double weight_sameHalfLine;		//2�ӂ����������ň�_�Őڂ��Ă����Ƃ��̉��_

	double weight_sameAngle_tigit;	//2�p������v���Ă����Ƃ��̉��_
	double weight_sameLine_tigit;	//2�ӂ�����v���Ă����Ƃ��̉��_
};
