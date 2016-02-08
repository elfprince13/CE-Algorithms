/*
//  debugConfig.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 2/6/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#ifdef _EZ80
#ifndef NDEBUG

#include "debugConfig.h"
#include <stdarg.h>

int dPrintfImpl(const char* fmt, ...){
	int ret;
	va_list args;
	va_start(args, fmt);
	ret = vsprintf(dbgout, fmt, args);
	va_end(args);
	return ret;
}

#endif
#endif