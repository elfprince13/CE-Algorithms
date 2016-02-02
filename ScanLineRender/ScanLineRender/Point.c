/*
//  Point.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "Point.h"

const Point origin(){
	const Point o = {0, 0, 0};
	return o;
}

void negate(Point *p){
	p->x *= -1;
	p->y *= -1;
	p->z *= -1;
}

void negated(const Point *p, Point *o){
	o->x = -p->x;
	o->y = -p->y;
	o->z = -p->z;
}

bool pointsEqual(const Point *p, const Point *q){
	return (p->x == q->x)
	&& (p->y == q->y)
	&& (p->z == q->z);
}

float dot(const Point *p, const Point *q){
	return p->x * q->x + p->y * q->y + p->z * q->z;
}