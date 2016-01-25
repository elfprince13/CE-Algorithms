//
//  Point.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
//

#include "Point.h"

void negate(Point *p){
	p->x *= -1;
	p->y *= -1;
	p->z *= -1;
}