/*
//  linklist.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/26/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "linklist.h"
#include <stdlib.h>

static void stealHead(LinkN**, LinkN**);
static LinkN* merge(LinkN*, LinkN*, const Comparator*);
static void divide(LinkN*, LinkN**, LinkN**);

void mergeSort(LinkN** headRef, const Comparator *comparator) {
	LinkN *head = *headRef, *left, *right;
	if(head && head->tail){
		divide(head, &left, &right);
		mergeSort(&left, comparator);
		mergeSort(&right, comparator);
		*headRef = merge(left, right, comparator);
	}
}

LinkN* merge(LinkN* left, LinkN* right, const Comparator *comparator){
	const CompareF f = comparator->f;
	void * state = comparator->state;
	LinkN* result = NULL;
	LinkN** tail;
	for(tail = &result;left && right;tail = &((*tail)->tail)){
		stealHead(tail, f(left->data, right->data, state) <= 0 ? &left : &right);
	}
	if(left){
		*tail = left;
	} else if (right){
		*tail = right;
	}
	return result;
}

void stealHead(LinkN** dstRef, LinkN** srcRef) {
	LinkN* head = *srcRef;
	*srcRef = head->tail;
	head->tail = *dstRef;
	*dstRef = head;
}

void divide(LinkN* source, LinkN** leftRef, LinkN** rightRef)
{
	LinkN* fast;
	LinkN* slow;
	if (source && source->tail) {
		slow = source;
		fast = source->tail;
		while (fast) {
			fast = fast->tail;
			if (fast) {
				slow = slow->tail;
				fast = fast->tail;
			}
		}
		*leftRef = source;
		*rightRef = slow->tail;
		slow->tail = NULL;
	} else {
		/* length < 2 cases */
		*leftRef = source;
		*rightRef = NULL;
	}
}



void freeLink(LinkN *link, void(*freeData)(void*)){
	if(freeData) freeData(link->data);
	free(link);
}
void freeList(LinkN *link, void(*freeData)(void*)){
	LinkN *head, *tail;
	for(head = link; head; head=tail){
		tail = head->tail;
		freeLink(head,freeData);
	}
}