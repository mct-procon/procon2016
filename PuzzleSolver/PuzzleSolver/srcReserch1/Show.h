#pragma once
#include "Poly.h"
#include "DxLib.h"
#include "Solver.h"
#include "Extern.h"
using namespace std;

class Show {
	Solver *solver;		//���U���g�I��p
	vector<MoveNode> *move_nodes;
	int selected_id;	//���Ԗڂ̌���I�񂾂��H

public:
	void init(Solver *solver, vector<MoveNode> *move_nodes) {
		selected_id = 0;
		this->solver = solver;
		this->move_nodes = move_nodes;
	}

	//�v���Z�X���b�Z�[�W��Ԃ�
	int select_result(Solver *solver, vector<MoveNode> *move_nodes) {
		init(solver, move_nodes);

		char bkey[256];
		char key[256];
		GetHitKeyStateAll(key);

		while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0) {
			memcpy(bkey, key, 256);
			GetHitKeyStateAll(key);

			if (!bkey[KEY_INPUT_RETURN] && key[KEY_INPUT_RETURN]) { //�m��
				if (selected_id < move_nodes->size()) {
					solver->connect_hand((*move_nodes)[selected_id]);
				}
				return 0;
			}
			
			//�Z���N�g(N, M)
			bool flag = false;	//�t�@�C�����X�V���邩�H
			if (!bkey[KEY_INPUT_M] && key[KEY_INPUT_M]) {
				selected_id = (selected_id + 1) % (move_nodes->size() + 1);
				flag = true;
			}
			if (!bkey[KEY_INPUT_N] && key[KEY_INPUT_N]) {
				selected_id = (selected_id - 1 + move_nodes->size() + 1) % (move_nodes->size() + 1);
				flag = true;
			}

			//�o�b�N�X�y�[�X�Ȃ�A�I��
			if (!bkey[KEY_INPUT_BACK] && key[KEY_INPUT_BACK]) {
				flag = true;
				return 0;
			}

			//�\��
			update_draw_option(); 

			if (selected_id < move_nodes->size()) {
				solver->move((*move_nodes)[selected_id]);

				//�s�b�N�A�b�v�\��
				draw();
				int id = (*move_nodes)[selected_id].src_poly_id;
				for (int i = 0; i < pieces[id].size(); i++) {
					Point s = to_draw_point(pieces[id].points[i]);
					Point e = to_draw_point(pieces[id].points[i + 1]);
					DrawLine((int)s.real(), (int)s.imag(), (int)e.real(), (int)e.imag(), GetColor(0, 100, 255), 3);
				}

				if (flag) solver->print("answer.txt");
				DrawFormatString(100, 100, GetColor(255, 0, 0), "Show Result!");
				solver->restore();

				solver->eval_obj.draw_histgram(0.01, 0.01);
			}
			else {
				draw();
			}
		}
		return 1;
	}
};