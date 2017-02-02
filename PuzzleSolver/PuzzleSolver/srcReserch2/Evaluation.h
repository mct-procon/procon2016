#include "Poly.h"
#include "DxLib.h"
using namespace std;

class Evaluation {
	vector<double> sorted_angle;
	vector<double> sorted_length;
	double most_exist_angle;

public:
	void make_histgram(vector<Poly> &wakus, vector<Poly> &pieces) {
		int i, j;

		sorted_angle.clear();
		sorted_length.clear();
		for (i = 0; i < wakus.size(); i++) {
			for (j = 0; j < wakus[i].size(); j++) {
				sorted_angle.push_back(wakus[i].angle(j));
				sorted_length.push_back(abs(wakus[i].points[j + 1] - wakus[i].points[j]));
			}
		}
		for (i = 0; i < pieces.size(); i++) {
			for (j = 0; j < pieces[i].size(); j++) {
				sorted_angle.push_back(pieces[i].angle(j));
				sorted_length.push_back(abs(pieces[i].points[j + 1] - pieces[i].points[j]));
			}
		}
		sort(sorted_angle.begin(), sorted_angle.end());
		sort(sorted_length.begin(), sorted_length.end());


		//�ł����݂���p�x�����߂�B�������炢�̊p�x�͑�̏㉺3�x�܂��ŏW������̂ŁA���̂��炢�Œ��ׂĂ����B
		int cnt = -1;
		for (double angle = 0; angle < 360; angle += 3) {
			if (cnt < get_count_angle(angle, 3)) {
				cnt = get_count_angle(angle, 3);
				most_exist_angle = angle;
			}
		}
	}

	//angle - range �` angle + range���̊p�������邩�H
	int get_count_angle(double angle, double range) {
		return upper_bound(sorted_angle.begin(), sorted_angle.end(), angle + range) - lower_bound(sorted_angle.begin(), sorted_angle.end(), angle - range);
	}

	//length - range �` length + range�s�N�Z���̕ӂ������邩�H
	int get_count_length(double length, double range) {
		return upper_bound(sorted_length.begin(), sorted_length.end(), length + range) - lower_bound(sorted_length.begin(), sorted_length.end(), length - range);
	}
	
	//�ŏ��p
	double get_min_angle() {
		if (sorted_angle.size() == 0) { return 0; }
		return sorted_angle[0];
	}

	//�ŕp�p
	double get_most_exist_angle() {
		return most_exist_angle;
	}

	//�q�X�g�O�����̕`��
	void draw_histgram(double range_length, double range_angle) {
		int i;
		int cnt;
		int x, y;
		int center_x, center_y;

		//�ӂ̃q�X�g�O����
		center_x = 100;
		center_y = 300;
		for (i = 0; i < sorted_length.size(); i += cnt) {
			cnt = get_count_length(sorted_length[i], range_length);
			
			x = sorted_length[i] + center_x;
			y = -cnt * 250 / max(1, (int)sorted_length.size()) + center_y;		//250����
			
			//���オ(x - 2, y), �E����(x + 2, center_y)
			DrawBox(x - 2, y, x + 2, center_y, GetColor(0, 255, 0), TRUE);
			DrawFormatString(x - 7, y - 15, GetColor(0, 0, 0), "%d", cnt);
		}

		for (i = 0; i <= 1000; i += 100) {
			x = i + center_x;
			DrawFormatString(x - 7, center_y + 5, 0, "%d", i);
		}

		DrawLine(0, center_y, 10000, center_y, 0, 1);
		DrawLine(center_x, -10000, center_x, 100000, 0, 1);

		//�p�x�̃q�X�g�O����
		center_x = 100;
		center_y = 600;
		for (i = 0; i < sorted_angle.size(); i += cnt) {
			cnt = get_count_angle(sorted_angle[i], range_angle);

			x = sorted_angle[i] * 2 + center_x;
			y = -cnt * 250 / max(1, (int)sorted_angle.size()) + center_y;		//250����

			DrawBox(x - 2, y, x + 2, center_y, GetColor(0, 255, 0), TRUE);
			DrawFormatString(x - 7, y - 15, GetColor(0, 0, 0), "%d", cnt);
		}

		for (i = 0; i <= 360; i += 45) {
			x = i * 2 + center_x;
			DrawFormatString(x - 7, center_y + 5, 0, "%d", i);
		}

		DrawLine(0, center_y, 10000, center_y, 0, 1);
		DrawLine(center_x, -10000, center_x, 100000, 0, 1);
	}
};