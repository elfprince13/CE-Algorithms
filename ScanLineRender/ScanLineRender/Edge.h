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

typedef enum {START = 0, END = 1} EndPoint;

typedef struct {
	Point coords[2];
} Edge;

#define INIT_EDGE(ed, sv, ev) \
(ed).coords[START] = (sv); \
(ed).coords[END] = (ev)

void flip(Edge * );
void flipped(const Edge *e, Edge * o);

int16_t dot(const Edge *, const Edge *);

#endif /* Edge_h */
