/*
//  main.c
//  SortTest
//
//  Created by Thomas Dickerson on 1/30/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include "../ScanLineRender/link_list/linklist.h"
#include "../ScanLineRender/rb_tree/misc.h"
#include "../ScanLineRender/rb_tree/red_black_tree.h"
#include "../ScanLineRender/debugConfig.h"

int8_t compareSizeT(size_t *i, size_t *j, void*discard){
	return (int)*i - (int)*j;
}

static const Comparator csT = {(CompareF)&compareSizeT, NULL};

int qsCompare(size_t *i, size_t *j){
	return (int)*i - (int)*j;
}

int pointerDiff(const size_t *p1, const size_t *p2){
	const ptrdiff_t delta = p1 - p2;
	return delta ? (delta < 0 ? -1 : 1) : 0;
}

void dumpList(LinkN **head){
	LinkN *i;
	for(i = *head; i; i = i->tail){
		printf(SZF " ",*(size_t*)(i->data));
	}
	printf("\n\n");
}

void dumpArray(size_t *ary, size_t COUNT){
	size_t i;
	for(i = 0; i < COUNT; i++){
		printf(SZF " ",ary[i]);
	}
	printf("\n\n");
}

void dumpSet(rb_red_blk_tree* tree){
	rb_red_blk_node* i;
	for(i = tree->first; i != tree->sentinel; i = TreeSuccessor(tree, i)){
		printf(SZF " ",*(size_t*)(i->key));
	}
	printf("\n\n");
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
	
	
	free(nodes);
	free(data);
	
    return 0;
}
