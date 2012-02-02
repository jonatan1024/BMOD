// bsp2tris_test.cpp : Defines the entry point for the console application.
//

#include "bsp2tris.h"
#include "bspfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

bool vnil(const float * vertex, int pos, float nvertex[3], int scale){
	return (
		fabs((vertex[pos+3]-vertex[pos])*(nvertex[1]/scale-vertex[pos+4])-
		(vertex[pos+4]-vertex[pos+1])*(nvertex[0]/scale-vertex[pos+3]))>=1.0/scale
		||
		fabs((vertex[pos+4]-vertex[pos+1])*(nvertex[2]/scale-vertex[pos+5])-
		(vertex[pos+5]-vertex[pos+2])*(nvertex[1]/scale-vertex[pos+4]))>=1.0/scale
		||
		fabs((vertex[pos+3]-vertex[pos+0])*(nvertex[2]/scale-vertex[pos+5])-
		(vertex[pos+5]-vertex[pos+2])*(nvertex[0]/scale-vertex[pos+3]))>=1.0/scale
		);
}

tris_s bsp2tris(const char* bsp_c,int scale,int model)
{
	//nacteni souboru
	FILE * bsp;
	bsp = fopen(bsp_c,"rb");
    fseek(bsp, 0, SEEK_END);
	long bsp_l = ftell(bsp);
    fseek(bsp, 0, SEEK_SET);
	char * bsp_r = (char*)malloc(bsp_l+1);
	fread(bsp_r, 1, bsp_l, bsp);
	fclose(bsp);
	////
	//nacteni lumpu
	dheader_t * header = (dheader_t*)bsp_r;
	dmodel_t * models = (dmodel_t*)(bsp_r+header->lumps[LUMP_MODELS].fileofs);
	int models_c = header->lumps[LUMP_MODELS].filelen/sizeof(dmodel_t);
	dface_t * faces = (dface_t*)(bsp_r+header->lumps[LUMP_FACES].fileofs);
	//int faces_c = header->lumps[LUMP_FACES].filelen/sizeof(dface_t);
	int * surfedges = (int*)(bsp_r+header->lumps[LUMP_SURFEDGES].fileofs);
	dedge_t * edges = (dedge_t*)(bsp_r+header->lumps[LUMP_EDGES].fileofs);
	dvertex_t * vertices = (dvertex_t*)(bsp_r+header->lumps[LUMP_VERTEXES].fileofs);
	/*char * entities = (char*)(bsp_r+header->lumps[LUMP_ENTITIES].fileofs);
	int entities_c = header->lumps[LUMP_ENTITIES].filelen;
	printf(entities);*/
	////
	//bsp2tris
	tris_s tris;
	tris.vertices = (float *) malloc(header->lumps[LUMP_VERTEXES].filelen*3*sizeof(float));
	tris.indices = (int *) malloc(header->lumps[LUMP_VERTEXES].filelen*3*sizeof(int));
	tris.vertices_c=0;
	tris.indices_c=0;
	int se,v,i_v,i_f,index=0,temp_vertices=0;
	for(i_f=models[model].firstface;i_f<models[model].firstface+models[model].numfaces;i_f++){
		for(i_v=0;i_v<faces[i_f].numedges;i_v++){
			se = surfedges[faces[i_f].firstedge + i_v];
			if (se < 0)
			{
				v = edges[-se].v[1];
			}
			else
			{
				v = edges[se].v[0];
			}
			if(i_v>=2){
				if(!vnil(tris.vertices,tris.vertices_c-6,vertices[v].point,scale)){
					tris.vertices_c-=3;
				}
			}
			for(int i=0;i<3;i++){
				tris.vertices[tris.vertices_c++] = vertices[v].point[i]/scale;
			}
		}
		temp_vertices=tris.vertices_c/3;
		if(temp_vertices-index<3)
			tris.vertices_c-=3*(temp_vertices-index);
		for(i_v=0;i_v<temp_vertices-index;i_v++){
			if(i_v+2<temp_vertices-index){
				tris.indices[tris.indices_c++]=index;
				tris.indices[tris.indices_c++]=index+i_v+2;
				tris.indices[tris.indices_c++]=index+i_v+1;
			}
		}
		index=tris.vertices_c/3;
	}
	tris.vertices = (float*)realloc(tris.vertices,tris.vertices_c*3*sizeof(float));
	tris.indices = (int*)realloc(tris.indices,tris.indices_c*sizeof(int));
	free(bsp_r);
	return tris;
}