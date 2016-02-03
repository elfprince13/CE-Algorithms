/*
//  Primitive.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "Primitive.h"
#include <math.h>

#ifndef NDEBUG
const char * fmtColor(Color c){
	/* extra unsafe */
	static char buf[64];
	const uint32_t r = (c >> 16),
	g = ((c >> 8) & 0xff),
	b = (c & 0xff);
	sprintf(buf,"java.awt.Color[r=%d,g=%d,b=%d]",r,g,b);
	return buf;
}
#endif

void makeLine(const Edge *e, Primitive *o){
	Primitive tmp;
	Edge * b = malloc(sizeof(Edge));
	b[0] = *e;
	INIT_PRIM(tmp, 0, 1, b);
	*o = tmp;
}
void makeTri(const Edge *e1, const Edge *e2, const Edge *e3, Primitive *o){
	Primitive tmp;
	Edge * b = malloc(3*sizeof(Edge));
	b[0] = *e1;
	b[1] = *e2;
	b[2] = *e3;
	INIT_PRIM(tmp, 0, 3, b);
	*o = tmp;
}

void makeQuad(const Edge *e1, const Edge *e2, const Edge *e3, const Edge *e4, Primitive *o){
	Primitive tmp;
	Edge * b = malloc(4*sizeof(Edge));
	b[0] = *e1;
	b[1] = *e2;
	b[2] = *e3;
	b[3] = *e4;
	INIT_PRIM(tmp, 0, 4, b);
	*o = tmp;
}

float getZForXY(const Primitive *p, float x, float y){
	if(p->arity == 1){
		const Edge * v = p->boundary;
		const Point * vs = v->coords + START,
		* ve = v->coords + END;
		
		const float sx = vs->x,
		sy = vs->y,
		sz = vs->z,
		
		dx = ve->x - sx,
		dy = ve->y - sy,
		dz = ve->z - sz;
		
		/* Theoretically these should be the same, but
		 // we'll average for robustness */
		const float xNumer = dz * (x - sx),
		yNumer = dz * (y - sy);
		const float xEst = (dx == 0) ? min(0,  dz) : (xNumer / dx),
		yEst = (dy == 0) ? min(0,  dz) : (yNumer / dy);
		
		return sz + (xEst + yEst) / 2;
	} else {
		const Edge * e1 = p->boundary,
		* e2 = e1 + 1;
		
		const Point * us = e1->coords + START,
		* ue = e1->coords + END,
		* vs = e2->coords + START,
		* ve = e2->coords + END;
		
		const float ux = us->x - ue->x,
		uy = us->y - ue->y,
		uz = us->z - ue->z,
		vx = vs->x - ve->x,
		vy = vs->y - ve->y,
		vz = vs->z - ve->z;
		
		const float nx = uy * vz - uz * vy,
		ny = uz * vx - ux * vz,
		nz = ux * vy - uy * vx;
		
		const float d = -nx * us->x
		-ny * us->y
		-nz * us->z,
		numer = (-d - nx * x - ny * y);
		
		return (nz == 0) ? ((numer > 0) ? HUGE_VALF : -HUGE_VALF) : (numer / nz) ;
	}
	
}


void projectPrimitive(const Projection * proj, const Primitive *p, Primitive *o){
	size_t i;
	for(i = 0; i < p->arity; ++i){
		projectEdge(proj, p->boundary + i, o->boundary + i);
	}
}


int32_t hashPrim(const Primitive *p){
	size_t i; int32_t ret = 0;
	for(i = 0; i < p->arity; ++i){
		ret ^= HASH_EDGE(p->boundary[i]);
	}
	ret ^= (p->color << 8) | 0xFF;
	return ret;
}