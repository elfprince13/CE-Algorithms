/*
//  Point.h
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#ifndef Point_h
#define Point_h

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

typedef struct {
	int16_t x; int16_t y; int16_t z;
} Point;

const Point origin(void);

void negate(Point *);

const Point negated(const Point *p);

#endif /* Point_h */
