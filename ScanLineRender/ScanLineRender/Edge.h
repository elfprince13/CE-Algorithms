//
//  Edge.h
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
//

#ifndef Edge_h
#define Edge_h

#include "Point.h"

typedef enum {START = 0, END = 1} EndPoint;

typedef struct {
	Point coords[2];
} Edge;

void flip(Edge * );
inline const Edge flipped(const Edge *e){
	return (Edge){{e->coords[END], e->coords[START]}};
	
}


int16_t dot(const Edge *, const Edge *);

#endif /* Edge_h */
