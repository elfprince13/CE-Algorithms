//
//  main.c
//  SortTest
//
//  Created by Thomas Dickerson on 1/30/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "../ScanLineRender/link_list/linklist.h"

int8_t compareSizeT(size_t *i, size_t *j, void*discard){
	return (int)*i - (int)*j;
}

static const Comparator csT = {(CompareF)&compareSizeT, NULL};

void dumpList(LinkN **head){
	LinkN *i;
	for(i = *head; i; i = i->tail){
		printf("%lu ",*(size_t*)(i->data));
	}
	printf("\n\n");
}

int main(int argc, const char * argv[]) {
	LinkN *null = NULL;
	LinkN **head = &null;
	const size_t COUNT = 37;
	size_t *data = SafeMalloc(COUNT * sizeof(size_t)), i;
	LinkN *nodes = SafeMalloc(COUNT * sizeof(LinkN));
	for(i = 0; i < COUNT; i++){
		data[i] = i;
		nodes[i].data = data + i;
		linkFront(head, nodes + i);
	}
	
	dumpList(head);
	mergeSort(head, &csT);
	dumpList(head);
	
	
	free(nodes);
	free(data);
	
    return 0;
}
