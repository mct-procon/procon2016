#include "Line.h"

//ベクトルuとベクトルvの外積の符号付きサイズ = u.real * v.imag - u.imag * v.realを返す.
double Line::cross(Point &u, Point &v) {
	return imag(conj(u) * v);
}