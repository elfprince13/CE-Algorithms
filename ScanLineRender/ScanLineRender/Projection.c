//
//  Projection.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
//

#include "Projection.h"

void identProj(const Point *p, Point *o, const void * discard){
	*o = *p;
}

void orthoProj(const Point *p, Point *o, const Edge * viewportSpan){
	const Point * min = viewportSpan->coords + START;
	const Point * max = viewportSpan->coords + END;
	int16_t l = min->x;
	int16_t r = max->x;
	int16_t b = min->y;
	int16_t t = max->y;
	int16_t n = min->z;
	int16_t f = max->z;
	*o = (Point){(2*p->x - (l + r))/(r-l),
		(2*p->y - (t + b))/(t-b),
		(-2*p->z + (f + n))/(f-n)};
}


void composeProj(const Point *p, Point *o, const ComposeProj * composition){
	Point tmp;
	composition->r(p, &tmp, composition->rS);
	composition->l(&tmp, o, composition->lS);
}


void ontoProj(const Point *p, Point *o, const OntoProj * state){
	*o = (Point){
		state->zeroCoord == offsetof(Point, x) ? state->planeValue : p->x,
		state->zeroCoord == offsetof(Point, y) ? state->planeValue : p->y,
		state->zeroCoord == offsetof(Point, z) ? state->planeValue : p->z
	};
}


void scaleProj(const Point *p, Point *o, const int16_t * scale){
	*o = (Point){
		*scale * p->x,
		*scale * p->y,
		*scale * p->z
	};
}

const Projection identity = &identProj;
const Projection orthographic = (Projection)(&orthoProj);
const Projection compose = (Projection)(&composeProj);
const Projection onto = (Projection)(&ontoProj);
const Projection scale = (Projection)(&scaleProj);