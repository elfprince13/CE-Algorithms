/*
//  AEL.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "AEL.h"
#include "debugConfig.h"

float getMinXForLine(const EdgeListEntry * node, const int32_t scanLine){
	const Edge *const coords = node->edge;
	const Point *const start = (*coords)[START],
	*const end = (*coords)[END];
	const float sx = start->x,
	sy = start->y,
	ex = end->x,
	ey = end->y,
	run = ex - sx,
	rise = ey - sy;
	float ret;
	if(rise){
		const float dxp = (scanLine - sy) * run;
		ret = (dxp / rise) + sx;
	} else {
		ret = min(ex, sx);
	}
	return ret;
}

float getMaxXForLine(const EdgeListEntry * node, const int32_t scanLine){
	const Edge *const coords = node->edge;
	const Point *const start = (*coords)[START],
	*const end = (*coords)[END];
	const float sx = start->x,
	sy = start->y,
	ex = end->x,
	ey = end->y,
	rise = sy - ey,
	run = sx - ex,
	minBelow = getMinXForLine(node, scanLine - 1),
	minAbove = getMinXForLine(node, scanLine + 1),
	/* Potentially some fixed-point accuracy issues here.
	 * We may need dedicated routines if the compiler does the stupid thing */
	ret = rise ? (run ? (max(minBelow, minAbove) - 1) : sx) : max(sx, ex);
	return ret;
}

float getSmartXForLine(const EdgeListEntry * node, const int32_t scanLine){
	return (node->placeHolder ? getMaxXForLine : getMinXForLine)(node, scanLine);
}

bool nodeHoldsSingleton(const EdgeListEntry * node){
	return node->owner->arity == 1;
}


const ActiveEdgeList freshAEL(){
	const ActiveEdgeList ael = {NULL, -1};
	return ael;
}

static LinkN* makeLink(Edge *e, Primitive *p, bool s);
#define makeLinkEZ(e, p) makeLink(e,p,false)

static int32_t leftToRightF(EdgeListEntry *, EdgeListEntry *, int32_t *);

void stepEdges(ActiveEdgeList *ael, const rb_red_blk_tree* activePrims){
	static int32_t scanLine;
	const static Comparator leftToRight = {(CompareF)(&leftToRightF), &scanLine};
	LinkN **aelHead = &(ael->activeEdges);
	scanLine = ++(ael->scanLine);
	{
		LinkN *i, *p, *nextP;
		for(p = NULL, i = ael->activeEdges; i; (p = i),(i = nextP)){
			const EdgeListEntry *const entry = i->data;
			const Edge *const edge = entry->edge;
			const float ys = (*edge)[START]->y,
			ye = (*edge)[END]->y,
			edgeEnd = max(ys, ye);
			nextP = i->tail;
			if(edgeEnd < scanLine){
#ifndef NDEBUG
				{
					const float lowEnd = min(ys, ye);
					dPrintf(("Deactivating %s with y-span: %f -> %f with x-span: %f -> %f(true: %f -> %f)\n",
						   fmtColor(entry->owner->color), lowEnd, edgeEnd,
						   getMinXForLine(entry, scanLine), getMaxXForLine(entry, scanLine),
						   (*edge)[START]->x, (*edge)[END]->x));
				}
#endif
				/* Entries don't own the primitives they point to,
				 * so we can get away with a simple free  */
				freeLink(removeLink(aelHead, i, p), &free);
				i = p; /* We don't want to advance p into garbage data */
			}
		}
	}
	{
		const rb_red_blk_node *i;
		for(i = activePrims->first; i != activePrims->sentinel; i = TreeSuccessor(activePrims, i)){
			Primitive *const prim = i->key;
			const size_t jMax = prim->arity;
			size_t j;
			for(j = 0; j < jMax; ++j){
				Edge *const e = prim->boundary[j];
				const float sy = (*e)[START]->y,
				ey = (*e)[END]->y,
				mnY = min(sy, ey),
				mxY = max(sy, ey);
				const bool singleton = prim->arity == 1;
				if((mnY == scanLine && (sy != ey || (singleton /* newEdge.isSingleton() */)))
				   || (scanLine == 0 && mnY < 0 && mxY > 0)){
					LinkN* newEdge = makeLinkEZ(e, prim);
#ifndef NDEBUG
					{
						dPrintf(("Activating %s with y-span: %f -> %f with x-span: %f -> %f(true: %f -> %f)\n",
							   fmtColor(prim->color), mnY, mxY,
							   getMinXForLine(newEdge->data, scanLine), getMaxXForLine(newEdge->data, scanLine),
							   (*e)[START]->x, (*e)[END]->x));
					}
#endif
					linkFront(aelHead, newEdge);
					if (singleton) {
						dPrintf(("\t->Activating dummy end\n"));
						linkFront(aelHead, makeLink(e, prim, true));
					}
				}
			}
		}
	}
	mergeSort(&(ael->activeEdges), &leftToRight);
}

LinkN* makeLink(Edge *e, Primitive *p, bool s){
	LinkN *newLink = malloc(sizeof(LinkN));
	EdgeListEntry *newEdge = malloc(sizeof(EdgeListEntry));
	newLink->data = newEdge;
	INIT_ELN(*newEdge, e, p, s);
	return newLink;
}

int leftToRightF(EdgeListEntry *o1, EdgeListEntry *o2, int *scanLine){
	float delta = getSmartXForLine(o1, *scanLine) - getSmartXForLine(o2, *scanLine);
#ifndef NDEBUG
	const Edge *const e1 = o1->edge,
	*const e2 = o2->edge;
	if (!delta) delta = (*e1)[START]->x - (*e2)[START]->x;
	if (!delta) delta = (*e1)[END]->x - (*e2)[END]->x;
	if (!delta) delta = (*e1)[START]->y - (*e2)[START]->y;
	if (!delta) delta = (*e1)[END]->y - (*e2)[END]->y;
	if (!delta) delta = o1->owner->arity - o2->owner->arity;
	if (!delta) delta = o1->owner->color - o2->owner->color;
	if (!delta) delta = (int)(o1->placeHolder) - (int)(o2->placeHolder);
#endif
	return delta ? (delta > 0 ? 1 : -1) : 0;
}