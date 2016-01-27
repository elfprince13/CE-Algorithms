/*
//  linklist.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/26/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "linklist.h"

void stealHead(LinkN**, LinkN**);
LinkN* merge(LinkN*, LinkN*, CompareF);
void divide(LinkN*, LinkN**, LinkN**);

void mergeSort(LinkN** headRef, CompareF comparator) {
	LinkN *head = *headRef, *left, *right;
	if(head && head->tail){
		divide(head, &left, &right);
		mergeSort(&left, comparator);
		mergeSort(&right, comparator);
		*headRef = merge(left, right, comparator);
	}
}

LinkN* merge(LinkN* left, LinkN* right, CompareF comparator){
	LinkN* result = NULL;
	LinkN** tail;
	for(tail = &result;left && right;tail = &((*tail)->tail)){
		stealHead(tail, comparator(left->data, right->data) <= 0 ? &left : &right);
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