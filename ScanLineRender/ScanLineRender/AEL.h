/*
//  AEL.h
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#ifndef AEL_h
#define AEL_h

#include <stdbool.h>
#include "Primitive.h"
#include "link_list/linklist.h"
#include "rb_tree/red_black_tree.h"

typedef struct _EdgeListEntry {
	Point ** edge;
	Primitive * owner;
	bool placeHolder;
} EdgeListEntry;

#define INIT_ELN(n, ev, ov, pv) \
(n).edge = (ev);\
(n).owner = (ov);\
(n).placeHolder = (pv)

#define INIT_ELN_EZ(n, ev, ov) INIT_ELN(n, ev, ov, false)

float getMinXForLine(const EdgeListEntry * node, const int scanLine);
float getMaxXForLine(const EdgeListEntry * node, const int scanLine);
float getSmartXForLine(const EdgeListEntry * node, const int scanLine);
bool nodeHoldsSingleton(const EdgeListEntry * node);

typedef struct {
	LinkN* activeEdges;
	int scanLine;
} ActiveEdgeList;

const ActiveEdgeList freshAEL(void);
void stepEdges(ActiveEdgeList *ael, const rb_red_blk_tree* activePrims);


#endif /* AEL_h */
