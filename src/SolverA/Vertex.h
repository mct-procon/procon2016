//���_�N���X, tuple�ł��ǂ��������A�ێ琫�������Ȃ肻���Ȃ̂Œǉ������B�P�ɁA�ǂ̑��p�`�̂ǂ̒��_�����L�^���Ă���B
#pragma once
#include "STL.h"

class Vertex {
public:
	bool is_piece;
	int poly_id;
	int vertex_id;

	Vertex();
	Vertex(bool is_piece, int poly_id, int vertex_id);
	bool operator<(const Vertex &r) const { return is_piece < r.is_piece; }
};
