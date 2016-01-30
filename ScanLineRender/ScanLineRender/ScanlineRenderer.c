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
#include "debugConfig.h"


typedef int(*StatelessCompF)(const void*,const void*) ;

/* We really ought to re-type this if possible */
static int topToBottom(const Primitive *, const Primitive *);
static int pointerDiff(const Primitive*, const Primitive*);
static StatelessCompF topToBottomF = (StatelessCompF)(&topToBottom);
static StatelessCompF pointerDiffF = (StatelessCompF)(&pointerDiff);
static int32_t topMostPrimPoint(const Primitive *);
static int32_t topMostEdgePoint(const Edge *);
static int32_t bottomMostPrimPoint(const Primitive *);
static int32_t bottomMostEdgePoint(const Edge *);
#ifndef NDEBUG
static int32_t rightMostPrimPoint(const Primitive *);
static int32_t rightMostEdgePoint(const Edge *);
static int32_t leftMostPrimPoint(const Primitive *);
static int32_t leftMostEdgePoint(const Edge *);
#endif


void render(Color *raster, int32_t lineWidth, int32_t numLines, const Primitive *geometry, size_t geomCount, const Projection *p){
	static OntoProj screenPlaneData = {offsetof(Point, z), 0};
	static const Projection screenPlane = {(ProjectionF)(&ontoProj), &screenPlaneData};
	Primitive *const projectedGeometry = malloc(geomCount * sizeof(Primitive));
	size_t edgeCount;
	rb_red_blk_tree *const scanLinePrimBuckets = malloc(numLines * sizeof(rb_red_blk_tree)); /* delta encode bucket start */
	size_t i;
	for(i = 0; i < numLines; RBTreeInit(scanLinePrimBuckets + (i++), pointerDiffF, NULL, &RBNodeAlloc)){}
	for(i = 0, edgeCount = 0; i < geomCount; edgeCount += geometry[i++].arity){}
	
	{
		Edge *const projectedEdges = malloc(edgeCount * sizeof(Edge));
		for(i = 0, edgeCount = 0; i < geomCount; edgeCount += geometry[i++].arity){
			INIT_PRIM(projectedGeometry[i], geometry[i].color, geometry[i].arity, projectedEdges + edgeCount);
			projectPrimitive(p, geometry+i, projectedGeometry+i);
			dPrintf(("Source geometry with arity %d begins on %d and ends on %d\n", geometry[i].arity, bottomMostPrimPoint(geometry + i), topMostPrimPoint(geometry + i)));
			dPrintf(("Projected geometry with arity %d begins on %d and ends on %d\n", projectedGeometry[i].arity, bottomMostPrimPoint(projectedGeometry + i), topMostPrimPoint(projectedGeometry + i)));
		}
		qsort(projectedGeometry, geomCount, sizeof(Primitive), topToBottomF);
		for(i = 0; i < geomCount; ++i){
			Primitive *prim = projectedGeometry + i;
			const int32_t pScanLine = bottomMostPrimPoint(prim);
			if(pScanLine < numLines && (pScanLine >= 0 || topMostPrimPoint(prim) >= 0)){
				rb_red_blk_tree *const dstBucket = scanLinePrimBuckets + max(0, pScanLine);
				RBSetAdd(dstBucket, prim);
				dPrintf(("dstBucket %ld gets geometry with arity %d begins on %d and ends on %d and now has size %lu\n", dstBucket - scanLinePrimBuckets, prim->arity, bottomMostPrimPoint(prim), topMostPrimPoint(prim), dstBucket->size));
			}
		}
		{
			int32_t line;
			rb_red_blk_tree activePrimSet;
			ActiveEdgeList ael = freshAEL();
			rb_red_blk_map_tree inFlags;
			rb_red_blk_tree deFlags;
			/* This ensures that both trees are initialized and in a cleared state */
			RBTreeMapInit(&inFlags, pointerDiffF, NULL, &RBMapNodeAlloc, NULL);
			RBTreeInit(&deFlags, pointerDiffF, NULL, &RBNodeAlloc);
			RBTreeInit(&activePrimSet, pointerDiffF, NULL, &RBNodeAlloc);
			dPrintf(("Scanning line: 0\n"));
			for(line = 0; line < numLines; (++line), (raster += lineWidth)) {
				rb_red_blk_node *primIt, *p = NULL, *nextP;
				dPrintf(("\tUpdating activePrimSet\n"));
				for (primIt = activePrimSet.first; primIt != activePrimSet.sentinel; (p = primIt), (primIt = nextP)) {
					const Primitive* prim = primIt->key;
					const int32_t top = topMostPrimPoint(prim);
					nextP = TreeSuccessor(&activePrimSet, primIt);
					if(top < line){
#ifndef NDEBUG
						{
							const int32_t bottom = bottomMostPrimPoint(prim);
							dPrintf(("\t\t%d -> %d ( %s ) is not valid here: %d\n",top,bottom,fmtColor(prim->color), line));
						}
#endif
						RBDelete(&activePrimSet, primIt);
						primIt = p; /* We don't want to advance p into garbage data */
					}
				}
				{
					const rb_red_blk_tree *bucket = scanLinePrimBuckets + line;
					const rb_red_blk_node *node;
					for(node = bucket->first; node != bucket->sentinel; node = TreeSuccessor(bucket, node)) {
						Primitive * prim = node->key;
#ifndef NDEBUG
						{
							const int32_t top = topMostPrimPoint(prim),
							bottom = bottomMostPrimPoint(prim);
							dPrintf(("\t\t%d -> %d ( %s ) is added here: %d\n",top,bottom,fmtColor(prim->color), line));
						}
#endif
						RBTreeInsert(&activePrimSet, prim);
					}
				}
				stepEdges(&ael, &activePrimSet);
				{
					int32_t curPixel = 0;
					const Primitive *curDraw = NULL;
					EdgeListEntry *nextEdge;
					LinkN* i = ael.activeEdges;
					if(i){
						nextEdge = i->data;
						while(nextEdge && curPixel < lineWidth){
							EdgeListEntry *const startEdge = nextEdge;
							Primitive *const startOwner = startEdge->owner;
							int32_t startX = getSmartXForLine(startEdge, line), nextX;
							rb_red_blk_map_node *inFlag = (rb_red_blk_map_node *)RBExactQuery((rb_red_blk_tree*)(&inFlags), startOwner);
							
							if(inFlag){
								const Edge * edgeHere = startEdge->edge, *edgeIn = inFlag->info;
								const Point *s = edgeHere->coords + START,
								*e = edgeHere->coords + END;
								Edge flatHere, flatIn, vert;
								Point here;
								bool sV, eV, v;
								int32_t dotH, dotIn;
								projectEdge(&screenPlane, edgeHere, &flatHere);
								projectEdge(&screenPlane, edgeIn, &flatIn);
								INIT_POINT(here, startX, line, 0);
								sV = contains(edgeIn, s);
								eV = contains(edgeIn, e);
								v = (sV || eV) && contains(&flatIn, &here) && contains(&flatHere, &here) && (startOwner->arity != 1);
								vert.coords[START] = here;
								INIT_POINT(vert.coords[END], startX, line+1, 0);
								dotH = v ? dotEdge(&vert, &flatHere) : 0;
								dotIn = v ? dotEdge(&vert, &flatIn) : 0;
								if(!v || dotH * dotIn > 0){
									dPrintf(("\tNot *in* old %s at %d\n", fmtColor(startEdge->owner->color), getSmartXForLine(startEdge, line)));
									RBSetAdd(&deFlags, startOwner);
								} else {
									dPrintf(("\tFound horizontal vertex %s at %d. Don't delete it yet\n",fmtColor(startEdge->owner->color), getSmartXForLine(startEdge, line)));
								}
							} else {
								dPrintf(("\tNow *in* new %s at %d\n",fmtColor(startEdge->owner->color), getSmartXForLine(startEdge, line)));
								/* This might happen if a polygon is parallel to the x-axis */
								RBMapPut(&inFlags, startOwner, startEdge->edge);
							}
							
							if(curPixel < startX){
								dPrintf(("\tcurPixel has fallen behind, dragging from %d to %d\n",curPixel, startX));
								curPixel = startX;
							}
							
							i = i->tail;
							if(i){
								nextEdge = i->data;
								nextX = getSmartXForLine(nextEdge, line);
								dPrintf(("\tNext edges @ x = %d from %s\n",nextX, fmtColor(nextEdge->owner->color)));
							} else {
								dPrintf(("\tNo more edges\n"));
								nextEdge = NULL;
								nextX = 0;
							}
							
							nextX = min(nextX, lineWidth);
							while ((!nextEdge && curPixel < lineWidth) || (curPixel < nextX)) {
								bool zFight = false, solitary = false;
								int32_t bestZ = 0, j = 0;
								const rb_red_blk_node *node;
								curDraw = NULL;
								dPrintf(("\tTesting depth:\n"));
								for(node = inFlags.tree.first; node != inFlags.tree.sentinel; node = TreeSuccessor((rb_red_blk_tree*)(&inFlags), node)) {
									const Primitive *prim = node->key;
									const int32_t testZ = getZForXY(prim, curPixel, line);
									if(++j == 1 || testZ <= bestZ){
										dPrintf(("\t\tHit: %d <= %d || %d == 1 for %s\n",testZ, bestZ, j,fmtColor(prim->color)));
										if (testZ == bestZ && j != 1) {
											zFight = true;
											if (prim->arity == 1) {
												curDraw = prim;
												solitary = RBSetContains(&deFlags, prim);
											}
										} else {
											zFight = false;
											bestZ = testZ;
											curDraw = prim;
											solitary = RBSetContains(&deFlags, prim);
										}
									}
								}
								
								if(curDraw){
#ifndef NDEBUG
									if(nextEdge || solitary){
#endif
										const int32_t drawWidth = (zFight || solitary) ? 1 : ((nextEdge ? nextX : lineWidth) - curPixel),
										stopPixel = curPixel + min(lineWidth - curPixel,
																	max(0, drawWidth));
										const Color drawColor = curDraw->color;
										dPrintf(("Drawing %d @ (%d, %d)\n",drawWidth,curPixel,line));
										dPrintf(("Drawing %d @ (%d, %d)\n",stopPixel - curPixel,curPixel,line));
										while(curPixel < stopPixel){
											raster[curPixel++] = drawColor;
										}
#ifndef NDEBUG
									} else {
										dPrintf(("Warning: we probably shouldn't have to draw if there are no more edges to turn us off. Look for parity errors\n");
										RBTreeClear((rb_red_blk_tree*)&inFlags));
									}
#endif
								} else if(!inFlags.tree.size && nextEdge){
									/* fast forward, we aren't in any polys */
									dPrintf(("Not in any polys at the moment, fast-forwarding(1) to %d\n", nextX));
									curPixel = nextX;
								} else {
									/* Nothing left */
									dPrintf(("Nothing to draw at end of line\n"));
									curPixel = lineWidth;
								}
								
								for(node = deFlags.first; node != deFlags.sentinel; node = TreeSuccessor(&deFlags, node)){
									RBMapRemove(&inFlags, node->key);
								}
								RBTreeClear(&deFlags);
							}
							if (!inFlags.tree.size && nextEdge) {
								dPrintf(("Not in any polys at the moment, fast-forwarding(2) to %d\n", nextX));
								curPixel = nextX;
							}
						}
					}
				}
#ifndef NDEBUG
				{
					dPrintf(("Scanning line: %d\n", line+1));
					if(inFlags.tree.size){
						rb_red_blk_node *node;
						dPrintf(("\tGarbage left in inFlags:\n"));
						for (node = inFlags.tree.first; node != inFlags.tree.sentinel; node = TreeSuccessor((rb_red_blk_tree*)&inFlags, node)) {
							dPrintf(("\t\t%s\n",fmtColor(((const Primitive*)node->key)->color)));
						}
					}
				}
#endif
				RBTreeClear(&deFlags);
				RBTreeClear((rb_red_blk_tree*)(&inFlags));
			}
			RBTreeDestroy(&activePrimSet, false);
			RBTreeDestroy(&deFlags, false);
			RBTreeDestroy((rb_red_blk_tree*)(&inFlags), false);
			for (i = 0; i < line; ++i) {
				RBTreeDestroy(scanLinePrimBuckets + i, false);
			}
		}
		
		free(projectedEdges);
	}
	free(scanLinePrimBuckets);
	free(projectedGeometry);
}

int pointerDiff(const Primitive *p1, const Primitive *p2){
	const ptrdiff_t delta = p1 - p2;
	return delta ? (delta < 0 ? -1 : 1) : 0;
}

int topToBottom(const Primitive *p1, const Primitive *p2){
	int32_t delta = topMostPrimPoint(p1) - topMostPrimPoint(p2);
#ifndef NDEBUG
	if(!delta) delta = bottomMostPrimPoint(p1) - bottomMostPrimPoint(p2);
	if(!delta) delta = leftMostPrimPoint(p1) - leftMostPrimPoint(p2);
	if(!delta) delta = rightMostPrimPoint(p1) - rightMostPrimPoint(p2);
	if(!delta) delta = p1->arity - p2->arity;
	if(!delta) delta = p1->color - p2->color;
#endif
	return delta;
}

int32_t topMostPrimPoint(const Primitive *prim){
	const int32_t arity = prim->arity;
	int32_t top, i;
	for(top = topMostEdgePoint(prim->boundary),i = 1; i < arity; ++i){
		const int32_t candidate = topMostEdgePoint(prim->boundary + i);
		if(candidate > top) top = candidate;
	}
	return top;
}

int32_t topMostEdgePoint(const Edge *edge){
	const Point * coords = edge->coords;
	return max(coords[START].y, coords[END].y);
}

int32_t bottomMostPrimPoint(const Primitive *prim){
	const int32_t arity = prim->arity;
	int32_t bottom, i;
	for(bottom = bottomMostEdgePoint(prim->boundary),i = 1; i < arity; ++i){
		const int32_t candidate = bottomMostEdgePoint(prim->boundary + i);
		if(candidate < bottom) bottom = candidate;
	}
	return bottom;
}

int32_t bottomMostEdgePoint(const Edge *edge){
	const Point * coords = edge->coords;
	return min(coords[START].y, coords[END].y);
}

#ifndef NDEBUG
int32_t rightMostPrimPoint(const Primitive *prim){
	const int32_t arity = prim->arity;
	int32_t top, i;
	for(top = rightMostEdgePoint(prim->boundary),i = 1; i < arity; ++i){
		const int32_t candidate = rightMostEdgePoint(prim->boundary + i);
		if(candidate > top) top = candidate;
	}
	return top;
}

int32_t rightMostEdgePoint(const Edge *edge){
	const Point * coords = edge->coords;
	return max(coords[START].x, coords[END].x);
}

int32_t leftMostPrimPoint(const Primitive *prim){
	const int32_t arity = prim->arity;
	int32_t bottom, i;
	for(bottom = leftMostEdgePoint(prim->boundary),i = 1; i < arity; ++i){
		const int32_t candidate = leftMostEdgePoint(prim->boundary + i);
		if(candidate < bottom) bottom = candidate;
	}
	return bottom;
}

int32_t leftMostEdgePoint(const Edge *edge){
	const Point * coords = edge->coords;
	return min(coords[START].x, coords[END].x);
}
#endif
