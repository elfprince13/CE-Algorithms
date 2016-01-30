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

typedef int8_t(*CompareF)(void *, void*, void*);
typedef struct {
	CompareF f;
	void * state;
} Comparator;

void mergeSort(LinkN** headRef, const Comparator *comparator);
void freeLink(LinkN *link, void(*freeData)(void*));
void freeList(LinkN *link, void(*freeData)(void*));

LinkN* removeLink(LinkN **headRef, LinkN* target, LinkN* prev);
LinkN* linkFront(LinkN **headRef, LinkN* target);



#endif /* linklist_h */
