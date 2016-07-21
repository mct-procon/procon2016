//頂点クラス, tupleでも良かったが、保守性が悪くなりそうなので追加した。単に、どの多角形のどの頂点かを記録している。
#pragma once
#include "STL.h"

class Vertex {
public:
	bool is_piece;
	int poly_id;
	int vertex_id;

	Vertex();
	Vertex(bool is_piece, int poly_id, int vertex_id);
};
