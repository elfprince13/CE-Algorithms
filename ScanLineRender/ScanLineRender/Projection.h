//
//  Projection.h
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
//

#ifndef Projection_h
#define Projection_h

#include "Point.h"
#include "Edge.h"

typedef void(*Projection)(const Point *p, Point *o, const void * state);
typedef struct {
	Projection l;
	void * lS;
	Projection r;
	void * rS;
} ComposeProj;

typedef struct {
	size_t zeroCoord;
	int16_t planeValue;
} OntoProj;

void identProj(const Point *p, Point *o, const void *);
void orthoProj(const Point *p, Point *o, const Edge * viewportSpan);
void composeProj(const Point *p, Point *o, const ComposeProj * composition);
void ontoProj(const Point *p, Point *o, const OntoProj * zOff);
void scaleProj(const Point *p, Point *o, const int16_t * scale);

Projection identity = &identProj;
Projection orthographic = (Projection)(&orthoProj);
Projection compose = (Projection)(&composeProj);
Projection onto = (Projection)(&ontoProj);
Projection scale = (Projection)(&scaleProj);

#endif /* Projection_h */
