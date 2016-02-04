/*
//  debugConfig.h
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/29/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#ifndef debugConfig_h
#define debugConfig_h

#include <assert.h>

#ifdef NDEBUG
#define dPrintf(args) ((void)0)
#else
#include <stdio.h>
#define dPrintf(args) printf args
#endif


#endif /* debugConfig_h */
