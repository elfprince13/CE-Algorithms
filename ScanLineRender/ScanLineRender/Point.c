/*
//  Point.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "Point.h"

const Point origin(){
	return (Point){0, 0, 0};
}

void negate(Point *p){
	p->x *= -1;
	p->y *= -1;
	p->z *= -1;
}

const Point negated(const Point *p){
	return (Point){-p->x, -p->y, -p->z};
}