/*
//  main.cpp
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "ScanlineRenderer.h"
#include <netpbm/ppm.h>
#include <string.h>

typedef enum {
	RED = 0xf800,
	MAGENTA = 0xf81f,
	GREEN = 0x07e0,
	ORANGE = 0xfe40,
	YELLOW = 0xffe0,
	PINK = 0xfd75,
	BLUE = 0x001f
} ColorCodes16BPP;

typedef struct {
	int32_t x;
	int32_t y;
	int32_t zNum;
	int32_t zDen;
	int32_t scale;
} DrawCoords;

static DrawCoords drawCoords = {170, 40, 7, 10, 100};

void viewF(const Point *p, Point *o, const DrawCoords *offset);
void viewF(const Point *p, Point *o, const DrawCoords *offset){
	const int32_t x = offset->scale * p->x,
	 y = offset->scale * p->y,
	z = offset->scale * p->z,
	zOff = (int32_t)((z * offset->zNum) / (float)offset->zDen);
	INIT_POINT(*o, offset->x + x + zOff, offset->y + y + zOff, z);
}

const Transformation viewProj = {(TransformationF)(&viewF),&drawCoords};



int main(int argc, const char * argv[]) {
	const static Point cubePointsSrc[8] =
	{{0,0,0},{0,0,1},{0,1,0},{0,1,1},
		{1,0,0},{1,0,1},{1,1,0},{1,1,1}};
	static Point cubePoints[8];
	static Point* cubeEdges[3][4][2] = {
		{
			{&cubePoints[0],&cubePoints[1]},
			{&cubePoints[2],&cubePoints[3]},
			{&cubePoints[4],&cubePoints[5]},
			{&cubePoints[6],&cubePoints[7]}},
		{
			{&cubePoints[0],&cubePoints[2]},
			{&cubePoints[1],&cubePoints[3]},
			{&cubePoints[4],&cubePoints[6]},
			{&cubePoints[5],&cubePoints[7]}},
		{
			{&cubePoints[0],&cubePoints[4]},
			{&cubePoints[1],&cubePoints[5]},
			{&cubePoints[2],&cubePoints[6]},
			{&cubePoints[3],&cubePoints[7]}}};
	
	static Point* cubeFaces[6][5] = {
		{&cubePoints[0],&cubePoints[1],&cubePoints[3],&cubePoints[2],&cubePoints[0]},
		{&cubePoints[7],&cubePoints[6],&cubePoints[4],&cubePoints[5],&cubePoints[7]},
		{&cubePoints[5],&cubePoints[1],&cubePoints[3],&cubePoints[7],&cubePoints[5]},
		{&cubePoints[4],&cubePoints[6],&cubePoints[2],&cubePoints[0],&cubePoints[4]},
		{&cubePoints[0],&cubePoints[1],&cubePoints[5],&cubePoints[4],&cubePoints[0]},
		{&cubePoints[2],&cubePoints[3],&cubePoints[7],&cubePoints[6],&cubePoints[2]},
	};
	
	
	static Primitive cube[] = {
		{cubeFaces[0], 4, RED},
		{cubeFaces[1], 4, MAGENTA},
		{cubeFaces[2], 4, GREEN},
		{cubeFaces[3], 4, ORANGE},
		{cubeFaces[4], 4, YELLOW},
		{cubeFaces[5], 4, PINK},
		{cubeEdges[0][0], 1, BLUE},
		{cubeEdges[0][1], 1, BLUE},
		{cubeEdges[0][2], 1, BLUE},
		{cubeEdges[0][3], 1, BLUE},
		{cubeEdges[1][0], 1, BLUE},
		{cubeEdges[1][1], 1, BLUE},
		{cubeEdges[1][2], 1, BLUE},
		{cubeEdges[1][3], 1, BLUE},
		{cubeEdges[2][0], 1, BLUE},
		{cubeEdges[2][1], 1, BLUE},
		{cubeEdges[2][2], 1, BLUE},
		{cubeEdges[2][3], 1, BLUE}
	};
	
	const int32_t numLines = 240;
	const int32_t lineWidth = 320;
	const size_t rasterByteCount = numLines * lineWidth * sizeof(Color);
	Color *const raster = (Color*)SafeMalloc(rasterByteCount);
	rb_red_blk_tree* buckets = NULL;
	pm_proginit(&argc, argv);
	
	transformData(&viewProj, cubePointsSrc, cubePoints, 8);
	buckets = bucketPrims(buckets, numLines, cube, sizeof(cube) / sizeof(Primitive));
	
	memset(raster, 0xff, rasterByteCount);
	render(raster, lineWidth, numLines, buckets);
	
	buckets = teardownBuckets(buckets, numLines);
	
	{
		pixel **const ppm_raster = ppm_allocarray(lineWidth, numLines);
		size_t x,y;
		FILE *fp;
		
		for(y = 0; y < numLines; y++){
			for(x = 0; x < lineWidth; x++){
				const Color rgb = raster[y * lineWidth + x];
				const uint32_t r = (rgb >> 11) << 3,
				g = ((rgb >> 5) & 0x3f) << 2,
				b = (rgb & 0x1f) << 3;
				ppm_raster[y][x].r = r;
				ppm_raster[y][x].g = g;
				ppm_raster[y][x].b = b;		}
		}
		
		if((fp = fopen("out.ppm", "wb"))){
			ppm_writeppm(fp, ppm_raster, lineWidth, numLines, 255, 0);
			fclose(fp);
		}
		
		ppm_freearray(ppm_raster, numLines);
	}
	free(raster);
	
    return 0;
}
