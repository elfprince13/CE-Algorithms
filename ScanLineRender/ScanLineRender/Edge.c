/*
//  Edge.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "Edge.h"
#include "Projection.h"


void flip(Edge * e){
	Point tmp = e->coords[START];
	e->coords[START] = e->coords[END];
	e->coords[END] = tmp;
}

void flipped(const Edge *e, Edge * o){
	o->coords[START] = e->coords[END];
	o->coords[START] = e->coords[START];	
}

int32_t dot(const Edge *u, const Edge *v){
	const Point
	*us= u->coords + START,
	*ue = u->coords + END,
	*vs = v->coords + START,
	*ve = v->coords + END;
	Point u0, v0;
 
	INIT_POINT(u0,
			   ue->x - us->x,
			   ue->y - us->y,
			   ue->z - us->z);
	
	INIT_POINT(v0,
			   ve->x - vs->x,
			   ve->y - vs->y,
			   ve->z - vs->z);
	
	return (u0.x * v0.x) +
	(u0.y * v0.y) +
	(u0.z * v0.z) ;
}

void projectEdge(const Projection * proj, const Edge *e, Edge *o){
	const ProjectionF f = proj->f;
	void * state = proj->state;
	size_t j;
	for (j = START; j <= END; ++j) {
		f(e->coords + j,o->coords + j, state);
	}
}


bool contains(const Edge *e, const Point *p){
	size_t i;
	bool ret = false;
	const Point* coords = e->coords;
	for(i = START; i <= END; ++i ){
		const Point* coord = coords + i;
		if(coord->x == p->x && coord->y == p->y){
			ret = true; break;
		}
	}
	return ret;
}