//’¸“_ƒNƒ‰ƒX‚ÌÀ‘•.
#include "Vertex.h"
#include "Point.h"

Vertex::Vertex() {
}

Vertex::Vertex(bool is_piece, int poly_id, int vertex_id) {
	this->is_piece = is_piece;
	this->poly_id = poly_id;
	this->vertex_id = vertex_id;
}