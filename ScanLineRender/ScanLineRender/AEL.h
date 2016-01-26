/*
//  AEL.h
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#ifndef AEL_h
#define AEL_h

#include "stdbool.h"
#include "Primitive.h"

typedef struct _EdgeListNode {
	Edge * edge;
	Primitive * owner;
	bool placeHolder;
	struct _EdgeListNode * next;
} EdgeListNode;

#define INIT_ELN(n, ev, ov, pv) \
(n).edge = (ev);\
(n).owner = (ov);\
(n).placeHolder = (pv);\
(n).next = NULL

#define INIT_ELN_EZ(n, ev, ov) INIT_ELN(n, ev, ov, false)

int16_t getMinXForLine(const EdgeListNode * node, const Projection * p, const int16_t scanLine);
int16_t getMaxXForLine(const EdgeListNode * node, const Projection * p, const int16_t scanLine);
int16_t getSmartXForLine(const EdgeListNode * node, const Projection * p, const int16_t scanLine);
bool nodeHoldsSingleton(const EdgeListNode * node);

typedef struct {
	EdgeListNode * activeEdges;
	int16_t scanLine;
} ActiveEdgeList;

const ActiveEdgeList freshAEL(void);
void stepEdges(ActiveEdgeList *ael, const void * activePrims);


#endif /* AEL_h */
