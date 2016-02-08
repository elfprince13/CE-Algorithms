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
#include "Transformation.h"
#include "Colors.h"
#include "debugConfig.h"

typedef struct {
	Point** boundary;
	size_t arity; /* boundary has arity+1 elements, to make encoding a line-loop a bit easier */
	Color color;
} Primitive;

#define INIT_PRIM(p, cv, av, bv) \
(p).boundary = (bv); \
(p).arity = (av); \
(p).color = (cv)

void makeLine(Primitive *o, Color c, const Edge e);
void makeTri(Primitive *o, Color c, const Edge e1, const Edge e2, const Edge e3);
void makeQuad(Primitive *o, Color c, const Edge e1, const Edge e2, const Edge e3, const Edge e4);

float getZForXY(const Primitive *p, float x, float y);
void transformPrimitive(const Transformation * txForm, const Primitive *p, Primitive *o);

int32_t hashPrim(const Primitive *p);



#endif /* Primitive_h */
