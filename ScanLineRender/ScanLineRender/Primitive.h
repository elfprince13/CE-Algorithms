/*
//  Primitive.h
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#ifndef Primitive_h
#define Primitive_h

#include "Edge.h"
#include "Projection.h"

typedef uint32_t Color;

const char * fmtColor(Color c);

typedef struct {
	Color color;
	uint32_t arity;
	Edge * boundary;
} Primitive;

#define INIT_PRIM(p, cv, av, bv) \
(p).color = (cv); \
(p).arity = (av); \
(p).boundary = (bv)

void makeLine(const Edge *e, Primitive *o);
void makeTri(const Edge *e1, const Edge *e2, const Edge *e3, Primitive *o);
void makeQuad(const Edge *e1, const Edge *e2, const Edge *e3, const Edge *e4, Primitive *o);

const int32_t getZForXY(const Primitive *p, const int32_t x, const int32_t y);
void projectPrimitive(const Projection * proj, const Primitive *p, Primitive *o);


#endif /* Primitive_h */
