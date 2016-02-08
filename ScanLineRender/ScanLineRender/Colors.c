/*
//  Colors.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 2/8/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "Colors.h"



#ifndef NDEBUG
const char * fmtColor(Color c){
	/* extra unsafe */
	static char buf[64];
	const int r = (c >> 11) << 3,
	g = ((c >> 5) & 0x3f) << 2,
	b = (c & 0x1f) << 3;
	sprintf(buf,"java.awt.Color[r=%d,g=%d,b=%d]",r,g,b);
	return buf;
}
#endif