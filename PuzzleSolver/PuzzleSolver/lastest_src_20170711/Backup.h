#pragma once
#include "Poly.h"
#include "Solver.h"

//���
class State {
public:
	vector<Poly> pieces;
	vector<Poly> wakus;

	//�\���o�N���X���̂��o�b�N�A�b�v�ł��Ȃ������̂ŁA�������ǖ�����肱������B
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

//�o�b�N�A�b�v
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