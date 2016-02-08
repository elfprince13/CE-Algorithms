/*
//  Colors.h
//  ScanLineRender
//
//  Created by Thomas Dickerson on 2/8/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#ifndef Colors_h
#define Colors_h

#include <stdint.h>
#include <stddef.h>
#include "debugConfig.h"

typedef uint16_t Color;
/* for LDraw-style palette, must be at least 18 bits. 
 * this is valid on x86 and z80. 
 * We can also use the 6 upper bits to store status flags. */

#ifndef NDEBUG
const char * fmtColor(Color c);
#endif


#endif /* Colors_h */
