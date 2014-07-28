#include "model.h"
#include <map>
#include <string>
#include "bspfile.h"

typedef std::map<std::string, btCollisionShape*> modelmap;

modelmap models;

extern char g_game_dir[];
extern char g_bspname[];

bool getBMODShape(const char * model, btCollisionShape** shape) {
	printf("lurki ng\n%s\n", model);
	float x, y, z;
	if(sscanf(model, "box/%f/%f/%f", &x, &y, &z) == 3) {
		*shape = new btBoxShape(btVector3(x, y, z));
	}
	else if(sscanf(model, "capsule/%f/%f", &x, &y) == 2) {
		*shape = new btCapsuleShapeZ(x, y);
	}
	else if(sscanf(model, "cone/%f/%f", &x, &y) == 2) {
		*shape = new btConeShapeZ(x, y);
	}
	else if(sscanf(model, "cylinder/%f/%f/%f", &x, &y, &z) == 3) {
		*shape = new btCylinderShapeZ(btVector3(x, y, z));
	}
	else if(sscanf(model, "sphere/%f", &x) == 1) {
		*shape = new btSphereShape(x);
	}
	else {
		return false;
	}
	return true;
}

inline int ___bsp_get_vertex(int edge, int * surfedges, dedge_t * edges) {
	int se = surfedges[edge];
	return (se < 0) ? edges[-se].v[1] : edges[se].v[0];
}

bool getBSPShape(FILE * bspfile, int modelnum, btCollisionShape ** shape) {
	//modelnum = 4;
	//read header
	dheader_t header;
	fseek(bspfile, 0, SEEK_SET);
	fread(&header, sizeof(header), 1, bspfile);

	//read model
	dmodel_t model;
	fseek(bspfile, header.lumps[LUMP_MODELS].fileofs + sizeof(model)*modelnum, SEEK_SET);
	fread(&model, sizeof(model), 1, bspfile);

	if(!model.numfaces)
		return false;

	//alloc & read faces, surfedges, edges, vertices
	fseek(bspfile, header.lumps[LUMP_FACES].fileofs, SEEK_SET);
	dface_t * faces = new dface_t[header.lumps[LUMP_FACES].filelen / sizeof(dface_t)];
	fread(faces, 1, header.lumps[LUMP_FACES].filelen, bspfile);

	fseek(bspfile, header.lumps[LUMP_SURFEDGES].fileofs, SEEK_SET);
	int * surfedges = new int[header.lumps[LUMP_SURFEDGES].filelen / sizeof(int)];
	fread(surfedges, 1, header.lumps[LUMP_SURFEDGES].filelen, bspfile);

	fseek(bspfile, header.lumps[LUMP_EDGES].fileofs, SEEK_SET);
	dedge_t * edges = new dedge_t[header.lumps[LUMP_EDGES].filelen / sizeof(dedge_t)];
	fread(edges, 1, header.lumps[LUMP_EDGES].filelen, bspfile);

	fseek(bspfile, header.lumps[LUMP_VERTEXES].fileofs, SEEK_SET);
	dvertex_t * vertices = new dvertex_t[header.lumps[LUMP_VERTEXES].filelen / sizeof(dvertex_t)];
	fread(vertices, 1, header.lumps[LUMP_VERTEXES].filelen, bspfile);

	//alloc space for indices
	//let's say there are faces with maximum of 32 vertices
	int * temp_indices = new int[model.numfaces * 3 * 32];
	int tri_indices_c = 0;

	//int se, v, index = 0, temp_vertices = 0;
	for(int i_f = model.firstface; i_f < model.firstface + model.numfaces; i_f++) {
		int v1 = ___bsp_get_vertex(faces[i_f].firstedge, surfedges, edges);
		int v2 = ___bsp_get_vertex(faces[i_f].firstedge + 1, surfedges, edges);
		for(int i_v = faces[i_f].firstedge + 2; i_v < faces[i_f].firstedge + faces[i_f].numedges; i_v++) {
			int v3 = ___bsp_get_vertex(i_v, surfedges, edges);
			temp_indices[tri_indices_c++] = v1;
			temp_indices[tri_indices_c++] = v2;
			temp_indices[tri_indices_c++] = v3;
			v2 = v3;
		}
	}

	//get used portion of vertices
	int min = temp_indices[0];
	int max = temp_indices[0];
	for(int i = 1; i < tri_indices_c; i++) {
		if(temp_indices[i] > max)
			max = temp_indices[i];
		if(temp_indices[i] < min)
			min = temp_indices[i];
	}
	max++;

	//alloc space for vertices and copy them
	int tri_vertices_c = max - min;
	float * tri_vertices = new float[tri_vertices_c * 3];
	for(int i = 0; i < tri_vertices_c; i++) {
		tri_vertices[i * 3 + 0] = vertices[min + i].point[0];
		tri_vertices[i * 3 + 1] = vertices[min + i].point[1];
		tri_vertices[i * 3 + 2] = vertices[min + i].point[2];
	}

	//alloc space for indices and copy them
	int * tri_indices = new int[tri_indices_c];
	for(int i = 0; i < tri_indices_c; i++) {
		tri_indices[i] = temp_indices[i] - min;
	}

	//pass trimesh to bullet
	btTriangleIndexVertexArray * map_trimesh = new btTriangleIndexVertexArray(tri_indices_c / 3, tri_indices, 3 * sizeof(int), tri_vertices_c, tri_vertices, 3 * sizeof(float));
	*shape = new btBvhTriangleMeshShape(map_trimesh, true);


	//freedom!
	delete faces;
	delete surfedges;
	delete edges;
	delete vertices;

	delete temp_indices;

	//delete tri_vertices;
	//delete tri_indices;

	return true;
}

FILE * fmdlopen(const char * model) {
	char path[260];
	sprintf(path, "%s/%s", g_game_dir, model);
	FILE * file = fopen(path, "rb");

	//fucking steam and his fetish in splitting files into shitload of useless folders
	if(!file) {
		sprintf(path, "%s_downloads/%s", g_game_dir, model);
		file = fopen(path, "rb");
	}
	return file;
}

bool getMAPShape(const char * model, btCollisionShape** shape) {
	int modelnum = atoi(model);
	FILE * file = fmdlopen(g_bspname);
	if(!file)
		return false;
	if(!getBSPShape(file, modelnum, shape)) {
		fclose(file);
		return false;
	}
	fclose(file);
	return true;
}

bool getMDLShape(FILE * mdlfile, btCollisionShape** shape) {
	//TODO MDL HANDLE
	return false;
}

bool getFILEShape(FILE * file, btCollisionShape** shape) {
	unsigned long magic;
	fread(&magic, 4, 1, file);
	if(magic == 0x0000001E) {
		if(!getBSPShape(file, 0, shape))
			return false;
	}
	else if(magic == 0x54534449) {
		if(!getMDLShape(file, shape))
			return false;
	}
	else
		return false;
	return true;
}

bool getModelShape(const char * model, btCollisionShape** shape) {
	std::string mdlstr(model);

	//does model already exist?
	modelmap::iterator el = models.find(mdlstr);
	if(el != models.end()) {
		printf("\n\nFOUDN!!!\n\n\n");
		*shape = el->second;
		return true;
	}

	//model doesn't exist, let's create one
	//is it bullet primitive?
	if(!strncmp(model, "BMOD/", 5)) {
		printf("bmodacky kokot\n");
		if(!getBMODShape(model + 5, shape))
			return false;
	}
	//is it map model?
	else if(model[0] == '*') {
		if(!getMAPShape(model + 1, shape))
			return false;
	}
	//is it bsp/mdl file model?
	else if(FILE * file = fmdlopen(model)) {
		if(!getFILEShape(file, shape)) {
			fclose(file);
			return false;
		}
	}
	else {
		printf("\n\nnot found..\n\n\n");
		return false;
	}
	printf("\n\nsaved!!!\n\n\n");
	models[mdlstr] = *shape;
	return true;
}

void clearModels() {
	for(modelmap::iterator it = models.begin(); it != models.end(); ++it) {
		delete it->second;
	}
	models.clear();
}