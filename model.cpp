#include "model.h"
#include <map>
#include <string>

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

bool getBSPShape(FILE * bspfile, int modelnum, btCollisionShape ** shape) {
	//TODO BSP HANDLE
	return false;
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