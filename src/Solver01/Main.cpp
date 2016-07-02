//�܂���1�̃t�@�C���Ń\���o�[���������Ă݂܂��B
#include "DxLib.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
using namespace std;

const double PAI = 3.14159265358979;

class Point {
public:
	double x, y;
	Point (double x, double y) { this->x = x; this->y = y; }
	Point () { }
	bool operator<(const Point &r) const { return (x != r.x) ? x < r.x : y < r.y; }
	Point operator-(const Point &r) const { return Point(x - r.x, y - r.y); }
	Point operator+(const Point &r) const { return Point(x + r.x, y + r.y); }
	double distance(Point &r) { return sqrt((r.x - x) * (r.x - x) + (r.y - y) * (r.y - y)); }				//�_(x, y)����_(r.x, r.y)�܂ł̋�����Ԃ�
	double angle() { double ang = atan2(y, x); /*�l��(-��, ��]*/ if (ang < 0) ang += 2 * PAI; return ang; }	//�ʒu�x�N�g��(x, y)�̊p�x (���W�A��)��Ԃ�, �l���[0, 2��)
};

class Poly {
private:
	Point point0;					//point0          = ���_0�̍��W
	vector<Point> points;			//points[i]       = ���_0�ɑ΂��钸�_i�̍��W
	vector<double> edges_length;	//edges_length[i] = ��i�̒����B��i = ���_i���n�_, ���_i+1���I�_�Ƃ���L�������B
	vector<double> edges_angle;		//edges_angle[i]  = ��i�̊p�x�i���W�A���j�B�l���[0, 2��)
	vector<double> points_angle;	//points_angle[i] = ���_i�̓��p�i���W�A���j�B�l���[0, 2��)

	bool line_ishit(Point a, Point b, Point c, Point d);	//����a->b�Ɛ���c->d�̌�������. T��������false�Ƃ݂Ȃ�
	void translate(Point t);
	void rotate(double angle);

public:
	Point get_point(int point_id);
	int get_point_num();
	Poly ();
	Poly (Point point0, vector<Point> points);
	void move(int point_id, Point s, Point e);
	bool ishit(Poly &r);
	bool iscover(Poly &r);
	bool isoverlap(Poly &r);
};

class Solver {
protected:
	vector<Poly> pieces;
	vector<Poly> wakus;		//wakus[id] = id(>=0)�Ԃ̘g��
	vector<bool> isputted;	//isputted[id] = �s�[�Xid��u�����Ȃ�true, �u���ĂȂ��Ȃ�false

	bool canput(Poly &piece);

public:
	void input(string filename);
	void print(string filename);
	void draw(bool isdraw_didnot_put);
};

class AllSearch : public Solver {
private:
	bool dfs(int dep, const int maxdep);

public:
	void solve();
};

/*���� Poly�N���X*/

//�_point_id�̐�΍��W�̎擾
Point Poly::get_point(int point_id) {
	return points[point_id] + point0;
}

//���_�����擾
int Poly::get_point_num() {
	return points.size();
}

//Please read this homepage : http://www5d.biglobe.ne.jp/%7Etomoya03/shtml/algorithm/Intersection.htm
//�����̌�������iT��������false�j�_(x1,y1), (x2,y2)��ʂ钼���̎��F(x1-x2)*(y-y1)+(y1-y2)*(x1-x)=0���g�p�B
bool Poly::line_ishit(Point a, Point b, Point c, Point d) {
	double ta = (c.x - d.x) * (a.y - c.y) + (c.y - d.y) * (c.x - a.x);
	double tb = (c.x - d.x) * (b.y - c.y) + (c.y - d.y) * (c.x - b.x);
	double tc = (a.x - b.x) * (c.y - a.y) + (a.y - b.y) * (a.x - c.x);
	double td = (a.x - b.x) * (d.y - a.y) + (a.y - b.y) * (a.x - d.x);
	double eps = 0.01;
	return ta * tb < 0 && tc * td < 0;	// �E��(0)�̒l�������������false��Ԃ��₷���Ȃ�B�傫�������true��Ԃ��₷���Ȃ�B0�ɂ���Ɓi�ڐG��false�Ƃ݂Ȃ��j�����Ȍ�������ɂȂ�B
}

//(t.x, t.y)�������s�ړ�����
void Poly::translate(Point t) {
	point0.x += t.x;
	point0.y += t.y;
}

//angle[���W�A��]������]����Bangle�̒l��F[-2��, 2��]
void Poly::rotate(double angle) {
	for (int id = 0; id < points.size(); id++) {
		double nx = cos(angle) * points[id].x - sin(angle) * points[id].y;
		double ny = sin(angle) * points[id].x + cos(angle) * points[id].y;
		points[id].x = nx;
		points[id].y = ny;
	}
}

//�������i�������Ȃ���j
Poly::Poly () {
}

//�������i�ӂ̒���, �p�x�Ȃǂ����߂�)
Poly::Poly (Point point0, vector<Point> points) {
	this->point0 = point0;
	this->points = points;

	//������points���g��
	int n = points.size();		//���_��
	edges_length.resize(n);
	edges_angle.resize(n);
	points_angle.resize(n);
	points.push_back(points[0]);
	for (int id = 0; id < n; id++) { edges_length[id] = points[id].distance(points[id + 1]); }
	for (int id = 0; id < n; id++) { edges_angle[id] = (points[id + 1] - points[id]).angle(); }
	for (int id = 0; id < n; id++) {
		points_angle[id] = PAI - (edges_angle[id] - edges_angle[(id + n - 1) % n]);
		while (points_angle[id] < 0) { points_angle[id] += 2 * PAI; }
		while (points_angle[id] >= 2 * PAI){  points_angle[id] -= 2 * PAI; }
	}
}

//���_point_id�̐�΍��W��s�ɂȂ�悤�ɕ��s�ړ����A���_point_id -> point_id + 1�����ԗL��������s->e�Ɠ��������ɂȂ�悤�ɉ�]����
void Poly::move(int point_id, Point s, Point e) {
	Point a = get_point(point_id);
	Point b = get_point((point_id + 1) % get_point_num());

	translate(s - a);
	rotate((e - s).angle() - (b - a).angle());
}

//���p�`r�ƏՓ˂��Ă��邩�𔻒肷��B�Փ˂��Ă����true��Ԃ��B(�ڂ��Ă���ꍇ��false)
bool Poly::ishit(Poly &r) {
	int n = points.size();
	int m = r.points.size();

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			if (line_ishit(get_point(i), get_point((i + 1) % n), r.get_point(j), r.get_point((j + 1) % n))) {
				return true;
			}
		}
	}
	return false;
}

//���p�`r���܂ނ��𔻒肷��B���p�`r�̂��钸�_�Ƒ��p�`this�̂���ӂ��߂��ꍇ�̓���͖���`�B
bool Poly::iscover(Poly &r) {
	int i, j;

	for (i = 0; i < r.points.size(); i++) {
		//r.points[i]�����p�`�ɓ����ɂ��邩�𔻒�B�����ɂȂ����, ���̎��_��false��Ԃ�
		int cnt = 0;
		Point a = r.get_point(i);
		Point b = Point(a.x, 99999);
		for (j = 0; j < points.size(); j++) {
			Point c = get_point(j);
			Point d = get_point((j + 1) % points.size());
			if (line_ishit(a, b, c, d)) {
				cnt++;
			}
		}
		if (cnt % 2 == 0) {
			return false;
		}
	}
	return true;
}

//���p�`r�Əd�Ȃ邩�𔻒肷��B�d�Ȃ�Ȃ�true��Ԃ��B���E���false
bool Poly::isoverlap(Poly &r) {
	if (ishit(r)) return true;
	if (iscover(r)) return true;
	if (r.iscover(*this)) return true;
	return false;
}

/*���� Solver�N���X (*solve()�֐��͎q�N���X�Ŏ������܂�)*/

//���ɒu�����s�[�X��piece���d�Ȃ炸�A���A�S�Ă̘g����piece�����Ă����true��Ԃ��B
bool Solver::canput(Poly &piece) {
	for (int i = 0; i < isputted.size(); i++) {
		if (piece.isoverlap(pieces[i])) {
			return false;
		}
	}
	for (int i = 0; i < wakus.size(); i++) {
		if (!wakus[i].iscover(piece)) {
			return false;
		}
	}
	return true;
}

//�t�@�C������f�[�^����͂��A�e��ϐ�������������
void Solver::input(string filename) {
	ifstream ifs(filename);
	if (ifs.fail()) { return; }

	int waku_num, piece_num;
	int point_num;

	ifs >> waku_num;
	for (int i = 0; i < waku_num; i++) {
		Point point0;
		vector<Point> points;

		ifs >> point_num;
		points.resize(point_num);
		//X���W
		for (int j = 0; j < point_num; j++) {
			ifs >> points[j].x;
		}
		//Y���W
		for (int j = 0; j < point_num; j++) {
			ifs >> points[j].y;
		}
		//���΍��W�`���ɂ���
		point0 = points[0];
		for (int j = 0; j < point_num; j++) {
			points[j] = points[j] - point0;
		}
		//�g����ǉ�
		wakus.push_back(Poly(point0, points));
	}

	ifs >> piece_num;
	for (int i = 0; i < piece_num; i++) {
		Point point0;
		vector<Point> points;

		ifs >> point_num;
		points.resize(point_num);
		//X���W
		for (int j = 0; j < point_num; j++) {
			ifs >> points[j].x;
		}
		//Y���W
		for (int j = 0; j < point_num; j++) {
			ifs >> points[j].y;
		}
		//���΍��W�`���ɂ���
		point0 = points[0];
		for (int j = 0; j < point_num; j++) {
			points[j] = points[j] - point0;
		}
		//�s�[�X��ǉ�
		pieces.push_back(Poly(point0, points));
	}
	
	//�t���O
	isputted.resize(piece_num);
	for (int i = 0; i < piece_num; i++) {
		isputted[i] = false;
	}
	ifs.close();
}

//�t�@�C���֓������o�͂���
void Solver::print(string filename) {
	ofstream ofs(filename);
	
	ofs << wakus.size() << endl;
	for (int i = 0; i < wakus.size(); i++) {
		int n = wakus[i].get_point_num();
		//���_��
		ofs << n << endl;
		//X���W
		ofs << wakus[i].get_point(0).x;
		for (int j = 1; j < n; j++) {
			ofs << " " << wakus[i].get_point(j).x;
		}
		ofs << endl;
		//Y���W
		ofs << wakus[i].get_point(0).y;
		for (int j = 1; j < n; j++) {
			ofs << " " << wakus[i].get_point(j).y;
		}
		ofs << endl;
		//���s
		ofs << endl;
	}

	ofs << pieces.size() << endl;
	for (int i = 0; i < pieces.size(); i++) {
		int n = pieces[i].get_point_num();
		//���_��
		ofs << n << endl;
		//X���W
		ofs << pieces[i].get_point(0).x;
		for (int j = 1; j < n; j++) {
			ofs << " " << pieces[i].get_point(j).x;
		}
		ofs << endl;
		//Y���W
		ofs << pieces[i].get_point(0).y;
		for (int j = 1; j < n; j++) {
			ofs << " " << pieces[i].get_point(j).y;
		}
		ofs << endl;
		//���s
		ofs << endl;
	}
}

//�g�ƃs�[�X�̕`��
void Solver::draw(bool isdraw_didnot_put) {
	int i, j;

	for (i = 0; i < wakus.size(); i++) {
		for (j = 0; j < wakus[i].get_point_num(); j++) {
			Point s = wakus[i].get_point(j);
			Point e = wakus[i].get_point((j + 1) % wakus[i].get_point_num());
			DrawLine((int)s.x, (int)s.y, (int)e.x, (int)e.y, GetColor(0, 255, 0), 2);
		}
	}

	for (i = 0; i < pieces.size(); i++) {
		if (!isdraw_didnot_put && !isputted[i]) { continue; }
		for (j = 0; j < pieces[i].get_point_num(); j++) {
			Point s = pieces[i].get_point(j);
			Point e = pieces[i].get_point((j + 1) % pieces[i].get_point_num());
			DrawLine((int)s.x, (int)s.y, (int)e.x, (int)e.y, GetColor(0, 0, 255), 2);
		}
	}
}

/*���� AllSearch�N���X*/
bool AllSearch::dfs(int dep, const int maxdep) {
	pieces[0].move(0, wakus[0].get_point(0), wakus[0].get_point(1));
	if (wakus[0].iscover(pieces[0])) isputted[0] = true;	//�g��0�̒��Ƀs�[�X0���܂܂�Ă��, if�����ɓ���B�i�s�[�X0���܂܂�Ă���ɂ��ւ�炸�Aif�����ɓ���܂���j
															//�i�����F�s�[�X0�̒��_0�Ƙg��0�̒��_0���w�Ǔ����ʒu�ɂ���̂ŁA�s�[�X0�̒��_0���g��0�̊O�ɂ���Ɣ��肵�Ă��܂��j
	return true;
}
void AllSearch::solve() {
	dfs(0, pieces.size());
}

//���C��
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 600, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	//����
	static AllSearch solver;
	solver.input("Problem/TR1.txt");	//���s�ړ�(T)�E��](R)�݂̂̃P�[�X
	solver.solve();
	solver.print("Answer/TR1.txt");

	// while(����ʂ�\��ʂɔ��f,���b�Z�[�W����,��ʃN���A, ESC�ŏI��)
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		solver.draw(false);	//isputted[i] = false�ƂȂ��Ă���Ƃ��A�s�[�Xi�͕`�悵�Ȃ����[�h�B
	}
	DxLib_End();
	return 0;
}

