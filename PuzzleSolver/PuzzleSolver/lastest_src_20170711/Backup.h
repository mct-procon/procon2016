#pragma once
#include "Poly.h"
#include "Solver.h"

//状態
class State {
public:
	vector<Poly> pieces;
	vector<Poly> wakus;

	//ソルバクラス自体をバックアップできなかったので、汚いけど無理やりこうする。
	vector<int> lastestMargedPolyIds;
	map<DstType, vector<MoveNode> > scores;
	double eval;

	State(vector<Poly> pieces, vector<Poly> wakus, vector<int> lastestMargedPolyIds, map<DstType, vector<MoveNode>> scores) {
		this->pieces = pieces;
		this->wakus = wakus;
		this->lastestMargedPolyIds = lastestMargedPolyIds;
		this->scores = scores;
	}
};

//バックアップ
class Backup {
private:
	vector<State> states;

public:
	void push(State state_before_move) {
		states.push_back(state_before_move);
	}
	State pop() {
		if (states.size() <= 1) return *states.rbegin();
		states.pop_back();
		State ret = *states.rbegin();
		return ret;
	}
	void clear() {
		states.clear();
	}
};