/*
//  AEL.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "AEL.h"

int16_t getMinXForLine(const EdgeListNode * node, const Projection * p, const int16_t scanLine){
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

int16_t getMaxXForLine(const EdgeListNode * node, const Projection * p, const int16_t scanLine){
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

int16_t getSmartXForLine(const EdgeListNode * node, const Projection * p, const int16_t scanLine){
	return (node->placeHolder ? getMaxXForLine : getMinXForLine)(node, p, scanLine);
}

bool nodeHoldsSingleton(const EdgeListNode * node){
	return node->owner->arity == 1;
}


const ActiveEdgeList freshAEL(){
	const ActiveEdgeList ael = {NULL, -1};
	return ael;
}

void stepEdges(ActiveEdgeList *ael, const void * activePrims){
	
}