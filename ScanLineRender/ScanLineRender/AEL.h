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
#include "link_list/linklist.h"

typedef struct _EdgeListEntry {
	Edge * edge;
	Primitive * owner;
	bool placeHolder;
} EdgeListEntry;

#define INIT_ELN(n, ev, ov, pv) \
(n).edge = (ev);\
(n).owner = (ov);\
(n).placeHolder = (pv)

#define INIT_ELN_EZ(n, ev, ov) INIT_ELN(n, ev, ov, false)

int16_t getMinXForLine(const EdgeListEntry * node, const Projection * p, const int16_t scanLine);
int16_t getMaxXForLine(const EdgeListEntry * node, const Projection * p, const int16_t scanLine);
int16_t getSmartXForLine(const EdgeListEntry * node, const Projection * p, const int16_t scanLine);
bool nodeHoldsSingleton(const EdgeListEntry * node);

typedef struct {
	LinkN* activeEdges;
	int16_t scanLine;
} ActiveEdgeList;

const ActiveEdgeList freshAEL(void);
void stepEdges(ActiveEdgeList *ael, const LinkN* activePrims);


#endif /* AEL_h */
