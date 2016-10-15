//�����蔻���]���֐��ŗp����A�p�����[�^���`���܂��B
//���e�덷�ϐ�, �d�ݕϐ�, ���\���̂ŊǗ����܂�

#pragma once

class SolverParameter {
public:
	double dist_error_waku_meter;	//�����蔻��F�g�Ƃ̓����蔻��͊ɂ߂ɂ�����
	double dist_error_meter;		//�����蔻��, �]���֐�, �}�[�W�����F��������ƃp�Y�����������Ȃ����A�ɂ��ƕςȂ̂�����
	double pixel_per_meter;			//1���[�g��������̃s�N�Z����

	double dist_error_waku() {
		return dist_error_waku_meter * pixel_per_meter;
	}

	double dist_error() {
		return dist_error_meter * pixel_per_meter;
	}

	//�p�x�̋��e�덷�����ŕԂ�
	double angle_error(double line_length) {
		//tan(angle_error) <= dist_error / line_length
		double angle_error = asin(dist_error() / line_length) * 180 / 3.1415926;
		return angle_error;
	}
};
