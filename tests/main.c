/*
//  main.c
//  AlgosTest
//
//  Created by Thomas Dickerson on 1/30/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/


#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "link_list/linklist.h"
#include "rb_tree/red_black_tree.h"
#include "debugConfig.h"

static int8_t compareSizeT(const size_t *i, const size_t *j, void*discard);
static int qsCompare(size_t *i, size_t *j);
static int pointerDiff(const size_t *p1, const size_t *p2);
static void dumpList(LinkN **head);
static void dumpArray(size_t *ary, size_t COUNT);
static void dumpSet(rb_red_blk_tree* tree);

int8_t compareSizeT(const size_t *i, const size_t *j, void*discard){
	return (int)*i - (int)*j;
}

static const Comparator csT = {(CompareF)&compareSizeT, NULL};

int qsCompare(size_t *i, size_t *j){
	return (int)*i - (int)*j;
}

int pointerDiff(const size_t *p1, const size_t *p2){
	const ptrdiff_t delta = p1 - p2;
	const int ret = delta ? (delta < 0 ? -1 : 1) : 0;
	dPrintf(("pd delta: %p - %p = " PDF " ?sign= %d\n", p1, p2, delta, ret));
	return ret;
}

void dumpList(LinkN **head){
	LinkN *i;
	for(i = *head; i; i = i->tail){
		dPrintf((SZF " ",*(size_t*)(i->data)));
	}
	dPrintf(("\n\n"));
}

void dumpArray(size_t *ary, size_t COUNT){
	size_t i;
	for(i = 0; i < COUNT; i++){
		dPrintf((SZF " ",ary[i]));
	}
	dPrintf(("\n\n"));
}

void dumpSet(rb_red_blk_tree* tree){
	rb_red_blk_node* i;
	for(i = tree->first; i != tree->sentinel; i = TreeSuccessor(tree, i)){
		dPrintf((SZF " ",*(size_t*)(i->key)));
	}
	dPrintf(("\n\n"));
}

int main(int argc, const char * argv[]) {
	LinkN *null = NULL;
	LinkN **head = &null;
	const size_t COUNT = 37;
	size_t *data = SafeMalloc(COUNT * sizeof(size_t)), i;
	LinkN *nodes = SafeMalloc(COUNT * sizeof(LinkN));
	rb_red_blk_tree tree;
	RBTreeInit(&tree, &pointerDiff, NULL, &RBNodeAlloc);
	
	for(i = 0; i < COUNT; i++){
		data[i] = i;
		nodes[i].data = data + i;
		linkFront(head, nodes + i);
	}
	
	dumpList(head);
	mergeSort(head, &csT);
	dumpList(head);
	
	qsort(data, 0, COUNT, &qsCompare);
	dumpArray(data, COUNT);
	
	for(i = 0; i < COUNT; i++){
		RBSetAdd(&tree, data + i);
	}
	
	dumpSet(&tree);
	
	dPrintf(("%p - %p = " PDF " ?= (-1 * " PDF ")\n",(void*)&data[2],(void*)&data[3],(&data[2] - &data[3]),(&data[3] - &data[2])));
	dPrintf(("\t ?= %d ?= (-1 * %d)\n",pointerDiff(&data[2], &data[3]),pointerDiff(&data[3], &data[2])));
	
	
	free(nodes);
	free(data);
	
    return 0;
}


#ifdef _EZ80

void cleanUp()
{
	// Clear/invalidate some RAM areas
	// and restore the home screen nicely
	_OS( asm("CALL _DelRes");
		asm("CALL _ClrTxtShd");
		asm("CALL _ClrScrn");
		asm("SET  graphDraw,(iy+graphFlags)");
		asm("CALL _HomeUp");
		asm("CALL _DrawStatusBar");
		);
}
#endif