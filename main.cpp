#include "sdk/amxxmodule.h"

#include <btBulletDynamicsCommon.h>
#include <stdio.h>

#include <map>

#include "object.h"
#include "model.h"

//#define BULLET_TRIANGLE_COLLISION 1
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

#define BMOD_GRAVITY -525 //-10m/s^2 -> -1000cm/s^2 -> convert to game units (1/1.905f)

//expecting min 10 FPS
int g_bt_max_ssteps = 6;
float g_bt_ftstep = 1.0 / 60;

char g_game_dir[64];
char g_bspname[260];

std::map<int, bmodObject*> g_bmod_objects;

extern AMX_NATIVE_INFO amxxfunctions[];

//shitload of bullet pointers
btBroadphaseInterface* g_bt_broadphase;
btDefaultCollisionConfiguration* g_bt_collisionConfiguration;
btCollisionDispatcher* g_bt_dispatcher;
btSequentialImpulseConstraintSolver* g_bt_solver;
btDiscreteDynamicsWorld* g_bt_dynamicsWorld;

void OnAmxxAttach() {
	MF_AddNatives(amxxfunctions);

	GET_GAME_DIR(g_game_dir);

	//shitload of bullet init stuff
	g_bt_broadphase = new btDbvtBroadphase();
	g_bt_collisionConfiguration = new btDefaultCollisionConfiguration();
	g_bt_dispatcher = new btCollisionDispatcher(g_bt_collisionConfiguration);
	g_bt_solver = new btSequentialImpulseConstraintSolver;
	g_bt_dynamicsWorld = new btDiscreteDynamicsWorld(g_bt_dispatcher, g_bt_broadphase, g_bt_solver, g_bt_collisionConfiguration);
	g_bt_dynamicsWorld->setGravity(btVector3(0, 0, BMOD_GRAVITY));

	btGImpactCollisionAlgorithm::registerAlgorithm(g_bt_dispatcher);

	MF_Log("attached");
	RETURN_META(MRES_IGNORED);
}

void ServerActivate_Post(edict_t *pEdictList, int edictCount, int clientMax) {
	sprintf(g_bspname, "maps/%s.bsp", STRING(gpGlobals->mapname));

	MF_Log("activated");
	RETURN_META(MRES_IGNORED);
}

void StartFrame() {
	static float oldtime = 0;
	float newtime = g_engfuncs.pfnTime();
	//step it
	g_bt_dynamicsWorld->stepSimulation(newtime - oldtime, g_bt_max_ssteps, g_bt_ftstep);
	oldtime = newtime;
	RETURN_META(MRES_IGNORED);
}

void ServerDeactivate_Post() {
	//unload objects
	for(std::map<int, bmodObject*>::iterator it = g_bmod_objects.begin(); it != g_bmod_objects.end(); ++it) {
		delete it->second;
	}
	g_bmod_objects.clear();

	//clear models
	clearModels();

	MF_Log("deactivated");
	RETURN_META(MRES_IGNORED);
}

void OnAmxxDetach() {
	//unload bullet stuff
	delete g_bt_dynamicsWorld;
	delete g_bt_solver;
	delete g_bt_collisionConfiguration;
	delete g_bt_dispatcher;
	delete g_bt_broadphase;

	MF_Log("detached");
	RETURN_META(MRES_IGNORED);
}