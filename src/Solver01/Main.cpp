//まずは1つのファイルでソルバーを実装してみます。
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
	double distance(Point &r) { return sqrt((r.x - x) * (r.x - x) + (r.y - y) * (r.y - y)); }				//点(x, y)から点(r.x, r.y)までの距離を返す
	double angle() { double ang = atan2(y, x); /*値域(-π, π]*/ if (ang < 0) ang += 2 * PAI; return ang; }	//位置ベクトル(x, y)の角度 (ラジアン)を返す, 値域は[0, 2π)
};

class Poly {
private:
	Point point0;					//point0          = 頂点0の座標
	vector<Point> points;			//points[i]       = 頂点0に対する頂点iの座標
	vector<double> edges_length;	//edges_length[i] = 辺iの長さ。辺i = 頂点iを始点, 頂点i+1を終点とする有向線分。
	vector<double> edges_angle;		//edges_angle[i]  = 辺iの角度（ラジアン）。値域は[0, 2π)
	vector<double> points_angle;	//points_angle[i] = 頂点iの内角（ラジアン）。値域は[0, 2π)

	bool line_ishit(Point a, Point b, Point c, Point d);	//線分a->bと線分c->dの交差判定. T字交差はfalseとみなす
	void translate(Point t);
	void rotate(double angle);

public:
	Point get_point(int point_id);
	int get_point_num();
	double get_point_angle(int point_id);
	Poly ();
	Poly (Point point0, vector<Point> points);
	void move(int point_id, Point s, Point e);
	int calc_combi_level(Poly &r, double angle_eps, double position_eps, int start_a, int start_b, int dir);
	bool ishit(Poly &r);
	bool iscover(Poly &r);
	bool isoverlap(Poly &r);
};

class Solver {
protected:
	vector<Poly> pieces;
	vector<Poly> wakus;		//wakus[id] = id(>=0)番の枠穴
	vector<bool> isputted;	//isputted[id] = ピースidを置いたならtrue, 置いてないならfalse

	bool canput(Poly &piece);

public:
	void input(string filename);
	void print(string filename);
	void draw(bool isdraw_didnot_put);
};

class TestSolver : public Solver {
private:

public:
	void solve();
};

/*実装 Polyクラス*/

//点point_idの絶対座標の取得
Point Poly::get_point(int point_id) {
	return points[point_id] + point0;
}

//頂点数を取得
int Poly::get_point_num() {
	return points.size();
}

//頂点角（ラジアン）を取得
double Poly::get_point_angle(int point_id) {
	return points_angle[point_id];
}

//Please read this homepage : http://www5d.biglobe.ne.jp/%7Etomoya03/shtml/algorithm/Intersection.htm
//線分の交差判定（T字交差はfalse）点(x1,y1), (x2,y2)を通る直線の式：(x1-x2)*(y-y1)+(y1-y2)*(x1-x)=0を使用。
bool Poly::line_ishit(Point a, Point b, Point c, Point d) {
	double ta = (c.x - d.x) * (a.y - c.y) + (c.y - d.y) * (c.x - a.x);
	double tb = (c.x - d.x) * (b.y - c.y) + (c.y - d.y) * (c.x - b.x);
	double tc = (a.x - b.x) * (c.y - a.y) + (a.y - b.y) * (a.x - c.x);
	double td = (a.x - b.x) * (d.y - a.y) + (a.y - b.y) * (a.x - d.x);
	double eps = 0.01;
	return ta * tb < 0 && tc * td < 0;	// 右辺(0)の値を小さくするとfalseを返しやすくなる。大きくするとtrueを返しやすくなる。0にすると（接触をfalseとみなす）厳密な交差判定になる。
}

//(t.x, t.y)だけ平行移動する
void Poly::translate(Point t) {
	point0.x += t.x;
	point0.y += t.y;
}

//angle[ラジアン]だけ回転する。angleの値域：[-2π, 2π]
void Poly::rotate(double angle) {
	for (int id = 0; id < points.size(); id++) {
		double nx = cos(angle) * points[id].x - sin(angle) * points[id].y;
		double ny = sin(angle) * points[id].x + cos(angle) * points[id].y;
		points[id].x = nx;
		points[id].y = ny;
		//edges_angle[id] += angle;
		//while (edges_angle[id] < 0) edges_angle[id] += 2 * PAI;
		//while (edges_angle[id] >= 2 * PAI) edges_angle[id] -= 2 * PAI;
	}
}

//初期化（何もしないやつ）
Poly::Poly () {
}

//初期化（辺の長さ, 角度なども求める)
Poly::Poly (Point point0, vector<Point> points) {
	this->point0 = point0;
	this->points = points;

	//引数のpointsを使う
	int n = points.size();		//頂点数
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

//頂点point_idの絶対座標がsになるように平行移動し、頂点point_id -> point_id + 1を結ぶ有向線分がs->eと同じ向きになるように回転する
void Poly::move(int point_id, Point s, Point e) {
	Point a = get_point(point_id);
	Point b = get_point((point_id + 1) % get_point_num());

	//頂点0周りの回転をする
	rotate((e - s).angle() - (b - a).angle());

	//頂点point_idの回転後座標を取得して、平行移動する
	a = get_point(point_id);
	translate(s - a);
}

//多角形rとの結合度を計算する。結合度が大きい程、くっつく可能性が高い。angle_eps, position_epsを大きくすると「アバウト」になる（結合度が上がりやすくなる）。
//start_a, start_b…多角形thisの頂点start_a, 多角形rの頂点start_bから探索開始。
//dir…ピース同士の結合度判定なら-1, 枠とピースの結合度判定なら1を指定する。
int Poly::calc_combi_level(Poly &r, double angle_eps, double position_eps, int start_a, int start_b, int dir) {
	int weight_edge = 1;	//辺の一致によって加算されるスコア（の半分）
	int weight_angle = 2;	//角度の一致（足して180°, 360°。引いて0°など）によって加算されるスコア
	int score = 0;			//スコア, すなわち結合度
	int n = this->get_point_num();
	int m = r.get_point_num();
	int id_as, id_bs;

	id_as = start_a;
	id_bs = start_b;

	while (this->get_point(id_as).distance(r.get_point(id_bs)) <= position_eps) {
		int id_ae = (id_as + 1) % n;
		int id_be = (id_bs + dir + m) % m;

		Point a = this->get_point(id_as);
		Point b = this->get_point(id_ae);
		Point c = r.get_point(id_bs);
		Point d = r.get_point(id_be);

		//頂点a, cの内角の結合を見る
		if (dir == -1) {
			double deg = this->get_point_angle(id_as) + r.get_point_angle(id_bs); deg *= 180; deg /= PAI;
			if (180 - angle_eps <= deg && deg <= 180 + angle_eps) score += weight_angle;
			else if (360 - angle_eps <= deg && deg <= 360 + angle_eps) score += weight_angle;
		}
		else {
			double deg = this->get_point_angle(id_as) - r.get_point_angle(id_bs); deg *= 180; deg /= PAI;
			if (-angle_eps <= deg && deg <= angle_eps) score += weight_angle;
		}

		//辺a -> b, c -> dの結合を見る
		double deg = (b - a).angle() - (d - c).angle(); deg *= 180; deg /= PAI;
		if (-angle_eps <= deg && deg <= angle_eps) score += weight_edge;		//ほぼ平行なら加点
		if (b.distance(d) <= position_eps) score += weight_edge;				//辺の終点が（ほぼ）一致してたらさらに加点

		//カウンター更新
		id_as = (id_as + 1) % n;
		id_bs = (id_bs + dir + m) % m;
	}
	return score;
}

//多角形rと衝突しているかを判定する。衝突していればtrueを返す。(接している場合はfalse)
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

//多角形rを含むかを判定する。多角形rのある頂点と多角形thisのある辺が近い場合の動作は未定義。
bool Poly::iscover(Poly &r) {
	int i, j;

	for (i = 0; i < r.points.size(); i++) {
		//r.points[i]が多角形に内部にあるかを判定。内部になければ, その時点でfalseを返す
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

//多角形rと重なるかを判定する。重なるならtrueを返す。境界上はfalse
bool Poly::isoverlap(Poly &r) {
	if (ishit(r)) return true;
	if (iscover(r)) return true;
	if (r.iscover(*this)) return true;
	return false;
}

/*実装 Solverクラス (*solve()関数は子クラスで実装します)*/

//既に置いたピースとpieceが重ならず、かつ、全ての枠穴がpieceを内包していればtrueを返す。
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

//ファイルからデータを入力し、各種変数を初期化する
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
		//X座標
		for (int j = 0; j < point_num; j++) {
			ifs >> points[j].x;
		}
		//Y座標
		for (int j = 0; j < point_num; j++) {
			ifs >> points[j].y;
		}
		//相対座標形式にする
		point0 = points[0];
		for (int j = 0; j < point_num; j++) {
			points[j] = points[j] - point0;
		}
		//枠穴を追加
		wakus.push_back(Poly(point0, points));
	}

	ifs >> piece_num;
	for (int i = 0; i < piece_num; i++) {
		Point point0;
		vector<Point> points;

		ifs >> point_num;
		points.resize(point_num);
		//X座標
		for (int j = 0; j < point_num; j++) {
			ifs >> points[j].x;
		}
		//Y座標
		for (int j = 0; j < point_num; j++) {
			ifs >> points[j].y;
		}
		//相対座標形式にする
		point0 = points[0];
		for (int j = 0; j < point_num; j++) {
			points[j] = points[j] - point0;
		}
		//ピースを追加
		pieces.push_back(Poly(point0, points));
	}
	
	//フラグ
	isputted.resize(piece_num);
	for (int i = 0; i < piece_num; i++) {
		isputted[i] = false;
	}
	ifs.close();
}

//ファイルへ答えを出力する
void Solver::print(string filename) {
	ofstream ofs(filename);
	
	ofs << wakus.size() << endl;
	for (int i = 0; i < wakus.size(); i++) {
		int n = wakus[i].get_point_num();
		//頂点数
		ofs << n << endl;
		//X座標
		ofs << wakus[i].get_point(0).x;
		for (int j = 1; j < n; j++) {
			ofs << " " << wakus[i].get_point(j).x;
		}
		ofs << endl;
		//Y座標
		ofs << wakus[i].get_point(0).y;
		for (int j = 1; j < n; j++) {
			ofs << " " << wakus[i].get_point(j).y;
		}
		ofs << endl;
		//改行
		ofs << endl;
	}

	ofs << pieces.size() << endl;
	for (int i = 0; i < pieces.size(); i++) {
		int n = pieces[i].get_point_num();
		//頂点数
		ofs << n << endl;
		//X座標
		ofs << pieces[i].get_point(0).x;
		for (int j = 1; j < n; j++) {
			ofs << " " << pieces[i].get_point(j).x;
		}
		ofs << endl;
		//Y座標
		ofs << pieces[i].get_point(0).y;
		for (int j = 1; j < n; j++) {
			ofs << " " << pieces[i].get_point(j).y;
		}
		ofs << endl;
		//改行
		ofs << endl;
	}
}

//枠とピースの描画
void Solver::draw(bool isdraw_didnot_put) {
	int i, j;

	for (i = 0; i < wakus.size(); i++) {
		for (j = 0; j < wakus[i].get_point_num(); j++) {
			Point s = wakus[i].get_point(j);
			Point e = wakus[i].get_point((j + 1) % wakus[i].get_point_num());
			DrawLine((int)s.x, (int)s.y, (int)e.x, (int)e.y, GetColor(0, 255, 0), 2);
			DrawFormatString((int)s.x, (int)s.y, GetColor(255, 0, 0), "%d", j);
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

/*実装 TestSolverクラス*/

//結合度Maxの組み合わせで枠にくっつけていく (動かす→評価, ではなく, 評価→動かす, にしたほうが速いかも）
void TestSolver::solve() {
	int i, j, k;

	for (i = 0; i < wakus[0].get_point_num(); i++) {
		//枠0の辺iと最も結合する「ピース番号j, ピースの辺番号k」を探す
		int max_score = -1;
		int best_j, best_k;
		Point s = wakus[0].get_point(i);
		Point e = wakus[0].get_point((i + 1) % wakus[0].get_point_num());

		for (j = 0; j < pieces.size(); j++) {
			if (isputted[j]) { continue; }
			for (k = 0; k < pieces[j].get_point_num(); k++) {
				//ピースjを動かす
				pieces[j].move(k, s, e);
				
				//スコア計算
				int score = pieces[j].calc_combi_level(wakus[0], 5, 5, k, i, 1);

				if (max_score < score) {
					max_score = score;
					best_j = j;
					best_k = k;
				}
			}
		}
		if (max_score == -1) { continue; }

		//最も結合するピース（の辺）を枠（の辺）にくっつける
		pieces[best_j].move(best_k, s, e);
		printfDx("ピース番号 = %d ピース辺 = %d スコア = %d\n", best_j, best_k, max_score);
		isputted[best_j] = true;
	}
}

//メイン
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	SetGraphMode(800, 600, 32);
	SetBackgroundColor(255, 255, 255);
	ChangeWindowMode(TRUE);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	//解く
	static TestSolver solver;
	solver.input("Problem/TR1.txt");	//平行移動(T)・回転(R)のみのケース
	solver.solve();
	solver.print("Answer/TR1.txt");

	// while(裏画面を表画面に反映,メッセージ処理,画面クリア, ESCで終了)
	while (ScreenFlip()==0 && ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0) {
		solver.draw(true);	//isputted[i] = falseとなっているとき、ピースiは描画しないモード。
	}
	DxLib_End();
	return 0;
}

