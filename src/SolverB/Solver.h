#pragma once
#include "SolverBase.h"

class Solver : public SolverBase {
private:
	bool connect();

public:
	void solve();		//–â‘è‚ð‰ð‚«‚Ü‚·
};