/*
//  linklist.h
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/26/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#ifndef linklist_h
#define linklist_h

#include <stdint.h>

typedef struct _LinkN {
	void * data;
	struct _LinkN *tail;
} LinkN;

typedef uint8_t(*CompareF)(void *, void*);

void mergeSort(LinkN** headRef, CompareF comparator);


#endif /* linklist_h */
