//�S����ɂ܂Ƃ߂��B���Ŏ��̍H���ɐi�݁A����1�O�ɖ߂�B
//�R�[�h�F�L�[�{�[�h���̍X�V�ikeyboard.update())��������ʒu�ɏ�����Ă��ĊQ���ł���B�L�[�{�[�h��}�E�X�̃N���b�N����������Ƃ��ꂢ�ɏ����Ȃ����̂��낤���c

#include "DxLib.h"
#include <iostream>
#include <tuple>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <complex>
using namespace std;

const int INF = 876765346;
const double PAI = 3.14159265358979;

//�L�[�{�[�h (�S�N���X���ʂŗp����I�u�W�F�N�g, keyboard��p�ӂ���j
class Keyboard {
	char bkey[256], key[256];
public:
	Keyboard() {
		for (int i = 0; i < 256; i++) bkey[i] = 0;
	}
	void update() {
		for (int i = 0; i < 256; i++) bkey[i] = key[i];
		GetHitKeyStateAll(key);
	}
	bool isClick(int code) {
		return (bkey[code] == 0 && key[code] == 1);
	}
}keyboard;

//���Main������A���N���X
class Project {
public:
	virtual int Main(int is_piece) = 0;
};

//�H��1. �y�C���g
class MyPaint : public Project {
private:
	typedef tuple<int, int, int, int> T;	//(�n�_x, �n�_y) -> (�I�_x, �I�_y)�̏��Ŋi�[

	struct Point {
		int x, y;
		Point (int x, int y) { this->x = x; this->y = y; }
		Point () { }
	};

	//����p
	int bmouse, mouse;		//���N���b�N�̂݁Bbmouse��1�t���[���O, mouse�͍��̏��(1����, 0�Ȃ�)
	Point s;				//�n�_���o���Ă���

	//�f�[�^�p
	int defaultNum;		//�f�t�H���g�̐���(�g�Ɏg���Ă�������j�̌�
	vector<T> lines;

	//�g�Ɏg���Ă��������ǂݍ��ށB�����l��ǂݍ��ށB
	//�f�t�H���g�����̌�defaultNum�������Őݒ肷��B
	void inputWaku() {

		ifstream ifs("wakudata.txt");
		if (ifs.fail()) { return; }

		int wakuNum;
		ifs >> wakuNum;

		for (int i = 0; i < wakuNum; i++) {
			int tyotenNum;
			vector<Point> poly;
			ifs >> tyotenNum;
			poly.resize(tyotenNum);
			for (int j = 0; j < tyotenNum; j++) {
				ifs >> poly[j].x;
			}
			for (int j = 0; j < tyotenNum; j++) {
				ifs >> poly[j].y;
			}
			for (int j = 0; j < tyotenNum; j++) {
				int k = (j + 1) % tyotenNum;
				lines.push_back(T(poly[j].x, poly[j].y, poly[k].x, poly[k].y));
			}
		}
		defaultNum = lines.size();

		ifs.close();
	}

	//�O���[�o���ϐ��̏�����
	void init() {
		mouse = 0;
		lines.clear();
	}

	//�}�E�X�E�L�[�{�[�h���̍X�V
	void update() {
		bmouse = mouse;
		mouse = GetMouseInput() & MOUSE_INPUT_LEFT;
		keyboard.update();
	}

	//�}�E�X���N���b�N�����Ƃ��ɌĂ΂��֐�
	void clicked() {
		GetMousePoint(&s.x, &s.y);
	}

	//�}�E�X�𗣂����Ƃ��ɌĂ΂��֐�
	void released() {
		Point g;
		GetMousePoint(&g.x, &g.y);
		if (abs(s.x - g.x) + abs(s.y - g.y) < 10) { return; }

		lines.push_back(T(s.x, s.y, g.x, g.y));
	}

	//�Ō�ɏ����������������C�x���g(������������Ή������Ȃ�)
	void restore() {
		if (lines.size() > defaultNum) { lines.pop_back(); }
	}

	//�����̏W����`�悷��֐�
	void draw(int is_piece) {
		for (int i = 0; i < lines.size(); i++) {
			int x1 = get<0>(lines[i]);
			int y1 = get<1>(lines[i]);
			int x2 = get<2>(lines[i]);
			int y2 = get<3>(lines[i]);
			DrawLine(x1, y1, x2, y2, 0, 2);
		}
		if (mouse == 1) {
			Point g;
			GetMousePoint(&g.x, &g.y);
			DrawLine(s.x, s.y, g.x, g.y, 0, 2);
		}
		if (is_piece) { DrawFormatString(400, 25, GetColor(0, 0, 255), "�s�[�X���쐬��"); }
		else { DrawFormatString(400, 25, GetColor(0, 0, 255), "�g���쐬��"); }
		DrawFormatString(400, 50, 0, "�}�E�X�Ő�������");
		DrawFormatString(400, 75, 0, "���ɖ߂��Fz, �����F��");
	}

	//�����̏W�����t�@�C���ɏo�͂���֐� (������1�`2�L�΂��ďo�́j
	void print(char *filename) {
		ofstream ofs(filename);

		ofs << lines.size() << endl;
		for (int i = 0; i < lines.size(); i++) {
			double x1 = get<0>(lines[i]);
			double y1 = get<1>(lines[i]);
			double x2 = get<2>(lines[i]);
			double y2 = get<3>(lines[i]);

			//�����O�w�A�[����
			typedef complex<double> P;
			P vec = P(x2 - x1, y2 - y1);
			vec /= abs(vec);

			P s = P(x1, y1); s -= vec * P(3, 0);
			P e = P(x2, y2); e += vec * P(3, 0);
			x1 = s.real();
			y1 = s.imag();
			x2 = e.real();
			y2 = e.imag();

			ofs << (int)x1 << " " << (int)y1 << " " << (int)x2 << " " << (int)y2 << endl;
		}
	}

public:
	//���C��
	int Main(int is_piece) {
		init();
		if (is_piece) { inputWaku(); }

		// while(����ʂ�\��ʂɔ��f,���b�Z�[�W����,��ʃN���A, �����ŏI��)
		while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && !keyboard.isClick(KEY_INPUT_LEFT) && !keyboard.isClick(KEY_INPUT_RIGHT)) {
			update();
			if (bmouse == 0 && mouse == 1) { clicked(); }
			if (bmouse == 1 && mouse == 0) { released(); }
			if (mouse == 0 && keyboard.isClick(KEY_INPUT_Z)) { restore(); }
			draw(is_piece);
		}
		print("lines.txt");
		return ProcessMessage();
	}
};

//�H��2. �����J�b�g
class Cut : public Project {
private:
	struct Point {
		double x, y;
		Point (double x, double y) { this->x = x; this->y = y; }
		Point () { }
		bool operator<(const Point &r) const { return (x != r.x) ? x < r.x : y < r.y; }
	};
	typedef pair<Point, Point> L;	//�n�_ -> �I�_�̏��Ŋi�[

	//�f�[�^�p
	int n;
	L lines[1000];
	bool isExist[1000];
	vector<Point> iPointLists[1000];	//iPointLists[i] = ����i�Ǝ��݂�������̌�_, �̏W��

	//�ǂݍ��݁i�Ə������j, ������true, ���s��false��Ԃ�
	bool input() {
		ifstream ifs("lines.txt");
		if (ifs.fail()) { return false; }
		ifs >> n;
		for (int i = 0; i < n; i++) {
			int x1, y1, x2, y2;
			ifs >> x1 >> y1 >> x2 >> y2;
			lines[i] = L(Point(x1, y1), Point(x2, y2));
		}
		ifs.close();

		for (int i = 0; i < n; i++) {
			isExist[i] = true;
		}
		return true;
	}

	//���s�Ȑ������}�[�W����
	void margeParallelLine() {
		int i, j;

		for (i = 0; i < n; i++) {
			if (!isExist[i]) { continue; }
			for (j = 0; j < n; j++) {
				if (i == j || !isExist[j]) { continue; }
				if (!isParallel(lines[i].first, lines[i].second, lines[j].first, lines[j].second)) { continue; }
				//����i + ����j�����i�ɑ�����A����j������
				Point p[4] = {lines[i].first, lines[i].second, lines[j].first, lines[j].second};
				sort(p, p + 4);
				lines[i] = L(p[0], p[3]);
				isExist[j] = false;
			}
		}
	}

	//�����̌�_���X�giPointLists[]���쐬����
	void set_iPointLists() {
		int i, j;

		for (i = 0; i < n; i++) {
			iPointLists[i].clear();
		}

		for (i = 0; i < n; i++) {
			if (!isExist[i]) { continue; }
			for (j = 0; j < n; j++) {
				if (i == j || !isExist[j]) { continue; }
				Point p = getIPoint(lines[i].first, lines[i].second, lines[j].first, lines[j].second);	//��_p
				if (p.x == INF && p.y == INF) { continue; }	//��_�����݂��Ȃ�
				iPointLists[i].push_back(p);	//����i�̌�_���X�g���X�V
			}
		}
	}

	//�e�����̌�_���X�g�̓_��(x���� -> y����)�Ń\�[�g����
	void sort_iPointLists() {
		for (int i = 0; i < n; i++) {
			sort(iPointLists[i].begin(), iPointLists[i].end());
		}
	}

	//��_��1�ȉ��̐����������B�����������������false��Ԃ��B
	bool eraseLines() {
		bool ret = false;
		for (int i = 0; i < n; i++) {
			if (!isExist[i]) { continue; }
			if (iPointLists[i].size() <= 1) { isExist[i] = false; ret = true; }
		}
		return ret;
	}

	//��_��2�ȏ�̐����ɂ��āA�͂ݏo������(�ǂ�2�̌�_�Ԃɂ����݂��Ȃ�����)���J�b�g����B�J�b�g����������������false��Ԃ��B
	//sort_iPointLists()���Ă�ł���Ăяo�����ƁB
	bool cutLines() {
		bool ret = false;
		for (int i = 0; i < n; i++) {
			if (!isExist[i]) { continue; }
			if (iPointLists[i].size() <= 1) { continue; }	//eraseLines()���Ă�łȂ��Ă��듮�삳���Ȃ��u���܂��Ȃ����v

			Point p[2] = {lines[i].first, lines[i].second};
			sort(p, p + 2);

			if (p[0] < iPointLists[i][0] || iPointLists[i][iPointLists[i].size() - 1] < p[1]) {
				ret = true;
				lines[i].first = iPointLists[i][0];
				lines[i].second = iPointLists[i][iPointLists[i].size() - 1];
				//�����O�w�A�[�����i�[�_�����������L�΂��j(����ɖ���)
				typedef complex<double> P;
				P vec = P(lines[i].second.x - lines[i].first.x, lines[i].second.y - lines[i].first.y);
				vec /= abs(vec);

				P s = P(lines[i].first.x, lines[i].first.y); s -= vec * P(1e-1, 0);
				P e = P(lines[i].second.x, lines[i].second.y); e += vec * P(1e-1, 0);
				lines[i].first.x = s.real();
				lines[i].first.y = s.imag();
				lines[i].second.x = e.real();
				lines[i].second.y = e.imag();
			}
		}
		return ret;
	}

	//����a -> b, c -> d�̕��s����
	bool isParallel(Point a, Point b, Point c, Point d) {
		return (a.x - b.x) * (c.y - d.y) == (a.y - b.y) * (c.x - d.x);
	}

	//����a -> b, c -> d�̌������� (T���������Ƃ݂Ȃ�)
	bool isHit(Point a, Point b, Point c, Point d) {
		long double u, v;
		//0 = (a.y - b.y)(x - a.x) + (a.x - b.x)(a.y - y)�̗�����c, d�����邩
		u = (a.y - b.y) * (c.x - a.x) + (a.x - b.x) * (a.y - c.y);
		v = (a.y - b.y) * (d.x - a.x) + (a.x - b.x) * (a.y - d.y);
		if (u * v > 0) { return false; }
		//0 = (c.y - d.y)(x - c.x) + (c.x - d.x)(c.y - y)�̗�����a, b�����邩
		u = (c.y - d.y) * (a.x - c.x) + (c.x - d.x) * (c.y - a.y);
		v = (c.y - d.y) * (b.x - c.x) + (c.x - d.x) * (c.y - b.y);
		if (u * v > 0) { return false; }
		return true;
	}

	//����a -> b, c -> d�̌�_��Ԃ��i�����ꍇ�́A(INF, INF)��Ԃ��j
	Point getIPoint(Point a, Point b, Point c, Point d) {
		if (isParallel(a, b, c, d) || !isHit(a, b, c, d)) return Point(INF, INF);
		//�O�p�`a -> c -> b�̖ʐ� * 2��s1, �O�p�`a -> d -> b�̖ʐ� * 2��s2�Ƃ���
		long double s1 = abs((a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x));
		long double s2 = abs((a.x - d.x) * (b.y - d.y) - (a.y - d.y) * (b.x - d.x));
		if (s1 == 0) { return Point(c.x, c.y); }
		if (s2 == 0) { return Point(d.x, d.y); }
		long double x = (s1 * d.x + s2 * c.x) / (s1 + s2);
		long double y = (s1 * d.y + s2 * c.y) / (s1 + s2);
		return Point((double)x, (double)y);
	}

	//�`��
	void draw() {
		for (int i = 0; i < n; i++) {
			if (!isExist[i]) { continue; }
			int x1 = lines[i].first.x;
			int y1 = lines[i].first.y;
			int x2 = lines[i].second.x;
			int y2 = lines[i].second.y;
			DrawLine(x1, y1, x2, y2, 0, 2);
			DrawFormatString(x1, y1, GetColor(0, 0, 255), "%d", i);
			DrawFormatString(x2, y2, GetColor(255, 0, 0), "%d", i);
		}
		DrawFormatString(400, 25, 0, "Prev : ��, Next : ��");
	}

	//�o��
	void print() {
		int cnt = 0;

		for (int i = 0; i < n; i++) {
			if (!isExist[i]) { continue; }
			cnt++;
		}

		ofstream ofs("lines_cuted.txt");

		ofs << cnt << endl;
		for (int i = 0; i < n; i++) {
			if (!isExist[i]) { continue; }
			ofs << fixed << setprecision(6) << lines[i].first.x << " " << lines[i].first.y << " " << lines[i].second.x << " " << lines[i].second.y << endl;
		}
	}

public:
	//���C��
	int Main(int is_piece) {
		input();
		margeParallelLine();
		bool isContinue = true;
		for (int i = 0; i < 2 * n && isContinue; i++) {
			isContinue = false;
			set_iPointLists();
			sort_iPointLists();
			isContinue = isContinue || eraseLines();
			isContinue = isContinue || cutLines();
		}

		// while(����ʂ�\��ʂɔ��f,���b�Z�[�W����,��ʃN���A, �����ŏI��)
		while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && !keyboard.isClick(KEY_INPUT_LEFT) && !keyboard.isClick(KEY_INPUT_RIGHT)) {
			keyboard.update();
			draw();
		}
		print();
		return ProcessMessage();
	}
};

//�H��3. �����X�v���b�g
class Split : public Project {
private:
	struct Point {
		double x, y;
		Point (double x, double y) { this->x = x; this->y = y; }
		Point () { }
		bool operator<(const Point &r) const { return (x != r.x) ? x < r.x : y < r.y; }
	};
	typedef pair<Point, Point> L;	//�n�_ -> �I�_�̏��Ŋi�[

	//�f�[�^�p
	int n;
	L lines[1000];
	vector<Point> iPointLists[1000];	//iPointLists[i] = ����i�Ǝ��݂�������̌�_, �̏W��

	//����p
	int t;

	//�ǂݍ��݁i�Ə������j, ������true, ���s��false��Ԃ�
	bool input() {
		ifstream ifs("lines_cuted.txt");
		if (ifs.fail()) { return false; }
		ifs >> n;
		for (int i = 0; i < n; i++) {
			double x1, y1, x2, y2;
			ifs >> x1 >> y1 >> x2 >> y2;
			lines[i] = L(Point(x1, y1), Point(x2, y2));
		}
		return true;
	}

	//�����̌�_���X�giPointLists[]���쐬����
	void set_iPointLists() {
		int i, j;

		for (i = 0; i < n; i++) {
			iPointLists[i].clear();
		}

		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				if (i == j) { continue; }
				Point p = getIPoint(lines[i].first, lines[i].second, lines[j].first, lines[j].second);	//��_p
				if (p.x == INF && p.y == INF) { continue; }	//��_�����݂��Ȃ�
				iPointLists[i].push_back(p);	//����i�̌�_���X�g���X�V
			}
		}
	}

	//�e�����̌�_���X�g�̓_��(x���� -> y����)�Ń\�[�g����
	void sort_iPointLists() {
		for (int i = 0; i < n; i++) {
			sort(iPointLists[i].begin(), iPointLists[i].end());
		}
	}

	//�\�[�g��̌�_���X�g��p���āA�����𕪊�����B������̐����W����Ԃ��B
	vector<L> split_lines() {
		vector<L> spLines;

		for (int i = 0; i < n; i++) {
			for (int j = 0; j < iPointLists[i].size() - 1; j++) {
				spLines.push_back(L(iPointLists[i][j], iPointLists[i][j+1]));
			}
		}
		return spLines;
	}

	//�`��
	void draw() {
		for (int i = 0; i < n; i++) {
			int x1 = lines[i].first.x;
			int y1 = lines[i].first.y;
			int x2 = lines[i].second.x;
			int y2 = lines[i].second.y;
			DrawLine(x1, y1, x2, y2, 0, 2);
			if ((t / 60) % n == i) {
				DrawLine(x1, y1, x2, y2, GetColor(0, 255, 0), 2);
				DrawFormatString(x1, y1, GetColor(0, 0, 255), "%d", i);
				DrawFormatString(x2, y2, GetColor(255, 0, 0), "%d", i);
			}
		}
		DrawFormatString(400, 25, 0, "Prev : ��, Next : ��");
	}

	//�o��
	void print() {
		ofstream ofs("lines_splited.txt");

		ofs << n << endl;
		for (int i = 0; i < n; i++) {
			ofs << fixed << setprecision(6) << lines[i].first.x << " " << lines[i].first.y << " " << lines[i].second.x << " " << lines[i].second.y << endl;
		}
	}

	//����a -> b, c -> d�̌������� (T���������Ƃ݂Ȃ�)
	bool isHit(Point a, Point b, Point c, Point d) {
		long double u, v;
		//0 = (a.y - b.y)(x - a.x) + (a.x - b.x)(a.y - y)�̗�����c, d�����邩
		u = (a.y - b.y) * (c.x - a.x) + (a.x - b.x) * (a.y - c.y);
		v = (a.y - b.y) * (d.x - a.x) + (a.x - b.x) * (a.y - d.y);
		if (u * v > 0) { return false; }
		//0 = (c.y - d.y)(x - c.x) + (c.x - d.x)(c.y - y)�̗�����a, b�����邩
		u = (c.y - d.y) * (a.x - c.x) + (c.x - d.x) * (c.y - a.y);
		v = (c.y - d.y) * (b.x - c.x) + (c.x - d.x) * (c.y - b.y);
		if (u * v > 0) { return false; }
		return true;
	}

	//����a -> b, c -> d�̌�_��Ԃ��i�����ꍇ�́A(INF, INF)��Ԃ��j
	Point getIPoint(Point a, Point b, Point c, Point d) {
		if (!isHit(a, b, c, d)) return Point(INF, INF);
		//�O�p�`a -> c -> b�̖ʐ� * 2��s1, �O�p�`a -> d -> b�̖ʐ� * 2��s2�Ƃ���
		long double s1 = abs((a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x));
		long double s2 = abs((a.x - d.x) * (b.y - d.y) - (a.y - d.y) * (b.x - d.x));
		if (s1 == 0) { return Point(c.x, c.y); }
		if (s2 == 0) { return Point(d.x, d.y); }
		long double x = (s1 * d.x + s2 * c.x) / (s1 + s2);
		long double y = (s1 * d.y + s2 * c.y) / (s1 + s2);
		return Point((double)x, (double)y);
	}

public:
	//���C��
	int Main(int is_piece) {
		input();
		set_iPointLists();
		sort_iPointLists();
		vector<L> spLines = split_lines();
		n = spLines.size();
		for (int i = 0; i < n; i++) {
			lines[i] = spLines[i];
		}
		//sort(lines, lines + n);

		// while(����ʂ�\��ʂɔ��f,���b�Z�[�W����,��ʃN���A, �����ŏI��)
		t = 0;
		while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && !keyboard.isClick(KEY_INPUT_LEFT) && !keyboard.isClick(KEY_INPUT_RIGHT)) {
			keyboard.update();
			draw();
			t++;
		}
		print();
		return ProcessMessage();
	}
};

//�H��4. �O���t��
class GeneGraph : public Project {
private:
	struct Point {
		double x, y;
		Point (double x, double y) { this->x = x; this->y = y; }
		Point () { }
		bool operator<(const Point &r) const { return (x != r.x) ? x < r.x : y < r.y; }
	};
	typedef pair<Point, Point> L;	//�n�_ -> �I�_�̏��Ŋi�[

	//�f�[�^�p
	int n;
	L lines[1000];

	int gNum;		//�O���[�v�̐�
	int gid[2000];	//lines[i].first�̃O���[�v�ԍ���gid[2*i]�ɁAlines[i].second�̃O���[�v�ԍ���gid[2*i + 1]�ɓ���B

	//�ǂݍ��݁i�Ə������j, ������true, ���s��false��Ԃ�
	bool input() {
		ifstream ifs("lines_splited.txt");
		if (ifs.fail()) { return false; }
		ifs >> n;
		for (int i = 0; i < n; i++) {
			double x1, y1, x2, y2;
			ifs >> x1 >> y1 >> x2 >> y2;
			lines[i] = L(Point(x1, y1), Point(x2, y2));
		}
		return true;
	}

	//lines�̊e���_�ɔԍ�������U��B�߂��ʒu�ɂ��钸�_�ɓ����O���[�v�ԍ�������U��B
	//�e�O���[�v�̑�\���͒��_�ԍ��ilines[i].first�Ȃ�i * 2, lines[i].second�Ȃ�i * 2 + 1)���ł����������́I
	void makeGroup() {
		int i, j;

		for (i = 0; i < 2 * n; i++) { gid[i] = -1; }

		int id = 0;
		for (i = 0; i < 2 * n; i++) {
			if (gid[i] != -1) { continue; }

			Point p1 =(i % 2) ? lines[i / 2].second : lines[i / 2].first;
			gid[i] = id;
			for (j = i + 1; j < 2 * n; j++) {
				Point p2 = (j % 2) ? lines[j / 2].second : lines[j / 2].first;
				if (hypot(p1.x - p2.x, p1.y - p2.y) <= 1e-1) {
					gid[j] = id;
				}
			}
			id++;
		}
		gNum = id;
	}

	//�O���[�v�ԍ�i�̒��_�́i��\���́j�ʒu��ret[i]�ɓ���āAret��Ԃ�
	//�e�O���[�v�̑�\���͒��_�ԍ��ilines[i].first�Ȃ�i * 2, lines[i].second�Ȃ�i * 2 + 1)���ł����������́I
	vector<Point> makeTyoten() {
		vector<Point> ret;

		ret.resize(gNum);
		for (int i = 2 * n  - 1; i >= 0; i--) {
			ret[gid[i]] = (i % 2) ? lines[i / 2].second : lines[i / 2].first;
		}
		return ret;
	}

	//�O���[�v�ԍ��ł܂Ƃ߂����_�ɂ��ăO���t���\�z����B(src�O���[�v�ԍ� -> dst�O���[�v�ԍ��Ƃ��������ŁI�j
	vector<pair<int, int>> makeGraph() {
		vector<pair<int, int>> ret;

		for (int i = 0; i < n; i++) {
			//lines[i].first �� lines[i].second��o�����łȂ��̂Łc
			//lines[i].first�̃O���[�v�ԍ�(gid[2 * i])��lines[i].second�̃O���[�v�ԍ�(gid[2 * i + 1])��o�����ɂȂ��΂悢
			ret.push_back(pair<int, int>(gid[2 * i], gid[2 * i + 1]));
			ret.push_back(pair<int, int>(gid[2 * i + 1], gid[2 * i]));
		}
		return ret;
	}

	//�O���t�̏o��
	void print(vector<Point> v, vector<pair<int, int>> e) {
		ofstream ofs("lines_graph.txt");

		ofs << v.size() << endl;
		for (int i = 0; i < v.size(); i++) {
			ofs << setprecision(4) << fixed << v[i].x << " " << v[i].y << endl;
		}
		ofs << e.size() << endl;
		for (int i = 0; i < e.size(); i++) {
			ofs << e[i].first << " " << e[i].second << endl;
		}
	}

	//�O���t�̕`��
	void draw(vector<Point> v, vector<pair<int, int>> e) {

		for (int i = 0; i < e.size(); i++) {
			Point s = v[e[i].first];
			Point g = v[e[i].second];
			DrawLine(s.x, s.y, g.x, g.y, 0, 2);
		}

		static int t = 0;
		for (int i = 0; i < v.size(); i++) {
			if (i == (t / 30) % (v.size())) {
				DrawFormatString(v[i].x, v[i].y, GetColor(255, 0, 0), "%d", i);
			}
		}
		DrawFormatString(400, 25, 0, "Prev : ��, Next : ��");
		t++;
	}

public:
	//���C��
	int Main(int is_piece) {
		input();
		makeGroup();

		vector<Point> v = makeTyoten();
		vector<pair<int, int> > e = makeGraph();
		print(v, e);

		// while(����ʂ�\��ʂɔ��f,���b�Z�[�W����,��ʃN���A, �����ŏI��)
		while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && !keyboard.isClick(KEY_INPUT_LEFT) && !keyboard.isClick(KEY_INPUT_RIGHT)) {
			keyboard.update();
			draw(v, e);
		}
		return ProcessMessage();
	}
};

//�H��5. �s�[�X�̐��� (���_��͎��v���, �\����(y�����������Ȃ̂�)�����v���ɂȂ�j
class GenePiece : public Project {
private:
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

	//�ǂݍ��݁i�Ə������j, ������true, ���s��false��Ԃ�
	bool input() {
		ifstream ifs("lines_graph.txt");
		if (ifs.fail()) { return false; }

		ifs >> n;
		for (int i = 0; i < n; i++) {
			ifs >> pos[i].x >> pos[i].y;
		}
		for (int i = 0; i < n; i++) {
			et[i].clear();
			eang[i].clear();
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
	bool used[1000][1000];		//�ӂ̎g�p�t���O
	bool used2[1000];			//���_�̎g�p�t���O (�ꎞ�ۊǗp)
	vector<vector<int>> search(int is_piece) {
		vector<vector<int>> ret;
		int i, j;

		for (i = 0; i < n; i++) { for (j = 0; j < n; j++) { used[i][j] = false; } }
		while (true) {
			//�ŏ��̕ӂ�T������
			for (i = 0; i < n; i++) {
				for (j = 0; j < et[i].size(); j++) {
					if (used[i][et[i][j]]) continue;
					
					//���g�p�ӂ��������̂ŁA�s�[�X������Ă݂�
					vector<int> cycle;
					int v = et[i][j];
					double ang = eang[i][j];
					for (int k = 0; k < n; k++) { used2[k] = false; }
					
					used2[i] = true;
					cycle.push_back(i);
					while (!used2[v]) {
						used2[v] = true;
						cycle.push_back(v);
						int id = selectId(ang, v, is_piece);
						ang = eang[v][id];
						v = et[v][id];
					}
					if (v != i) { continue; }
					if (is_piece && getArea(toPoints(cycle)) <= 0) { continue; }	//�s�[�X�Ȃ�, ���v���̓_��
					if (!is_piece && getArea(toPoints(cycle)) >= 0) { continue; }	//�g�Ȃ�, �����v���̓_��
					//���_i���g�����p�`��o�^
					ret.push_back(cycle);
					//cycle�Ɏg������ (�L����) �̃t���O��true�ɂ���
					for (int k = 0; k < cycle.size(); k++) {
						if (k + 1 == cycle.size()) {
							used[cycle[k]][cycle[0]] = true;
						}
						else {
							used[cycle[k]][cycle[k+1]] = true;
						}
					}
					break;

				}
				if (j < et[i].size()) break;
			}
			if (i == n) break;
		}
		return ret;
	}

	//���O�̈ړ�������ang�ŁA�����_v�ɂ���Ƃ��A�ǂ̕ӂňړ����邩��Ԃ��B
	int selectId(double ang, int v, int is_piece) {
		int i;
		double minEval = INF;
		double maxEval = -INF;
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
				if (!is_piece) selectId = i;
			}
			if (eval > maxEval) {
				maxEval = eval;
				if (is_piece) selectId = i;
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
		DrawFormatString(400, 25, 0, "Prev : ��, Next : ��");
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

public:
	//���C��
	int Main(int is_piece) {
		input();
		vector<vector<int>> res = search(is_piece);

		/*for (int i = 0; i < res.size(); i++) {
		for (int j = 0; j < res[i].size(); j++) {
		printfDx("%d ", res[i][j]);
		}
		printfDx("\n");
		}*/

		// while(����ʂ�\��ʂɔ��f,���b�Z�[�W����,��ʃN���A, �����ŏI��)
		while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && !keyboard.isClick(KEY_INPUT_LEFT) && !keyboard.isClick(KEY_INPUT_RIGHT)) {
			keyboard.update();
			draw(res);
		}
		print(res);
		return ProcessMessage();
	}
};

//�H��6. ���i�s�[�X�̂݁j�̐���, ���_��͔����v���i�\���Ƃ��Ă͎��v���j
class GeneProb : public Project {
private:
	struct Point {
		double x, y;
		Point (double x, double y) { this->x = x; this->y = y; }
		Point () { }
		bool operator<(const Point &r) const { return (x != r.x) ? x < r.x : y < r.y; }
	};

	//�f�[�^�p
	int n;
	vector<Point> polyIni[1000];	//���H�O�̃s�[�X
	Point base[1000];	//�d�S

	vector<Point> poly[1000];	//���H��̃s�[�X

	//���H�p
	double rot[1000];	//�d�S�𒆐S�ɉ�]
	bool opFlag[1000];	//�d�SX�𒆐S�ɑΏ̈ړ�
	Point trans[1000];	//���s�ړ�
	int noiz;			//�ʒu�ɁA�v���X�}�C�i�Xnoiz�s�N�Z���̌덷��t����

	//����
	void input() {
		ifstream ifs("piecedata.txt");

		ifs >> n;
		for (int i = 0; i < n; i++) {
			int tyotenNum;
			//���_��
			ifs >> tyotenNum;
			polyIni[i].resize(tyotenNum);
			//x���W
			for (int j = 0; j < tyotenNum; j++) {
				ifs >> polyIni[i][j].x;
			}
			//y���W
			for (int j = 0; j < tyotenNum; j++) {
				ifs >> polyIni[i][j].y;
			}
		}
	}

	//������
	void init() {
		for (int i = 0; i < n; i++) {
			int tyotenNum = polyIni[i].size();

			base[i] = Point(0, 0);
			for (int j = 0; j < tyotenNum; j++) {
				base[i].x += polyIni[i][j].x;
				base[i].y += polyIni[i][j].y;
			}
			base[i].x /= tyotenNum;
			base[i].y /= tyotenNum;
			rot[i] = 0.0;
			opFlag[i] = false;
			trans[i] = Point(0, 0);
			poly[i].clear();
		}
	}

	//���H�p�ϐ���M��֐�
	void convert() {
		for (int i = 0; i < n; i++) {
			rot[i] = GetRand(628) / 100.0 - 3.14159265358979;	//�����_����]�ł��B��]�Ȃ��ɂ���ꍇ��rot[i] = 0�ɐݒ肵�Ă��������B
			trans[i].x = GetRand(301) - 150;
			trans[i].y = GetRand(301) - 150;
			//opFlag[i] = ((GetRand(2) == 1) ? true : false);		//true�ɂ���ƃs�[�Xi�����]���܂�
		}
		noiz = 0;		//���_���W�Ɍ덷������p�����[�^�ł��Bnoiz = 0�Ƃ���Ɩw�ǐ��m�ɂȂ�, noiz��傫������ƒ��_���W�Ɍ덷�������܂��B
	}

	//���H��̃s�[�Xpoly[i]�����֐�
	void makePoly(int is_piece) {
		for (int i = 0; i < n; i++) {
			//�R�s�[
			poly[i] = polyIni[i];
			if (is_piece) {
				//y����Ώ̂Ƃ��Ĕ��] (���_��̌����͕ω������Ȃ�, �̂Œ��_i�����_n-1-i�ɂȂ�)
				if (opFlag[i]) {
					vector<Point> tmp;
					tmp.resize(poly[i].size());
					for (int j = 0; j < poly[i].size(); j++) {
						tmp[j].x = poly[i][j].x;
						tmp[j].y = -poly[i][j].y;
					}
					for (int j = 0; j < poly[i].size(); j++) {
						poly[i][j] = tmp[(int)poly[i].size() - 1 - j];
					}
				}
				//��] (�d�S�𒆐S�Ƃ�����]�j
				for (int j = 0; j < poly[i].size(); j++) {
					double x = base[i].x + cos(rot[i]) * (poly[i][j].x - base[i].x) - sin(rot[i]) * (poly[i][j].y - base[i].y);
					double y = base[i].y + sin(rot[i]) * (poly[i][j].x - base[i].x) + cos(rot[i]) * (poly[i][j].y - base[i].y);
					poly[i][j].x = x;
					poly[i][j].y = y;
				}
				//���s�ړ��i���������j
				for (int j = 0; j < poly[i].size(); j++) {
					poly[i][j].x += trans[i].x;
					poly[i][j].y += trans[i].y;
				}
				//�m�C�Y
				for (int j = 0; j < poly[i].size(); j++) {
					poly[i][j].x += GetRand(2 * noiz + 1) - noiz;
					poly[i][j].y += GetRand(2 * noiz + 1) - noiz;
				}
			}
		}
	}

	//�`��
	void draw() {
		static int t = 0;
		int id = (t / 60) % n;

		//����
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < poly[i].size(); j++) {
				int x1 = poly[i][j].x;
				int y1 = poly[i][j].y;
				int x2 = poly[i][(j + 1) % poly[i].size()].x;
				int y2 = poly[i][(j + 1) % poly[i].size()].y;
				DrawLine(x1, y1, x2, y2, 0, 2);
			}
		}
		//�ΐ�
		for (int j = 0; j < poly[id].size(); j++) {
			int x1 = poly[id][j].x;
			int y1 = poly[id][j].y;
			int x2 = poly[id][(j + 1) % poly[id].size()].x;
			int y2 = poly[id][(j + 1) % poly[id].size()].y;
			DrawLine(x1, y1, x2, y2, GetColor(0, 255, 0), 2);
		}
		//���_�ԍ�
		for (int j = 0; j < poly[id].size(); j++) {
			int x = poly[id][j].x;
			int y = poly[id][j].y;
			DrawFormatString(x, y, GetColor(255, 0, 0), "%d", j);
		}
		DrawFormatString(400, 25, 0, "Prev : ��, Next : ��");
		t++;
	}

	//�o��
	void print(ofstream &ofs) {
		//�s�[�X��
		ofs << n << endl;
		for (int i = 0; i < n; i++) {
			int tyotenNum = poly[i].size();

			//���_��
			ofs << tyotenNum << endl;
			//x���W
			ofs << (int)poly[i][0].x;
			for (int j = 1; j < poly[i].size(); j++) {
				ofs << " " << (int)poly[i][j].x;
			}
			ofs << endl;
			//y���W
			ofs << (int)poly[i][0].y;
			for (int j = 1; j < poly[i].size(); j++) {
				ofs << " " << (int)poly[i][j].y;
			}
			ofs << endl;
			//���s
			ofs << endl;
		}
		ofs.close();
	}

public:
	//���C��
	int Main(int is_piece) {
		input();
		init();
		convert();
		makePoly(is_piece);

		// while(����ʂ�\��ʂɔ��f,���b�Z�[�W����,��ʃN���A, �����ŏI��)
		while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && !keyboard.isClick(KEY_INPUT_LEFT) && !keyboard.isClick(KEY_INPUT_RIGHT)) {
			keyboard.update();
			draw();
		}
		if (is_piece) {
			ofstream ofs("probData.txt", ios::app);
			print(ofs);
		}
		else {
			ofstream ofs("probData.txt");
			print(ofs);
			CopyFile("probData.txt", "wakudata.txt", FALSE);
		}
		return ProcessMessage();
	}
};

//�g��
class Expansion {
private:
	void expansion(vector<int> &v, double mul) {
		vector<int> ret;
		for (int i = 0; i < v.size(); i++) { ret.push_back(v[i] * mul); }
		v = ret;
	}

	void ioLine(ifstream &cin, ofstream &cout, int n, double mul) {
		vector<int> v;
		v.resize(n);
	
		for (int i = 0; i < n; i++) {
			cin >> v[i];
		}
		expansion(v, mul);
	
		cout << v[0];
		for (int i = 1; i < n; i++) {
			cout << " " << v[i];
		}
		cout << endl;
	}

	void io(ifstream &cin, ofstream &cout, double mul) {
		int numOfPoly;
	
		cin >> numOfPoly;
		cout << numOfPoly << endl;
	
		while (numOfPoly--) {
			int n;
			cin >> n;
			cout << n << endl;
		
			ioLine(cin, cout, n, mul);
			ioLine(cin, cout, n, mul);
			cout << endl;
		}
	}
public:
	int main(double mul) {
		{
			ifstream cin("probData.txt");
			ofstream cout("probData_New.txt");
			io(cin, cout, mul);
			io(cin, cout, mul);
		}
		{
			CopyFile("probData_New.txt", "probData.txt", FALSE);
		}
		return 0;
	}
};

//�t�H�[�}�b�g��V�������̂ɕϊ�
class ConvIn {
private:
	void io_poly(ifstream &cin, ofstream &cout, int n) {
		vector<int> x;
		vector<int> y;
		double t;
		x.resize(n);
		y.resize(n);
	
		for (int i = 0; i < n; i++) {
			cin >> t; x[i] = t;
		}
		for (int i = 0; i < n; i++) {
			cin >> t; y[i] = t;
		}
	
		for (int i = 0; i < n; i++) {
			cout << x[i] << " " << y[i] << endl;
		}
	}

	void io(ifstream &cin, ofstream &cout) {
		int numOfPoly;
	
		cin >> numOfPoly;
		cout << numOfPoly << endl;
	
		while (numOfPoly--) {
			int n;
			cin >> n;
			cout << n << endl;
			io_poly(cin, cout, n);
		}
	}
public:
	int main() {
		{
			ifstream cin("probData.txt");
			ofstream cout("probData_New.txt");
			io(cin, cout);
			io(cin, cout);
		}
		{
			CopyFile("probData_New.txt", "probData.txt", FALSE);
		}
		return 0;
	}
};


//C++�̃I�u�W�F�N�g�͎Q�Ƃł͂Ȃ����́I�I �|�����[�t�B�Y������������΁A�|�C���^�[���g���I�I
MyPaint paint;
Cut cut;
Split split;
GeneGraph gene_graph;
GenePiece gene_piece;
GeneProb gene_prob;

int stage = 0;	//���́A�H����\���ԍ�
Project *p[6] = {&paint, &cut, &split, &gene_graph, &gene_piece, &gene_prob};	//�uProject�N���X�̎q�N���X�̎��́v�̏ꏊ��z��ɓ���Ă����B

//�S�̃��C��
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 600, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	// while(����ʂ�\��ʂɔ��f,���b�Z�[�W����,��ʃN���A, ESC�ŏI��)
	int is_piece = 0;
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		keyboard.update();
		if (p[stage]->Main(is_piece) != 0) break;
		if (keyboard.isClick(KEY_INPUT_LEFT) && stage > 0) stage--;
		if (keyboard.isClick(KEY_INPUT_RIGHT)) stage++;
		if (stage > 5) { if (!is_piece) { is_piece = 1; stage = 0; continue; } else { break; } }
	}

	//���f�[�^�̐��`
	Expansion ex;
	ConvIn conv;
	ex.main(2.5);
	conv.main();

	//��2�����ɏ������ꏊ�ɖ��f�[�^�i�V���������̂��́j���������܂�܂��B
	//�t�@�C�����̎d�l�F
	//T�c���s�ړ�����̂Ƃ�����, R�c��]����̂Ƃ�����, U�c���]����̂Ƃ�����, _Noiz���c���W�l��-���`+���̌덷������Ƃ��ɂ���B��=0�̂Ƃ��͕t���Ȃ��B, ���c�ԍ��^�O
	//[T][R][U][_Noiz��]��.txt�̂悤�ɏ����܂��B���͔C�ӂ̕�����B
	//�ڂ����́AProbGene�N���X��convert()�֐����Q�Ƃ��Ă��������BopFlag[i] = true�ɂȂ�\��������΁A���]���H������Ƃ݂Ȃ��Ă��������B
	CopyFile("probData.txt", "TR_sample1.txt", FALSE);	//FALSE�ɂ���ƁA�t�@�C�������݂��Ă���ꍇ�Ɂu�㏑�������v�BTRUE�ɂ���Ɓu�㏑������Ȃ��v�B
	DxLib_End();
	return 0;
}