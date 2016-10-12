#include "SolverBase.h"
#include "Extern.h"

//���p�`��ǂݍ���
Poly SolverBase::input_poly(ifstream &ifs, bool is_clockwise) {
	int n;			//���_��
	int poly_id;	//���p�`�ɂ͂�ꂽ�V�[���̔ԍ� (�Ȃ����-1�j
	vector<Point> points;	//���p�`�̒��_��
	vector<Line> lines;		//�\���������

	{
		string tag_num;
		ifs >> tag_num;
		int i; for (i = (int)tag_num.size() - 1; i >= 0; i--) if (tag_num[i] == ':') break; i++;
		//i�����ځ`�̐��l�� (��̌����珇��) �擾����
		n = 0;
		for (int j = i; j < (int)tag_num.size(); j++) { n *= 10; n += tag_num[j] - '0'; }
		poly_id = 0;
		if (tag_num[0] == '-') { poly_id = -1; }
		else { for (int j = 0; j < i - 1; j++) { poly_id *= 10; poly_id += tag_num[j] - '0'; } }
		//printfDx("%s %d %d %d", tag_num, tag_num.size(), n, poly_id);
	}

	for (int i = 0; i < n; i++) {
		double x, y;
		ifs >> x >> y;
		points.push_back(Point(x, y));
	}
	if (points.size() == 0) { return Poly(); }
	points.push_back(points[0]);

	for (int i = 0; i < n; i++) {
		lines.push_back(Line(points[i], points[i + 1]));
		lines[i].set_tag(poly_id, i);
	}
	Poly ret = Poly(points, lines);

	if (is_clockwise && ret.area() > 0) ret.point_reverse();
	if (!is_clockwise && ret.area() < 0) ret.point_reverse();
	return ret;
}


//�����蔻��p�̗֊s��Ԃ��B
//dist > 0�̂Ƃ��c�s�[�X�̏ꍇ�͓����蔻��g�������ɂł���B�g�̏ꍇ�͓����蔻��g���O���ɂł���B
Poly SolverBase::scaledown_poly(Poly &poly, double dist) {
	int i;
	int n = poly.size();
	vector<Line> lines(n);
	vector<Point> points(n + 1);

	//���p�`�����ɐ���������
	for (i = 0; i < n; i++) {
		lines[i] = Line(poly.points[i], poly.points[i + 1]);
		lines[i].move(-dist);
	}
	for (i = 0; i < n; i++) {
		int id1 = i - 1; if (id1 < 0) id1 += n;
		int id2 = i + 1; if (id2 >= n) id2 -= n;
		lines[i].cut(lines[id1], lines[id2]);	//lines[i].s <- lines[id1]�Ƃ̌�_, lines[i].e <- lines[id2]�Ƃ̌�_
	}

	//���p�`���\��
	for (i = 0; i < n; i++) {
		points[i] = lines[i].s;
	}
	points[n] = points[0];
	return Poly(points, lines);		//��Q�����͉��ł��ǂ�
}

//�t�@�C���o��
void SolverBase::print_poly(ofstream & ofs, Poly poly)
{
	for (int i = 0; i < poly.size(); i++) {
		ofs << poly.points[i].real() << " " << poly.points[i].imag() << endl;
	}
}


//���� (�V�����t�H�[�}�b�g�œǂ�, �^�O����Ȃ������ɑΉ����܂�)
void SolverBase::input(string filename)
{
	ifstream ifs(filename); if (ifs.fail()) return;
	int poly_num;

	ifs >> poly_num;
	for (int i = 0; i < poly_num; i++) { wakus.push_back(input_poly(ifs, true)); }
	init_wakus = wakus;

	ifs >> poly_num;
	for (int i = 0; i < poly_num; i++) { pieces.push_back(input_poly(ifs, false)); }
	ifs.close();

	//�p�����[�^�̐ݒ�
	parameter.dist_error_meter = 1.2 * 0.001;
	parameter.dist_error_waku_meter = 2.0 * 0.001;
	parameter.pixel_per_meter = 20000 / 2.54;		//200[dpi]�̏ꍇ
}


//�����ł͗�O�I�ɁA�����o�[�ϐ��𒼐ڑ��삵�Ă��܂��B
void SolverBase::�]���Ȓ��_����菜��(double dist_error) {
	int i, j;

	for (i = 0; i < pieces.size(); i++) {
		int poly_id = pieces[i].lines[0].poly_id();

		while (true) {
			for (j = 0; j < pieces[i].size(); j++) {
				int a = j - 1 + ((j == 0) * pieces[i].size());
				int b = j;
				int c = j + 1;
				Line line = Line(pieces[i].points[a], pieces[i].points[c]);

				if (line.dist(pieces[i].points[b]) <= dist_error) {
					break;
				}
			}
			if (j == pieces[i].size()) { break; }	//�I��
			//���_j����菜����, �ǂ���
			vector<Point> cycle;
			int id = j;
			for (j = 0; j < pieces[i].size(); j++) if (j != id) cycle.push_back(pieces[i].points[j]);
			cycle.push_back(cycle[0]);
			pieces[i].points = cycle;
		}
		vector<Line> lines;
		for (j = 0; j < pieces[i].size(); j++) { lines.push_back(Line(pieces[i].points[j], pieces[i].points[j + 1])); }
		pieces[i] = Poly(pieces[i].points, lines);
		//�^�O�t��
		for (j = 0; j < pieces[i].lines.size(); j++) { pieces[i].lines[j].set_tag(poly_id, j); }
	}

	for (i = 0; i < wakus.size(); i++) {
		while (true) {
			for (j = 0; j < wakus[i].size(); j++) {
				int a = j - 1 + ((j == 0) * wakus[i].size());
				int b = j;
				int c = j + 1;
				Line line = Line(wakus[i].points[a], wakus[i].points[c]);

				if (line.dist(wakus[i].points[b]) <= dist_error) {
					break;
				}
			}
			if (j == wakus[i].size()) { break; }	//�I��
			//���_j����菜����, �ǂ���
			vector<Point> cycle;
			int id = j;
			for (j = 0; j < wakus[i].size(); j++) if (j != id) cycle.push_back(wakus[i].points[j]);
			cycle.push_back(cycle[0]);
			wakus[i].points = cycle;
		}
		vector<Line> lines;
		for (j = 0; j < wakus[i].size(); j++) { lines.push_back(Line(wakus[i].points[j], wakus[i].points[j + 1])); }
		wakus[i] = Poly(wakus[i].points, lines);
		//�^�O�t��
		for (j = 0; j < wakus[i].lines.size(); j++) { wakus[i].lines[j].set_tag(-1, j); }
	}
}

void SolverBase::���p�`���g��(double dist_waku, double dist_piece) {
	for (int i = 0; i < wakus.size(); i++) {
		wakus[i] = scaledown_poly(wakus[i], dist_waku);

		//��
		wakus[i].lines.clear();
		for (int j = 0; j < wakus[i].size(); j++) { wakus[i].lines.push_back(Line(wakus[i].points[j], wakus[i].points[j + 1])); }
		for (int j = 0; j < wakus[i].lines.size(); j++) { wakus[i].lines[j].set_tag(-1, j); }
	}
	/*for (int i = 0; i < pieces.size(); i++) {
		int poly_id = pieces[i].lines[0].poly_id;
		pieces[i] = scaledown_poly(pieces[i], -dist_piece);

		//��
		pieces[i].lines.clear();
		for (int j = 0; j < pieces[i].size(); j++) { pieces[i].lines.push_back(Line(pieces[i].points[j], pieces[i].points[j + 1])); }
		for (int j = 0; j < pieces[i].lines.size(); j++) { pieces[i].lines[j].set_tag(poly_id, j); }
	}*/
}


//�o��
void SolverBase::print(string filename)
{
	int i, j;
	Point out_pieces_data[60][60];
	double INF = 114514;

	for (i = 0; i < 60; i++) {
		for (j = 0; j < 60; j++) {
			out_pieces_data[i][j] = Point(INF, INF);
		}
	}

	for (i = 0; i < wakus.size(); i++) {
		for (j = 0; j < wakus[i].lines.size(); j++) {
			Line line = wakus[i].lines[j];
			if (line.is_exist_tag() == false) continue;
			int piece_id = line.poly_id();
			int point_id = line.line_id();
			Point point  = line.s;
			out_pieces_data[piece_id][point_id] = point;
		}
	}
	for (i = 0; i < pieces.size(); i++) {
		for (j = 0; j < pieces[i].lines.size(); j++) {
			Line line = pieces[i].lines[j];
			if (line.is_exist_tag() == false) continue;

			int piece_id = line.poly_id();
			int point_id = line.line_id();
			Point point = line.s;
			out_pieces_data[piece_id][point_id] = point;
		}
	}

	//���K��
	vector<Poly> out_pieces_tmp;
	vector<int> out_pieces_id;
	for (i = 0; i < 60; i++) {
		Poly poly;
		for (j = 0; j < 60; j++) {
			if (out_pieces_data[i][j] == Point(INF, INF)) {
				break;
			}
			poly.points.push_back(out_pieces_data[i][j]);
		}
		if (poly.size() <= 0) continue;
		poly.points.push_back(poly.points[0]);
		out_pieces_tmp.push_back(poly);
		out_pieces_id.push_back(i);
	}

	//�\���f�[�^�̐���
	out_pieces.clear();
	out_pieces.resize(60);	//�^�O�ԍ���59�𒴂��Ȃ�
	for (i = 0; i < out_pieces_tmp.size(); i++) {
		out_pieces[out_pieces_id[i]] = out_pieces_tmp[i];
	}

	//�o�̓t�@�C���̐���
	ofstream ofs(filename);
	if (ofs.fail()) { return; }
	
	ofs << init_wakus.size() << endl;
	for (i = 0; i < init_wakus.size(); i++) {
		//�^�O�̏o��
		ofs << -1 << ":" << init_wakus[i].size() << endl;
		//���_�̏o��
		print_poly(ofs, init_wakus[i]);
	}

	ofs << out_pieces_tmp.size() << endl;
	for (i = 0; i < out_pieces_tmp.size(); i++) {
		//�^�O�̏o��
		ofs << out_pieces_id[i] << ":" << out_pieces_tmp[i].size() << endl;
		//���_�̏o��
		print_poly(ofs, out_pieces_tmp[i]);
	}
}
