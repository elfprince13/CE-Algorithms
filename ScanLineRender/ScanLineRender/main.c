/*
//  main.cpp
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "ScanlineRenderer.h"

typedef enum {
	RED = 0xf800,
	MAGENTA = 0xf81f,
	GREEN = 0x07e0,
	ORANGE = 0xfe40,
	YELLOW = 0xffe0,
	PINK = 0xfd75,
	BLUE = 0x01f
} ColorCodes16BPP;

typedef struct {
	int16_t x;
	int16_t y;
	int16_t zNum;
	int16_t zDen;
} DrawCoords;

static DrawCoords drawCoords = {170, 40, 7, 10};

void viewF(const Point *p, Point *o, const DrawCoords *offset);
void viewF(const Point *p, Point *o, const DrawCoords *offset){
	int16_t zOff = (p->z * offset->zNum) / offset->zDen;
	INIT_POINT(*o, offset->x + p->x + zOff, offset->y + p->y + zOff, p->z);
}

const Projection viewProj = {(ProjectionF)(&viewF),&drawCoords};



int main(int argc, const char * argv[]) {
	static const Point cubePoints[2][2][2] =
	{{{{0,0,0},{0,0,1}},{{0,1,0},{0,1,1}}},
		{{{1,0,0},{1,0,1}},{{1,1,0},{1,1,1}}}};
	static Edge cubeEdges[3][4];
	static Edge cubeFaces[6][4];
	static Primitive cubeAndSkel[18];
	int16_t numLines = 240;
	int16_t lineWidth = 320;
	Color *raster = (Color*)calloc(numLines * lineWidth, sizeof(Color));

	
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
	INIT_PRIM(cubeAndSkel[00], RED,		4, cubeFaces[0]);
	INIT_PRIM(cubeAndSkel[00], MAGENTA,	4, cubeFaces[1]);
	INIT_PRIM(cubeAndSkel[00], GREEN,		4, cubeFaces[2]);
	INIT_PRIM(cubeAndSkel[00], ORANGE,	4, cubeFaces[3]);
	INIT_PRIM(cubeAndSkel[00], YELLOW,	4, cubeFaces[4]);
	INIT_PRIM(cubeAndSkel[00], PINK,		4, cubeFaces[5]);
	/* Skeleton here */
	INIT_PRIM(cubeAndSkel[00], BLUE, 1, cubeEdges[0] + 0);
	INIT_PRIM(cubeAndSkel[00], BLUE, 1, cubeEdges[0] + 1);
	INIT_PRIM(cubeAndSkel[00], BLUE, 1, cubeEdges[0] + 2);
	INIT_PRIM(cubeAndSkel[00], BLUE, 1, cubeEdges[0] + 3);
	INIT_PRIM(cubeAndSkel[00], BLUE, 1, cubeEdges[1] + 0);
	INIT_PRIM(cubeAndSkel[00], BLUE, 1, cubeEdges[1] + 1);
	INIT_PRIM(cubeAndSkel[00], BLUE, 1, cubeEdges[1] + 2);
	INIT_PRIM(cubeAndSkel[00], BLUE, 1, cubeEdges[1] + 3);
	INIT_PRIM(cubeAndSkel[00], BLUE, 1, cubeEdges[2] + 0);
	INIT_PRIM(cubeAndSkel[00], BLUE, 1, cubeEdges[2] + 1);
	INIT_PRIM(cubeAndSkel[00], BLUE, 1, cubeEdges[2] + 2);
	INIT_PRIM(cubeAndSkel[00], BLUE, 1, cubeEdges[2] + 3);
	
	render(raster, lineWidth, numLines, cubeAndSkel, 18, &viewProj);
	
    return 0;
}
