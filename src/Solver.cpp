//とりあえず、置けるところから置いていくソルバーです。
//実は枠穴も動かせるんだけど、なるべく処理を簡単にしたいので、ピースだけ動かしています。

#include "Solver.h"

void capture(Solver shot);	//スクリーンショットを撮ります。Main.cppに定義。

bool Solver::connect() {
	for (int i = 0; i < wakus.size(); i++) {
		for (int j = 0; j < pieces.size(); j++) {
			if (pieces[j].is_putted) continue;
			//ピースj→枠i
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