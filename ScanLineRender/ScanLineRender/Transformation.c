/*
 //  Transformation.c
 //  ScanLineRender
 //
 //  Created by Thomas Dickerson on 1/25/16.
 //  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
 */

#include "Transformation.h"

void identProj(const Point *p, Point *o, const void * discard){
	*o = *p;
}

void orthoProj(const Point *p, Point *o, const Edge * viewportSpan){
	const Point * min = (*viewportSpan)[START];
	const Point * max = (*viewportSpan)[END];
	const float l = min->x,
	r = max->x,
	b = min->y,
	t = max->y,
	n = min->z,
	f = max->z,
	x = (2*p->x - (l + r))/(r-l),
	y = (2*p->y - (t + b))/(t-b),
	z = (-2*p->z + (f + n))/(f-n);
	Point tmp;
	INIT_POINT(tmp, x,y,z);
	*o = tmp;
}


void composeProj(const Point *p, Point *o, const ComposeProj * composition){
	Point tmp;
	composition->r.f(p, &tmp, composition->r.state);
	composition->l.f(&tmp, o, composition->l.state);
}


void ontoProj(const Point *p, Point *o, const OntoProj * state){
	Point tmp;
	const float x = state->zeroCoord == offsetof(Point, x) ? state->planeValue : p->x,
	y = state->zeroCoord == offsetof(Point, y) ? state->planeValue : p->y,
	z = state->zeroCoord == offsetof(Point, z) ? state->planeValue : p->z;
	INIT_POINT(tmp,x,y,z);
	*o = tmp;
}


void scaleProj(const Point *p, Point *o, const float * scale){
	Point tmp;
	INIT_POINT(tmp,
			   *scale * p->x,
			   *scale * p->y,
			   *scale * p->z);
	*o = tmp;
}

const TransformationF identity = &identProj;
const TransformationF orthographic = (TransformationF)(&orthoProj);
const TransformationF compose = (TransformationF)(&composeProj);
const TransformationF onto = (TransformationF)(&ontoProj);
const TransformationF scale = (TransformationF)(&scaleProj);