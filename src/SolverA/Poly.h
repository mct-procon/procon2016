//‘½ŠpŒ`ƒNƒ‰ƒX‚ÌéŒ¾

#pragma once
#include "Point.h"

class Poly {
private:
	Point point0;
	vector<Point> point;
	Poly *left_child, *right_child;

public:
	//‰Šú‰»
	Poly(Point point0, vector<Point> point);
	Poly();

	//getter
	int size();
	Point get_point(int id);
	double get_angle_deg(int id);

	//setter
	void move(int id, Point s, Point e);
};
