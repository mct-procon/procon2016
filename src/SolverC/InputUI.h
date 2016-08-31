#pragma once
#include "DxLib.h"
#include "STL.h"
using namespace std;

class InputUI {
	char bkey[256], key[256];
	int bPush, push;
	int bmouseX, bmouseY;
	int mouseX, mouseY;

public:
	InputUI() {
		for (int i = 0; i < 256; i++) key[i] = 0;
		push = 0;
	}

	void update() {
		memcpy(bkey, key, 256);
		bPush = push;
		bmouseX = mouseX;
		bmouseY = mouseY;
		GetHitKeyStateAll(key);
		push = GetMouseInput() & MOUSE_INPUT_LEFT;
		GetMousePoint(&mouseX, &mouseY);
	}

	bool isClick() {
		return (bPush == 0 && push == 1);
	}

	bool isPush(int key_code) {
		return (bkey[key_code] == 0 && key[key_code] == 1);
	}

	complex<double> getMousePoint() {
		return complex<double>(mouseX, mouseY);
	}

	bool isDrugStart() {
		return bPush && push && (abs(mouseX - bmouseX) + abs(mouseY - bmouseY) >= 5);
	}

	bool isDrugEnd() {
		return (push == 0);
	}
};