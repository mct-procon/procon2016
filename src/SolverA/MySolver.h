#include "Solver.h"

class MySolver : public Solver {
private:
	queue<Vertex> standard_point;

	bool is_combi_vertex(Vertex piece_v, Vertex dst, double error_dist, double error_angle);	//���_���������Ă��邩�H (�������Ă����true��Ԃ��j
	int get_combi_level(Vertex src, double error_dist, double error_angle);						//�����x�v�Z�i���j
	Vertex get_best_vertex(Vertex dst);															//���_dst�ƍł��������钸�_src��Ԃ��i���j
	void update(int piece_id);																	//�s�[�Xpiece_id�̈ʒu���m�肷��Ƃ��̍X�V

public:
	void solve();	//���������܂�
};