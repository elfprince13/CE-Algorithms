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
#include "debugConfig.h"

typedef uint16_t Color;

#ifndef NDEBUG
const char * fmtColor(Color c);
#endif

typedef struct {
	Edge ** boundary;
	size_t arity;
	Color color;
} Primitive;

#define INIT_PRIM(p, cv, av, bv) \
(p).boundary = (bv); \
(p).arity = (av); \
(p).color = (cv)

void makeLine(const Edge *e, Primitive *o);
void makeTri(const Edge *e1, const Edge *e2, const Edge *e3, Primitive *o);
void makeQuad(const Edge *e1, const Edge *e2, const Edge *e3, const Edge *e4, Primitive *o);

float getZForXY(const Primitive *p, float x, float y);
void transformPrimitive(const Transformation * txForm, const Primitive *p, Primitive *o);

int32_t hashPrim(const Primitive *p);



#endif /* Primitive_h */
