//
//  Primitive.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
//

#include "Primitive.h"

const int16_t getZForXY(const Primitive *p, const int16_t x, const int16_t y){
	//assert(getArity() > 0);
	if(p->arity == 1){
		// Alpha blend! wut wut.
		Edge * v = p->boundary;
		Point * vs = v->coords + START;
		Point * ve = v->coords + END;
		
		int16_t sx = vs->x;
		int16_t sy = vs->y;
		int16_t sz = vs->z;
		
		int16_t dx = ve->x - sx;
		int16_t dy = ve->y - sy;
		int16_t dz = ve->z - sz;
		
		//assert((dx == 0 && dy == 0) || (dx != 0 && dy != 0));
		
		// Theoretically these should be the same, but
		// we'll average for robustness
		int32_t xNumer = dz * (x - sx);
		int32_t yNumer = dz * (y - sy);
		int16_t xEst = (dx == 0) ? min(0,  dz) : (xNumer / dx);
		int16_t yEst = (dy == 0) ? min(0,  dz) : (yNumer / dy);
		
		return sz + (xEst + yEst) / 2;
	} else {
		Edge * e1 = p->boundary;
		Edge * e2 = e1 + 1;
		
		Point * us = e1->coords + START;
		Point * ue = e1->coords + END;
		Point u = {us->x - ue->x,
			us->y - ue->y,
			us->z - ue->z};
		
		Point * vs = e2->coords + START;
		Point * ve = e2->coords + END;
		Point v = {vs->x - ve->x,
			vs->y - ve->y,
			vs->z - ve->z};
		
		int16_t nx = u.y * v.z - u.z * v.y;
		int16_t ny = u.z * v.x - u.x * v.z;
		int16_t nz = u.x * v.y - u.y * v.x;
		
		int32_t d = -nx * us->x
		-ny * us->y
		-nz * us->z;
		
		int32_t numer = (-d - nx * x - ny * y);
		
		return (nz == 0) ? ((numer > 0) ? INT16_MAX : INT16_MIN) : (numer / nz) ;
	}
	
}


void projectPrimitive(Projection proj, const Primitive *p, Primitive *o, void * state){
	for(size_t i = 0; i < p->arity; ++i){
		for (size_t j = START; j <= END; ++j) {
			proj(p->boundary[i].coords + j,o->boundary[i].coords + j,state);
		}
	}
}