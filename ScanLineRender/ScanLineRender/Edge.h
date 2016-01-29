/*
//  Edge.h
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#ifndef Edge_h
#define Edge_h

#include "Point.h"
#include <stdbool.h>

typedef enum {START = 0, END = 1} EndPoint;

typedef struct {
	Point coords[2];
} Edge;

#define INIT_EDGE(ed, sv, ev) \
(ed).coords[START] = (sv); \
(ed).coords[END] = (ev)

void flip(Edge * );
void flipped(const Edge *e, Edge * o);

int32_t dot(const Edge *, const Edge *);
struct _Projection;
void projectEdge(const struct _Projection * proj, const Edge *e, Edge *o);
bool contains(const Edge *, const Point *);

#endif /* Edge_h */
