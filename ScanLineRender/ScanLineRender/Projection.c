/*
//  Projection.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "Projection.h"

void identProj(const Point *p, Point *o, const void * discard){
	*o = *p;
}

void orthoProj(const Point *p, Point *o, const Edge * viewportSpan){
	const Point * min = (*viewportSpan)[START];
	const Point * max = (*viewportSpan)[END];
	int32_t l = min->x;
	int32_t r = max->x;
	int32_t b = min->y;
	int32_t t = max->y;
	int32_t n = min->z;
	int32_t f = max->z;
	Point tmp;
	INIT_POINT(tmp,
			   (2*p->x - (l + r))/(r-l),
			   (2*p->y - (t + b))/(t-b),
			   (-2*p->z + (f + n))/(f-n));
	*o = tmp;
}


void composeProj(const Point *p, Point *o, const ComposeProj * composition){
	Point tmp;
	composition->r.f(p, &tmp, composition->r.state);
	composition->l.f(&tmp, o, composition->l.state);
}


void ontoProj(const Point *p, Point *o, const OntoProj * state){
	Point tmp;
	INIT_POINT(tmp,
		state->zeroCoord == offsetof(Point, x) ? state->planeValue : p->x,
		state->zeroCoord == offsetof(Point, y) ? state->planeValue : p->y,
		state->zeroCoord == offsetof(Point, z) ? state->planeValue : p->z);
	*o = tmp;
}


void scaleProj(const Point *p, Point *o, const int32_t * scale){
	Point tmp;
	INIT_POINT(tmp,
			   *scale * p->x,
			   *scale * p->y,
			   *scale * p->z);
	*o = tmp;
}

const ProjectionF identity = &identProj;
const ProjectionF orthographic = (ProjectionF)(&orthoProj);
const ProjectionF compose = (ProjectionF)(&composeProj);
const ProjectionF onto = (ProjectionF)(&ontoProj);
const ProjectionF scale = (ProjectionF)(&scaleProj);