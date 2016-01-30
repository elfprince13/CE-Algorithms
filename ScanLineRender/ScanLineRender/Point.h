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
#include <stdbool.h>
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

typedef struct {
	int32_t x; int32_t y; int32_t z;
} Point;
#define INIT_POINT(p, xv, yv, zv) \
(p).x = (xv); \
(p).y = (yv); \
(p).z = (zv)

#define HASH_POINT(p) ((p).x ^ (p).y ^ (p).z)

const Point origin(void);

void negate(Point *);
void negated(const Point *p, Point *o);

bool pointsEqual(const Point *p, const Point *q);

#define DOT(p, q) (((p).x * (q).x) + ((p).y * (q).y) + ((p).z * (q).z));
int32_t dot(const Point *p, const Point *q);

#endif /* Point_h */
