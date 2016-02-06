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
#ifdef _EZ80
#include <debug.h>
int dPrintfImpl(const char*, ...);
#define dPrintf(args) dPrintfImpl args
#define SZF "%u"
#else
#include <stdio.h>
#define dPrintf(args) printf args
#define SZF "%lu"
#endif
#endif


#endif /* debugConfig_h */
