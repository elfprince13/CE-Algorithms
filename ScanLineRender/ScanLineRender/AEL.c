/*
//  AEL.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "AEL.h"
#include "debugConfig.h"

int32_t getMinXForLine(const EdgeListEntry * node, const int32_t scanLine){
	const Point *coords = node->edge->coords;
	const int32_t sx = coords[START].x,
	sy = coords[START].y,
	ex = coords[END].x,
	ey = coords[END].y,
	run = ex - sx,
	rise = ey - sy;
	int32_t ret;
	if(rise){
		/* Potentially some fixed-point accuracy issues here.
		 * We may need dedicated routines if the compiler does the stupid thing */
		const int32_t dxp = (scanLine - sy) * run;
		ret = (dxp / rise) + sx;
	} else {
		ret = min(ex, sx);
	}
	return ret;
}

int32_t getMaxXForLine(const EdgeListEntry * node, const int32_t scanLine){
	const Point *coords = node->edge->coords;
	const int32_t sx = coords[START].x,
	sy = coords[START].y,
	ex = coords[END].x,
	ey = coords[END].y,
	rise = sy - ey,
	run = sx - ex,
	minBelow = getMinXForLine(node, scanLine - 1),
	minAbove = getMinXForLine(node, scanLine + 1),
	/* Potentially some fixed-point accuracy issues here.
	 * We may need dedicated routines if the compiler does the stupid thing */
	ret = rise ? (run ? (max(minBelow, minAbove) - 1) : sx) : max(sx, ex);
	return ret;
}

int32_t getSmartXForLine(const EdgeListEntry * node, const int32_t scanLine){
	return (node->placeHolder ? getMaxXForLine : getMinXForLine)(node, scanLine);
}

bool nodeHoldsSingleton(const EdgeListEntry * node){
	return node->owner->arity == 1;
}


const ActiveEdgeList freshAEL(){
	const ActiveEdgeList ael = {NULL, -1};
	return ael;
}

static void removeLink(ActiveEdgeList *ael, LinkN* target, LinkN* prev);
static void linkFront(ActiveEdgeList *ael, LinkN* target);
static LinkN* makeLink(Edge *e, Primitive *p, bool s);
#define makeLinkEZ(e, p) makeLink(e,p,false)

static int32_t leftToRightF(EdgeListEntry *, EdgeListEntry *, int32_t *);

void stepEdges(ActiveEdgeList *ael, const rb_red_blk_tree* activePrims){
	static int32_t scanLine;
	const static Comparator leftToRight = {(CompareF)(&leftToRightF), &scanLine};
	scanLine = ++(ael->scanLine);
	{
		LinkN *i, *p, *nextP;
		for(p = NULL, i = ael->activeEdges; i; (p = i),(i = nextP)){
			const EdgeListEntry* edge = i->data;
			const int32_t ys = edge->edge->coords[START].y,
			ye = edge->edge->coords[END].y,
			edgeEnd = max(ys, ye);
			nextP = i->tail;
			if(edgeEnd < scanLine){
				{
					const int32_t lowEnd = min(ys, ye);
					printf("Deactivating %s with y-span: %d -> %d with x-span: %d - %d (true: %d -> %d)\n",
						   fmtColor(edge->owner->color), lowEnd, edgeEnd,
						   getMinXForLine(edge, scanLine), getMaxXForLine(edge, scanLine),
						   edge->edge->coords[START].x, edge->edge->coords[END].x);
				}
				removeLink(ael, i, p);
				i = p; /* We don't want to advance p into garbage data */
			}
		}
	}
	{
		const rb_red_blk_node *i;
		for(i = activePrims->first; i != activePrims->sentinel; i = TreeSuccessor(activePrims, i)){
			Primitive *prim = i->key;
			size_t j;
			uint32_t jMax = prim->arity;
			for(j = 0; j < jMax; ++j){
				Edge *e = prim->boundary + j;
				const int32_t sy = e->coords[START].y,
				ey = e->coords[END].y,
				mnY = min(sy, ey),
				mxY = max(sy, ey);
				const bool singleton = prim->arity == 1;
				if((mnY == scanLine && (sy != ey || (singleton /* newEdge.isSingleton() */)))
				   || (scanLine == 0 && mnY < 0 && mxY > 0)){
					LinkN* newEdge = makeLinkEZ(e, prim);
					{
						printf("Activating %s with y-span: %d -> %d with x-span: %d -> %d(true: %d -> %d)\n",
							   fmtColor(prim->color), mnY, mxY,
							   getMinXForLine(newEdge->data, scanLine), getMaxXForLine(newEdge->data, scanLine),
							   e->coords[START].x, e->coords[END].x);
					}
					linkFront(ael, newEdge);
					if (singleton) {
						printf("\t->Activating dummy end\n");
						linkFront(ael, makeLink(e, prim, true));
					}
				}
			}
		}
	}
	mergeSort(&(ael->activeEdges), &leftToRight);
}

void removeLink(ActiveEdgeList *ael, LinkN* target, LinkN* prev){
	if(prev){
		prev->tail = target->tail;
	} else {
		ael->activeEdges = target->tail;
	}
	/* Entries don't own the primitives they point to, 
	 * so we can get away with a simple free  */
	freeLink(target, &free);
}

void linkFront(ActiveEdgeList *ael, LinkN *target){
	target->tail = ael->activeEdges;
	ael->activeEdges = target;
}


LinkN* makeLink(Edge *e, Primitive *p, bool s){
	LinkN *newLink = malloc(sizeof(LinkN));
	EdgeListEntry *newEdge = malloc(sizeof(EdgeListEntry));
	newLink->data = newEdge;
	INIT_ELN(*newEdge, e, p, s);
	return newLink;
}

int32_t leftToRightF(EdgeListEntry *o1, EdgeListEntry *o2, int32_t *scanLine){
	int32_t delta = getSmartXForLine(o1, *scanLine) - getSmartXForLine(o2, *scanLine);
#ifndef NDEBUG
	if (!delta) {
		delta = o1->edge->coords[START].x - o2->edge->coords[START].x;
	}
	if (!delta) {
		delta = o1->edge->coords[END].x - o2->edge->coords[END].x;
	}
	if(!delta) {
		delta = o1->owner->arity - o2->owner->arity;
	}
#endif
	return delta;
}