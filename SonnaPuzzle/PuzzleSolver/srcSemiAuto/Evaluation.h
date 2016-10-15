#include "Poly.h"
using namespace std;

class Evaluation {
	vector<double> sorted_angle;
	vector<double> sorted_length;
	
public:
	void make_histgram(vector<Poly> &pieces) {
		int i, j;

		sorted_angle.clear();
		sorted_length.clear();
		for (i = 0; i < pieces.size(); i++) {
			for (j = 0; j < pieces[i].size(); j++) {
				sorted_angle.push_back(pieces[i].angle(j));
				sorted_length.push_back(abs(pieces[i].points[j + 1] - pieces[i].points[j]));
			}
		}
		sort(sorted_angle.begin(), sorted_angle.end());
		sort(sorted_length.begin(), sorted_length.end());
	}

	//angle - range ～ angle + range°が全体の何パーセントあるか？
	double get_percent_angle(double angle, double range) {
		if (sorted_angle.size() == 0) { return 100; }
		int cnt = upper_bound(sorted_angle.begin(), sorted_angle.end(), angle + range) - lower_bound(sorted_angle.begin(), sorted_angle.end(), angle - range);
		return cnt * 100.0 / sorted_angle.size();
	}

	//length - range ～ length + rangeピクセルの辺が全体の何パーセントあるか？
	double get_percent_length(double length, double range) {
		if (sorted_length.size() == 0) { return 100; }
		int cnt = upper_bound(sorted_length.begin(), sorted_length.end(), length + range) - lower_bound(sorted_length.begin(), sorted_length.end(), length - range);
		return cnt * 100.0 / sorted_length.size();
	}
	
	//最小角
	double get_min_angle() {
		if (sorted_angle.size() == 0) { return 0; }
		return sorted_angle[0];
	}
};