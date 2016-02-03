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

typedef Point*(Edge)[2];

#define INIT_EDGE(ed, sv, ev) \
(ed).coords[START] = (sv); \
(ed).coords[END] = (ev)

#define HASH_EDGE(e) (HASH_POINT(*((e)[START])) ^ HASH_POINT(*((e)[END])))

void flip(Edge * );
void flipped(const Edge *e, Edge * o);

float dotEdge(const Edge *, const Edge *);
struct _Projection;
void projectEdge(const struct _Projection * proj, const Edge *e, Edge *o);
bool contains(const Edge *, const Point *);

#endif /* Edge_h */
