/*
//  ScanlineRenderer.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/27/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "AEL.h"
#include "ScanlineRenderer.h"
#include "link_list/linklist.h"
#include "rb_tree/red_black_tree.h"

typedef int(*StatelessCompF)(const void*,const void*) ;

/* We really ought to re-type this if possible */
static int topToBottom(const Primitive *, const Primitive *);
static int pointerDiff(const Primitive*, const Primitive*);
static StatelessCompF topToBottomF = (StatelessCompF)(&topToBottom);
static StatelessCompF pointerDiffF = (StatelessCompF)(&pointerDiff);
static int16_t topMostPrimPoint(const Primitive *);
static int16_t topMostEdgePoint(const Edge *);
static int16_t bottomMostPrimPoint(const Primitive *);
static int16_t bottomMostEdgePoint(const Edge *);

static void removeLink(LinkN **aPS, LinkN* target, LinkN* prev);
static void linkFront(LinkN **aPS, LinkN* target);
static LinkN* makeLink(Primitive *p);

void render(Color *raster, int16_t lineWidth, int16_t numLines, const Primitive *geometry, size_t geomCount, const Projection *p){
	static OntoProj screenPlaneData = {offsetof(Point, z), 0};
	static const Projection screenPlane = {(ProjectionF)(&ontoProj), &screenPlaneData};
	Primitive *projectedGeometry = malloc(geomCount * sizeof(Primitive));
	const Primitive* projGEnd = projectedGeometry + geomCount;
	size_t *bucketEnd = calloc(numLines, sizeof(size_t)); /* delta encode bucket start */
	size_t i;
	for(i = 0; i < geomCount; ++i){
		projectPrimitive(p, geometry, projectedGeometry);
	}
	qsort(projectedGeometry, geomCount, sizeof(Primitive), topToBottomF);
	{
		/* Capture the full span of each bucket */
		for(i = 0; i < geomCount; ++i){
			const Primitive *prim = projectedGeometry + i;
			const int16_t pScanLine = bottomMostPrimPoint(prim);
			if(pScanLine < numLines && (pScanLine >= 0 || topMostPrimPoint(prim) >= 0)){
				size_t * dstBucket = bucketEnd + max(0, pScanLine);
				if(*dstBucket <= i) *dstBucket = i+1;
			}
		}
		for(i = 1; i < numLines; ++i){
			const size_t prev = bucketEnd[i-1];
			/* if this is true and bucketEnd[i] is not zero, 
			 * something went wrong before hand
			 * but we need to make sure bucketEnd is monotonic,
			 * because we're delta-encoding the start positions */
			if(bucketEnd[i] < prev) bucketEnd[i] = prev;
		}
	}
	{
		int16_t line;
		LinkN* activePrimSet = NULL;
		ActiveEdgeList ael = freshAEL();
		rb_red_blk_map_tree inFlags;
		rb_red_blk_tree deFlags;
		/* This ensures that both trees are initialized and in a cleared state */
		RBTreeMapInit(&inFlags, pointerDiffF, NULL, &RBMapNodeAlloc, NULL);
		RBTreeInit(&deFlags, pointerDiffF, NULL, &RBNodeAlloc);
		for(line = 0; line < numLines; (++line), (raster += lineWidth)) {
			LinkN* primIt, *p;
			const size_t maxPrimI = bucketEnd[line];
			size_t keySetCt = 0;
			for (p = NULL, primIt = activePrimSet; primIt; (p=primIt),(primIt=primIt->tail)) {
				const Primitive* prim = p->data;
				const int16_t top = topMostPrimPoint(prim);
				if(top < line){
					removeLink(&activePrimSet, primIt, p);
				}
			}
			
			for (i = line ? bucketEnd[line - 1] : 0; i < maxPrimI; ++i) {
				linkFront(&activePrimSet, makeLink(projectedGeometry + i));
			}
			
			stepEdges(&ael, activePrimSet);
			
			{
				int16_t curPixel = 0;
				const Primitive *curDraw = NULL;
				EdgeListEntry *nextEdge;
				LinkN* i = ael.activeEdges;
				if(i){
					nextEdge = i->data;
					while(nextEdge && curPixel < lineWidth){
						EdgeListEntry *startEdge = nextEdge;
						const Primitive *startOwner = startEdge->owner;
						int16_t startX = getSmartXForLine(startEdge, line), nextX;
						rb_red_blk_map_node *inFlag = (rb_red_blk_map_node *)RBExactQuery((rb_red_blk_tree*)(&inFlags), startOwner);
						/* nextEdge = NULL;  We want an error if we use nextEdge prematurely */
						
						if(inFlag){
							const Edge * edgeHere = startEdge->edge, *edgeIn = inFlag->info;
							const Point *s = edgeHere->coords + START,
							*e = edgeHere->coords + END;
							Edge flatHere, flatIn, vert;
							Point here;
							bool sV, eV, v;
							int16_t dotH, dotIn;
							projectEdge(&screenPlane, edgeHere, &flatHere);
							projectEdge(&screenPlane, edgeIn, &flatIn);
							INIT_POINT(here, startX, line, 0);
							sV = contains(edgeIn, s);
							eV = contains(edgeIn, e);
							v = (sV || eV) && contains(&flatIn, &here) && contains(&flatHere, &here) && (startOwner->arity != 1);
							dotH = v ? dot(&vert, &flatHere) : 0;
							dotIn = v ? dot(&vert, &flatIn) : 0;
							if(!v || dotH * dotIn > 0){
								RBTreeInsert(&deFlags, startOwner);
							}
						} else {
							/* This might happen if a polygon is parallel to the x-axis */
							inFlag = (rb_red_blk_map_node *)RBExactQuery((rb_red_blk_tree*)(&inFlags), startOwner);
							if(!inFlag){
								inFlag = (rb_red_blk_map_node *)RBTreeInsert((rb_red_blk_tree*)(&inFlags), startOwner);
								++keySetCt;
							}
							inFlag->info = startEdge->edge;
						}
						
						if(curPixel < startX){
							curPixel = startX;
						}
						
						i = i->tail;
						if(i){
							nextEdge = i->data;
							nextX = getSmartXForLine(nextEdge, line);
						} else {
							nextEdge = NULL;
							nextX = 0;
						}
						
						nextX = min(nextX, lineWidth);
						while ((!nextEdge && curPixel < lineWidth) || (curPixel < nextX)) {
							bool zFight = false, solitary = false;
							int16_t bestZ = 0, j = 0;
							stk_stack keySet;
							rb_red_blk_node *node;
							size_t keySetCt = 0;
							curDraw = NULL;
							RBEnumerate((rb_red_blk_tree*)(&inFlags), projectedGeometry, projGEnd, &keySet);
							while ((node = StackPop(&keySet))) {
								const Primitive *prim = node->key;
								const int16_t testZ = getZForXY(prim, curPixel, line);
								++keySetCt;
								if(++j == 1 || testZ <= bestZ){
									if (testZ == bestZ && j != 1) {
										zFight = true;
										if (prim->arity == 1) {
											curDraw = prim;
											solitary = RBExactQuery(&deFlags, prim);
										}
									} else {
										zFight = false;
										bestZ = testZ;
										curDraw = prim;
										solitary = RBExactQuery(&deFlags, prim);
									}
								}
							}
							
							if(curDraw){
								if(nextEdge || solitary){
									const int16_t drawWidth = (zFight || solitary) ? 1 : ((nextEdge ? nextX : lineWidth) - curPixel),
									stopPixel = lineWidth + min(lineWidth - curPixel,
																max(0, drawWidth));
									const Color drawColor = curDraw->color;
									while(curPixel < stopPixel){
										raster[curPixel++] = drawColor;
									}
								} /* The else case should be an assert(false) */
							} else if((!keySetCt) && nextEdge){
								/* fast forward, we aren't in any polys */
								curPixel = nextX;
							} else {
								/* Nothing left */
								curPixel = lineWidth;
							}
							
							/* n.b.: keySet is empty here */
							RBEnumerate(&deFlags, projectedGeometry, projGEnd, &keySet);
							while ((node = StackPop(&keySet))){
								node = RBExactQuery((rb_red_blk_tree*)(&inFlags), node->key);
								if(node){
									RBDelete((rb_red_blk_tree*)(&inFlags), node);
									--keySetCt;
								}
								
							}
							RBTreeClear(&deFlags);
						}
						
						if (!keySetCt && nextEdge) {
							curPixel = nextX;
						}
						
					}
				}
			}
			
			
			/* This data was for the old line, we don't care anymore */
			RBTreeClear(&deFlags);
			RBTreeClear((rb_red_blk_tree*)(&inFlags));
		}
		RBTreeDestroy(&deFlags, false);
		RBTreeDestroy((rb_red_blk_tree*)(&inFlags), false);
	}
	
}



int pointerDiff(const Primitive*p1, const Primitive*p2){
	const ptrdiff_t delta = p1 - p2;
	return delta ? (delta < 0 ? -1 : 1) : 0;
}

int topToBottom(const Primitive *p1, const Primitive *p2){
	return topMostPrimPoint(p1) - topMostPrimPoint(p2);
}

int16_t topMostPrimPoint(const Primitive *prim){
	const int16_t arity = prim->arity;
	int16_t top, i;
	for(top = topMostEdgePoint(prim->boundary),i = 1; i < arity; ++i){
		const int16_t candidate = topMostEdgePoint(prim->boundary + i);
		if(candidate > top) top = candidate;
	}
	return top;
}

int16_t topMostEdgePoint(const Edge *edge){
	const Point * coords = edge->coords;
	return max(coords[START].y, coords[END].y);
}

int16_t bottomMostPrimPoint(const Primitive *prim){
	const int16_t arity = prim->arity;
	int16_t bottom, i;
	for(bottom = bottomMostEdgePoint(prim->boundary),i = 1; i < arity; ++i){
		const int16_t candidate = bottomMostEdgePoint(prim->boundary + i);
		if(candidate < bottom) bottom = candidate;
	}
	return bottom;
}

int16_t bottomMostEdgePoint(const Edge *edge){
	const Point * coords = edge->coords;
	return min(coords[START].y, coords[END].y);
}

void removeLink(LinkN **aPS, LinkN* target, LinkN* prev){
	if(prev){
		prev->tail = target->tail;
	} else {
		*aPS = target->tail;
	}
	/* We don't own the primitive */
	freeLink(target, NULL);
}

void linkFront(LinkN **aPS, LinkN *target){
	target->tail = *aPS;
	*aPS = target;
}

LinkN* makeLink(Primitive *p){
	LinkN *newLink = malloc(sizeof(LinkN));
	newLink->data = p;
	return newLink;
}