/*
//  main.cpp
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "ScanlineRenderer.h"
#include <netpbm/ppm.h>
#include <memory.h>

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

const Projection viewProj = {(ProjectionF)(&viewF),&drawCoords};



int main(int argc, const char * argv[]) {
	static const Point cubePoints[2][2][2] =
	{{{{0,0,0},{0,0,1}},{{0,1,0},{0,1,1}}},
		{{{1,0,0},{1,0,1}},{{1,1,0},{1,1,1}}}};
	static Edge cubeEdges[3][4];
	static Edge cubeFaces[6][4];
	static Primitive cubeAndSkel[18];
	int32_t numLines = 240;
	int32_t lineWidth = 320;
	size_t rasterByteCount = numLines * lineWidth * sizeof(Color);
	Color *raster = (Color*)malloc(rasterByteCount);
	pixel** ppm_raster;
	size_t x,y;
	FILE *fp;
	
	pm_proginit(&argc, argv);

	
	INIT_EDGE(cubeEdges[0][0],cubePoints[0][0][0],cubePoints[0][0][1]);
	INIT_EDGE(cubeEdges[0][1],cubePoints[0][1][0],cubePoints[0][1][1]);
	INIT_EDGE(cubeEdges[0][2],cubePoints[1][0][0],cubePoints[1][0][1]);
	INIT_EDGE(cubeEdges[0][3],cubePoints[1][1][0],cubePoints[1][1][1]);
	INIT_EDGE(cubeEdges[1][0],cubePoints[0][0][0],cubePoints[0][1][0]);
	INIT_EDGE(cubeEdges[1][1],cubePoints[0][0][1],cubePoints[0][1][1]);
	INIT_EDGE(cubeEdges[1][2],cubePoints[1][0][0],cubePoints[1][1][0]);
	INIT_EDGE(cubeEdges[1][3],cubePoints[1][0][1],cubePoints[1][1][1]);
	INIT_EDGE(cubeEdges[2][0],cubePoints[0][0][0],cubePoints[1][0][0]);
	INIT_EDGE(cubeEdges[2][1],cubePoints[0][0][1],cubePoints[1][0][1]);
	INIT_EDGE(cubeEdges[2][2],cubePoints[0][1][0],cubePoints[1][1][0]);
	INIT_EDGE(cubeEdges[2][3],cubePoints[0][1][1],cubePoints[1][1][1]);
	
	cubeFaces[0][0] = cubeEdges[0][0];
	cubeFaces[0][1] = cubeEdges[1][0];
	cubeFaces[0][2] = cubeEdges[0][1];
	cubeFaces[0][3] = cubeEdges[1][1];
	cubeFaces[1][0] = cubeEdges[0][2];
	cubeFaces[1][1] = cubeEdges[1][3];
	cubeFaces[1][2] = cubeEdges[0][3];
	cubeFaces[1][3] = cubeEdges[1][2];
	cubeFaces[2][0] = cubeEdges[1][1];
	cubeFaces[2][1] = cubeEdges[2][3];
	cubeFaces[2][2] = cubeEdges[1][3];
	cubeFaces[2][3] = cubeEdges[2][1];
	cubeFaces[3][0] = cubeEdges[1][2];
	cubeFaces[3][1] = cubeEdges[2][2];
	cubeFaces[3][2] = cubeEdges[1][0];
	cubeFaces[3][3] = cubeEdges[2][0];
	cubeFaces[4][0] = cubeEdges[0][0];
	cubeFaces[4][1] = cubeEdges[2][1];
	cubeFaces[4][2] = cubeEdges[0][2];
	cubeFaces[4][3] = cubeEdges[2][0];
	cubeFaces[5][0] = cubeEdges[0][1];
	cubeFaces[5][1] = cubeEdges[2][2];
	cubeFaces[5][2] = cubeEdges[0][3];
	cubeFaces[5][3] = cubeEdges[2][3];
	
	/* Faces here */
	INIT_PRIM(cubeAndSkel[0], RED,		4, cubeFaces[0]);
	INIT_PRIM(cubeAndSkel[1], MAGENTA,	4, cubeFaces[1]);
	INIT_PRIM(cubeAndSkel[2], GREEN,		4, cubeFaces[2]);
	INIT_PRIM(cubeAndSkel[3], ORANGE,	4, cubeFaces[3]);
	INIT_PRIM(cubeAndSkel[4], YELLOW,	4, cubeFaces[4]);
	INIT_PRIM(cubeAndSkel[5], PINK,		4, cubeFaces[5]);
	/* Skeleton here */
	INIT_PRIM(cubeAndSkel[6], BLUE, 1, cubeEdges[0] + 0);
	INIT_PRIM(cubeAndSkel[7], BLUE, 1, cubeEdges[0] + 1);
	INIT_PRIM(cubeAndSkel[8], BLUE, 1, cubeEdges[0] + 2);
	INIT_PRIM(cubeAndSkel[9], BLUE, 1, cubeEdges[0] + 3);
	INIT_PRIM(cubeAndSkel[10], BLUE, 1, cubeEdges[1] + 0);
	INIT_PRIM(cubeAndSkel[11], BLUE, 1, cubeEdges[1] + 1);
	INIT_PRIM(cubeAndSkel[12], BLUE, 1, cubeEdges[1] + 2);
	INIT_PRIM(cubeAndSkel[13], BLUE, 1, cubeEdges[1] + 3);
	INIT_PRIM(cubeAndSkel[14], BLUE, 1, cubeEdges[2] + 0);
	INIT_PRIM(cubeAndSkel[15], BLUE, 1, cubeEdges[2] + 1);
	INIT_PRIM(cubeAndSkel[16], BLUE, 1, cubeEdges[2] + 2);
	INIT_PRIM(cubeAndSkel[17], BLUE, 1, cubeEdges[2] + 3);
	
	memset(raster, 0xff, rasterByteCount);
	render(raster, lineWidth, numLines, cubeAndSkel, 18, &viewProj);
	
	ppm_raster = ppm_allocarray(lineWidth, numLines);
	
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
	
	
	
    return 0;
}
