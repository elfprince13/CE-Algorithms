//
//  Primitive.h
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
//

#ifndef Primitive_h
#define Primitive_h

#include <stdio.h>
#include "Edge.h"

typedef uint16_t Color;

typedef struct {
	Color color;
	uint16_t arity;
	Edge * boundary;
} Primitive;

inline Primitive makeLine(const Edge *e){
	Edge * b = malloc(sizeof(Edge));
	b[0] = *e;
	return (Primitive){0, 1, b};
}
inline Primitive makeTri(const Edge *e1, const Edge *e2, const Edge *e3){
	Edge * b = malloc(3*sizeof(Edge));
	b[0] = *e1;
	b[1] = *e2;
	b[2] = *e3;
	return (Primitive){0, 3, b};
}
inline Primitive makeQuad(const Edge *e1, const Edge *e2, const Edge *e3, const Edge *e4){
	Edge * b = malloc(4*sizeof(Edge));
	b[0] = *e1;
	b[1] = *e2;
	b[2] = *e3;
	b[3] = *e4;
	return (Primitive){0, 4, b};
}

const int16_t getZForXY(const Primitive *p, const int16_t x, const int16_t y);


#endif /* Primitive_h */
