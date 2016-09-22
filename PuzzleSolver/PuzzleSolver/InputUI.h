#pragma once
#include "DxLib.h"
#if _MSC_VER <= 1600
#include "STL.h"
#endif
using namespace std;

class InputUI {
	char bkey[256], key[256];
	int bClick, click;
	int mouseX, mouseY;

public:
	InputUI() {
		for (int i = 0; i < 256; i++) key[i] = 0;
		click = 0;
	}

	void update() {
		memcpy(bkey, key, 256);
		bClick = click;
		GetHitKeyStateAll(key);
		click = GetMouseInput() & MOUSE_INPUT_LEFT;
		GetMousePoint(&mouseX, &mouseY);
	}

	bool isClick() {
		return (bClick == 0 && click == 1);
	}

	bool isPush(int key_code) {
		return (bkey[key_code] == 0 && key[key_code] == 1);
	}

	complex<double> getMousePoint() {
		return complex<double>(mouseX, mouseY);
	}
};