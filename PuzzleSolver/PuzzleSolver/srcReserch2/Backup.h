#pragma once
#include "Poly.h"

//状態
class State {
public:
	vector<Poly> pieces;
	vector<Poly> wakus;
	double eval;

	State(vector<Poly> pieces, vector<Poly> wakus) {
		this->pieces = pieces;
		this->wakus = wakus;
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