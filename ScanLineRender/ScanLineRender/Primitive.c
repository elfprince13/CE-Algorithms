/*
//  Primitive.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "Primitive.h"


Primitive makeLine(const Edge *e){
	Edge * b = malloc(sizeof(Edge));
	b[0] = *e;
	return (Primitive){0, 1, b};
}
Primitive makeTri(const Edge *e1, const Edge *e2, const Edge *e3){
	Edge * b = malloc(3*sizeof(Edge));
	b[0] = *e1;
	b[1] = *e2;
	b[2] = *e3;
	return (Primitive){0, 3, b};
}

Primitive makeQuad(const Edge *e1, const Edge *e2, const Edge *e3, const Edge *e4){
	Edge * b = malloc(4*sizeof(Edge));
	b[0] = *e1;
	b[1] = *e2;
	b[2] = *e3;
	b[3] = *e4;
	return (Primitive){0, 4, b};
}

const int16_t getZForXY(const Primitive *p, const int16_t x, const int16_t y){
	if(p->arity == 1){
		const Edge * v = p->boundary;
		const Point * vs = v->coords + START,
		* ve = v->coords + END;
		
		const int16_t sx = vs->x,
		sy = vs->y,
		sz = vs->z,
		
		dx = ve->x - sx,
		dy = ve->y - sy,
		dz = ve->z - sz;
		
		/* Theoretically these should be the same, but
		// we'll average for robustness */
		const int32_t xNumer = dz * (x - sx),
		yNumer = dz * (y - sy);
		const int16_t xEst = (dx == 0) ? min(0,  dz) : (xNumer / dx),
		yEst = (dy == 0) ? min(0,  dz) : (yNumer / dy);
		
		return sz + (xEst + yEst) / 2;
	} else {
		const Edge * e1 = p->boundary,
		* e2 = e1 + 1;
		
		const Point * us = e1->coords + START,
		* ue = e1->coords + END,
		u = {us->x - ue->x,
			us->y - ue->y,
			us->z - ue->z},
		* vs = e2->coords + START,
		* ve = e2->coords + END,
		v = {vs->x - ve->x,
			vs->y - ve->y,
			vs->z - ve->z};
		
		const int16_t nx = u.y * v.z - u.z * v.y,
		ny = u.z * v.x - u.x * v.z,
		nz = u.x * v.y - u.y * v.x;
		
		const int32_t d = -nx * us->x
		-ny * us->y
		-nz * us->z,
		numer = (-d - nx * x - ny * y);
		
		return (nz == 0) ? ((numer > 0) ? INT16_MAX : INT16_MIN) : (numer / nz) ;
	}
	
}


void projectPrimitive(Projection proj, const Primitive *p, Primitive *o, void * state){
	size_t i, j;
	for(i = 0; i < p->arity; ++i){
		for (j = START; j <= END; ++j) {
			proj(p->boundary[i].coords + j,o->boundary[i].coords + j,state);
		}
	}
}