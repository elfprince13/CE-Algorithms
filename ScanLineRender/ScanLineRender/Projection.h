/*
//  Projection.h
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#ifndef Projection_h
#define Projection_h

#include "Point.h"
#include "Edge.h"

typedef void(*ProjectionF)(const Point *p, Point *o, const void * state);
typedef struct _Projection {
	ProjectionF f;
	void * state;
} Projection;

typedef struct {
	Projection l;
	Projection r;
} ComposeProj;

typedef struct {
	size_t zeroCoord;
	int32_t planeValue;
} OntoProj;

void identProj(const Point *p, Point *o, const void *);
void orthoProj(const Point *p, Point *o, const Edge * viewportSpan);
void composeProj(const Point *p, Point *o, const ComposeProj * composition);
void ontoProj(const Point *p, Point *o, const OntoProj * zOff);
void scaleProj(const Point *p, Point *o, const int32_t * scale);

extern const ProjectionF identity;
extern const ProjectionF orthographic;
extern const ProjectionF compose;
extern const ProjectionF onto;
extern const ProjectionF scale;


#endif /* Projection_h */
