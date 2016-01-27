/*
//  AEL.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "AEL.h"

int16_t getMinXForLine(const EdgeListEntry * node, const Projection * p, const int16_t scanLine){
	Edge projEdge;
	int16_t sx, ex, sy, ey, run, rise, ret;
	int32_t dxp;
	projectEdge(p, node->edge, &projEdge);
	sx = projEdge.coords[START].x,
	sy = projEdge.coords[START].y,
	ex = projEdge.coords[END].x,
	ey = projEdge.coords[END].y,
	run = ex - sx,
	rise = ey - sy,
	/* Potentially some fixed-point accuracy issues here. 
	 * We may need dedicated routines if the compiler does the stupid thing */
	ret = rise ? (((dxp = (scanLine - sy) * run) / rise) + sx) : min(ex, sx);
	return ret;
}

int16_t getMaxXForLine(const EdgeListEntry * node, const Projection * p, const int16_t scanLine){
	Edge projEdge;
	int16_t sx, ex, sy, ey, run, rise, minBelow, minAbove, ret;
	projectEdge(p, node->edge, &projEdge);
	sx = projEdge.coords[START].x,
	sy = projEdge.coords[START].y,
	ex = projEdge.coords[END].x,
	ey = projEdge.coords[END].y,
	rise = sy - ey,
	run = sx - ex,
	minBelow = getMinXForLine(node, p, scanLine - 1),
	minAbove = getMinXForLine(node, p, scanLine + 1),
	/* Potentially some fixed-point accuracy issues here.
	 * We may need dedicated routines if the compiler does the stupid thing */
	ret = rise ? (run ? (max(minBelow, minAbove) - 1) : sx) : max(sx, ex);
	return ret;
}

int16_t getSmartXForLine(const EdgeListEntry * node, const Projection * p, const int16_t scanLine){
	return (node->placeHolder ? getMaxXForLine : getMinXForLine)(node, p, scanLine);
}

bool nodeHoldsSingleton(const EdgeListEntry * node){
	return node->owner->arity == 1;
}


const ActiveEdgeList freshAEL(){
	const ActiveEdgeList ael = {NULL, -1};
	return ael;
}

void removeLink(ActiveEdgeList *ael, LinkN* target, LinkN* prev);
void linkFront(ActiveEdgeList *ael, LinkN* target);
LinkN* makeLink(Edge *e, Primitive *p, bool s);
#define makeLinkEZ(e, p) makeLink(e,p,false)

int16_t leftToRightF(EdgeListEntry *, EdgeListEntry *, int16_t *);

void stepEdges(ActiveEdgeList *ael, const LinkN* activePrims){
	static int16_t scanLine;
	const static Comparator leftToRight = {(CompareF)(&leftToRightF), &scanLine};
	
	scanLine = ++(ael->scanLine);
	{
		LinkN *i, *p;
		for(p = NULL, i = ael->activeEdges; i; ((p = i),(i = i->tail))){
			const EdgeListEntry* edge = i->data;
			const int16_t ys = edge->edge->coords[START].y,
			ye = edge->edge->coords[END].y,
			edgeEnd = max(ys, ye);
			/* const int16_t lowEnd = min(ys, ye); */
			if(edgeEnd < scanLine){
				removeLink(ael, i, p);
			}
		}
	}
	{
		const LinkN *i;
		for(i = activePrims; i; i = i->tail){
			Primitive *prim = i->data;
			size_t j; uint16_t jMax = prim->arity;
			for(j = 0; j < jMax; ++j){
				Edge *e = prim->boundary + j;
				const int16_t sy = e->coords[START].y,
				ey = e->coords[END].y,
				mnY = min(sy, ey),
				mxY = max(sy, ey);
				const bool singleton = prim->arity == 1;
				if((mnY == scanLine && (sy != ey || (singleton /* newEdge.isSingleton() */)))
				   || (scanLine == 0 && mnY < 0 && mxY > 0)){
					linkFront(ael, makeLinkEZ(e, prim));
					if (singleton) {
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
	/* Entries don't own the primitives they point to */
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

int16_t leftToRightF(EdgeListEntry *o1, EdgeListEntry *o2, int16_t *scanLine){
	const static Projection id = {&identProj, NULL};
	return getSmartXForLine(o1, &id, *scanLine) - getSmartXForLine(o2, &id, *scanLine);
}