//�Ƃ肠�����A�u����Ƃ��납��u���Ă����\���o�[�ł��B
//���͘g������������񂾂��ǁA�Ȃ�ׂ��������ȒP�ɂ������̂ŁA�s�[�X�����������Ă��܂��B

#include "Solver.h"

void capture(Solver shot);	//�X�N���[���V���b�g���B��܂��BMain.cpp�ɒ�`�B

bool Solver::connect() {
	for (int i = 0; i < wakus.size(); i++) {
		for (int j = 0; j < pieces.size(); j++) {
			if (pieces[j].is_putted) continue;
			//�s�[�Xj���gi
			for (int tyotenA = 0; tyotenA < wakus[i].size(); tyotenA++) {
				for (int tyotenB = 0; tyotenB < pieces[j].size(); tyotenB++) {
					bool backup = wakus[i].is_putted;
					wakus[i].is_putted = true;
					move(pieces[j], tyotenB, false, wakus[i], tyotenA, false);
					if (!canput(pieces[j])) { wakus[i].is_putted = backup; continue; }
					pieces[j].is_putted = true;
					printf("%d\n", j);
					return true;
				}
			}
		}
	}
	return false;
}

void Solver::solve() {
	capture(*this);
	while (connect()) {
		capture(*this);
	}
	make_answer();
	capture(*this);
}