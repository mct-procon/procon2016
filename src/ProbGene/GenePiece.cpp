//lines_graph.txt��ǂݍ���ŁA�s�[�X�f�[�^�����B�s�[�X�f�[�^��piecedata.txt�ɏo�͂���B
//�t�H�[�}�b�g�͓K��, ���W�l�͏����_�ȉ�4���ŏo�͂���B����A�s�[�X�̒��_0���ʒu0�ɍ��킹�鏈���͕s�v�Ƃ���B
//s�L�[����������A�s�[�X�f�[�^�̃t�@�C���o�͂��J�n����B

#include "DxLib.h"
#include <iostream>
#include <tuple>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <iomanip>
using namespace std;

const int INF = 876765346;
const double PAI = 3.14159265358979;

struct Point {
	double x, y;
	Point (double x, double y) { this->x = x; this->y = y; }
	Point () { }
	bool operator<(const Point &r) const { return (x != r.x) ? x < r.x : y < r.y; }
};

//�f�[�^�p
int n;						//�O���t�̒��_�̐�
Point pos[1000];			//pos[i] = ���_i�̍��W
int m;						//�O���t�̕ӂ̐�
vector<int> et[1000];		//et[i][j] = ���_i��j�Ԗڂ̐ڑ��悪���_et[i][j]
vector<double> eang[1000];	//eang[i][j] = ��(���_i�����_et[i][j])�̊p�x�����W�A���\�L��������

//�v���g�^�C�v
bool input();
vector<vector<int>> search();
void draw(vector<vector<int>> polyList);
void print(vector<vector<int>> polyList);	//�s�[�X�f�[�^��piecedata.txt�ɏo�͂���

//���^�֐�
int selectId(double ang, int v);
vector<Point> toPoints(vector<int> pointId);	//���p�`�͒��_�ԍ�(pointId[0], pointId[1], �c, )�ō\�������
double getArea(vector<Point> points);			//���_points[0] -> points[1]�c�̏��łȂ������p�`�̕����t���ʐς�Ԃ��i<0�Ȃ玞�v���j

//���C��
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 600, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	input();
	vector<vector<int>> res = search();

	/*for (int i = 0; i < res.size(); i++) {
		for (int j = 0; j < res[i].size(); j++) {
			printfDx("%d ", res[i][j]);
		}
		printfDx("\n");
	}*/

	// while(����ʂ�\��ʂɔ��f,���b�Z�[�W����,��ʃN���A, ESC�ŏI��)
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		draw(res);
		if (CheckHitKey(KEY_INPUT_S)) { print(res); }
	}
	DxLib_End();
	return 0;
}

//�ǂݍ��݁i�Ə������j, ������true, ���s��false��Ԃ�
bool input() {
	ifstream ifs("lines_graph.txt");
	if (ifs.fail()) { return false; }
	
	ifs >> n;
	for (int i = 0; i < n; i++) {
		ifs >> pos[i].x >> pos[i].y;
	}
	ifs >> m;
	for (int i = 0; i < m; i++) {
		int src, dst;
		ifs >> src >> dst;
		et[src].push_back(dst);
		eang[src].push_back(atan2(pos[dst].y - pos[src].y, pos[dst].x - pos[src].x));	//src -> dst
	}
	return true;
}

//ret[i][j]�ɂ͑��p�`i�̒��_j�ɂ����钸�_�̔ԍ�������Bret��Ԃ��B
vector<vector<int>> search() {
	vector<vector<int>> ret;
	bool used[1000] = {false};
	bool used2[1000];
	int i, j;

	for (i = 0; i < n; i++) {
		if (used[i]) { continue; }

		//�ŏ��̕ӂ�T������
		for (j = 0; j < et[i].size(); j++) {
			vector<int> cycle;
			for (int k = 0; k < n; k++) {
				used2[k] = false;
			}
			used2[i] = true;
			cycle.push_back(i);

			int v = et[i][j];			//���_�ԍ���v
			double ang = eang[i][j];	//�ӂ̊p�x��eang[i][j]
			while (!used2[v]) {
				used2[v] = true;
				cycle.push_back(v);
				int id = selectId(ang, v);
				ang = eang[v][id];
				v = et[v][id];
			}
			if (v != i) { continue; }
			if (getArea(toPoints(cycle)) >= 0) { continue; }
			//���_i���g�����p�`��o�^
			ret.push_back(cycle);
			break;
		}
		//cycle�Ɏg�������_�̃t���O��true�ɂ���
		for (j = 0; j < n; j++) {
			used[j] |= used2[j];
		}
	}
	return ret;
}

//���O�̈ړ�������ang�ŁA�����_v�ɂ���Ƃ��A�ǂ̕ӂňړ����邩��Ԃ��B
int selectId(double ang, int v) {
	int i;
	double minEval = INF;
	int selectId = -1;

	//ang�𔽓]���܂�
	ang = ang + PAI;
	if (ang > PAI) { ang -= 2 * PAI; }

	//ang�ƁAv����o��ӂ̕���eang[v][i]���r���܂��B
	//ang���甽���v�܂��Ɍ��Ă����Ă����Ƃ��ŏ��ɂ����������T��. ������ang�Ɠ��������͖���������.
	for (i = 0; i < eang[v].size(); i++) {
		if (fabs(ang -eang[v][i]) <= 1e-5) {
			continue;
		}
		double eval = (eang[v][i] < ang) ? eang[v][i] - ang + 2 * PAI : eang[v][i] - ang;
		if (eval < minEval) {
			minEval = eval;
			selectId = i;
		}
	}

	return selectId;
}

//���_���W��Ԃ�
vector<Point> toPoints(vector<int> pointId) {
	vector<Point> ret;

	for (int i = 0; i < pointId.size(); i++) {
		ret.push_back(pos[pointId[i]]);
	}
	return ret;
}

//���_points[0] -> points[1]�c�̏��łȂ������p�`�̕����t���ʐς�Ԃ��i<0�Ȃ玞�v���j
double getArea(vector<Point> points) {
	double area = 0;

	for (int i = 0; i < points.size(); i++) {
		int j = (i + 1) % points.size();
		area += points[i].x * points[j].y - points[j].x * points[i].y;
	}
	area *= 0.5;
	return area;
}

//�`��
void draw(vector<vector<int>> polyList) {
	static int t = 0;
	
	//���ʂ̕\��
	for (int i = 0; i < polyList.size(); i++) {	//i�ڂ̑��p�`
		for (int j = 0; j < polyList[i].size(); j++) {	//j�ڂ̒��_
			int src = polyList[i][j];
			int dst = polyList[i][(j + 1) % polyList[i].size()];

			DrawLine(pos[src].x, pos[src].y, pos[dst].x, pos[dst].y, GetColor(0, 0, 0), 2);
		}
	}

	//���p�`�̕\��
	int id = (t / 90) % polyList.size();
	for (int j = 0; j < polyList[id].size(); j++) {	//j�ڂ̒��_
		int src = polyList[id][j];
		int dst = polyList[id][(j + 1) % polyList[id].size()];

		DrawLine(pos[src].x, pos[src].y, pos[dst].x, pos[dst].y, GetColor(0, 255, 0), 2);
	}

	//���_�ԍ��̕\��
	for (int j = 0; j < polyList[id].size(); j++) {	//j�ڂ̒��_
		int src = polyList[id][j];
		DrawFormatString(pos[src].x, pos[src].y, GetColor(255, 0, 0), "%d", j);
	}
	t++;
}

//�s�[�X�f�[�^��piecedata.txt�ɏo�͂���
//�����ŁA�����_�͎l�̌ܓ��B
void print(vector<vector<int>> polyList) {
	ofstream ofs("piecedata.txt");
	
	ofs << polyList.size() << endl;
	for (int i = 0; i < polyList.size(); i++) {
		//�s�[�X�̒��_��
		ofs << polyList[i].size() << endl;
		//x���W
		ofs << fixed << setprecision(4) << pos[polyList[i][0]].x;
		for (int j = 1; j < polyList[i].size(); j++) {
			int src = polyList[i][j];
			ofs << " " << fixed << setprecision(4) << pos[src].x;
		}
		ofs << endl;
		//y���W
		ofs << fixed << setprecision(4) << pos[polyList[i][0]].y;
		for (int j = 1; j < polyList[i].size(); j++) {
			int src = polyList[i][j];
			ofs << " " << fixed << setprecision(4) << pos[src].y;
		}
		ofs << endl;
		//���s
		ofs << endl;
	}
	ofs.close();
}