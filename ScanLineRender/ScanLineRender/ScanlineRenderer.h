/*
//  ScanlineRenderer.h
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/27/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#ifndef ScanlineRenderer_h
#define ScanlineRenderer_h

#include "Primitive.h"
#include "Transformation.h"
#include "rb_tree/red_black_tree.h"

void transformData(const Transformation *txForm, const Point *srcGeometry, Point *dstGeometry, size_t pointCount);
rb_red_blk_tree* teardownBuckets(rb_red_blk_tree* buckets, int numLines);
rb_red_blk_tree* bucketPrims(rb_red_blk_tree* bucketsRef, int numLines, Primitive *geometry, size_t geomCount);
void render(Color *raster, int lineWidth, int numLines, const rb_red_blk_tree *scanLinePrimBuckets);

#endif /* ScanlineRenderer_h */
