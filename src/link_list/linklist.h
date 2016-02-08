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

/***************************************************************************************
 * The base type for nodes that belong in a linked-list
 ***************************************************************************************/
typedef struct _LinkN {
	void * data;
	struct _LinkN *tail;
} LinkN;

/***************************************************************************************
 * CompareF compares two data references, and may optionally make use of additional state
 ***************************************************************************************/
typedef int8_t(*CompareF)(const void *, const void*, void*);

/***************************************************************************************
 * The comparator type encapsulates CompareF, and state to be passed through
 ***************************************************************************************/
typedef struct {
	CompareF f;
	void * state;
} Comparator;

/***************************************************************************************
 * Given the head of a list, and a comparator for its data,
 * modifies the list to be in sorted order using the merge sort algorithm
 ***************************************************************************************/
void mergeSort(LinkN** headRef, const Comparator *comparator);

/***************************************************************************************
 * Frees a single link, and optionally, the data.
 ***************************************************************************************/
void freeLink(LinkN *link, void(*freeData)(void*));

/***************************************************************************************
 * Frees a whole list, and optionally, the data.
 ***************************************************************************************/
void freeList(LinkN *link, void(*freeData)(void*));

/***************************************************************************************
 * Points prev->tail to target->tail, unless prev is null
 * in which case headRef is updated instead.
 * Returns the link that that was removed (note: target->tail is not modified)
 ***************************************************************************************/
LinkN* removeLink(LinkN **headRef, LinkN* target, LinkN* prev);

/***************************************************************************************
 * Updates headRef so that target is the new head, and target->tail points to the old
 * head. The previous value of target->tail is returned.
 ***************************************************************************************/
LinkN* linkFront(LinkN **headRef, LinkN* target);



#endif /* linklist_h */
