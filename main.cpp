#include "sdk/amxxmodule.h"

#include <btBulletDynamicsCommon.h>
#include <stdio.h>

#include <vector>

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

std::vector<bmodObject*> g_bmod_objects;

extern AMX_NATIVE_INFO amxxfunctions[];

//shitload of bullet pointers
btBroadphaseInterface* g_bt_broadphase;
btDefaultCollisionConfiguration* g_bt_collisionConfiguration;
btCollisionDispatcher* g_bt_dispatcher;
btSequentialImpulseConstraintSolver* g_bt_solver;
btDiscreteDynamicsWorld* g_bt_dynamicsWorld;

void OnAmxxAttach() {
	MF_Log("attached");
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

	RETURN_META(MRES_IGNORED);
}

//tris_s * g_bmod_tris;

void ServerActivate_Post(edict_t *pEdictList, int edictCount, int clientMax) {
	MF_Log("activated");

	sprintf(g_bspname, "maps/%s.bsp", STRING(gpGlobals->mapname));

	//TODO load map and handle entities
	/*btCollisionShape * g_bmod_mapShape = new btStaticPlaneShape(btVector3(0, 0, 1), 0);
	btDefaultMotionState* motionState = new btDefaultMotionState();
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState, g_bmod_mapShape, btVector3(0, 0, 0));
	btRigidBody * g_bmod_mapBody = new btRigidBody(rigidBodyCI);
	g_bt_dynamicsWorld->addRigidBody(g_bmod_mapBody);
	*/

	//get bsp path
	//char gamedir[16];
	//GET_GAME_DIR(gamedir);
	//char bspname[64];
	//sprintf(bspname, "%s/maps/%s.bsp", gamedir, STRING(gpGlobals->mapname));
	//load the map

	//g_bmod_tris = bsp2tris(bspname, 0);
	//btTriangleIndexVertexArray * map_trimesh = new btTriangleIndexVertexArray(g_bmod_tris->indices_c / 3, g_bmod_tris->indices, 3 * sizeof(int), g_bmod_tris->vertices_c, g_bmod_tris->vertices, 3 * sizeof(float));
	//g_bmod_mapShape = new btBvhTriangleMeshShape(map_trimesh, true);
	//btDefaultMotionState* motionState = new btDefaultMotionState();
	//btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState, g_bmod_mapShape, btVector3(0, 0, 0));
	//g_bmod_mapBody = new btRigidBody(rigidBodyCI);
	//g_bt_dynamicsWorld->addRigidBody(g_bmod_mapBody);

	RETURN_META(MRES_IGNORED);
}

void StartFrame() {
	//kinematic objects should be updated automatically
	//update immovable objects
	/*for(std::vector<bmodObject*>::iterator it = g_bmod_objects.begin(); it != g_bmod_objects.end(); ++it) {
		//MF_Log("%d: %d %f", it - g_bmod_objects.begin(), (*it)->isImmovable(), INDEXENT((*it)->getEntities()->back())->v.origin.z);
		if((*it)->getRigidBody()->isStaticObject())
			(*it)->update();
	}*/

	static float oldtime = 0;
	float newtime = g_engfuncs.pfnTime();
	//step it
	g_bt_dynamicsWorld->stepSimulation(newtime - oldtime, g_bt_max_ssteps, g_bt_ftstep);
	oldtime = newtime;
	RETURN_META(MRES_IGNORED);
}

void ServerDeactivate_Post() {
	MF_Log("deactivated");

	//unload objects
	for(std::vector<bmodObject*>::iterator it = g_bmod_objects.begin(); it != g_bmod_objects.end(); ++it) {
		delete *it;
	}
	g_bmod_objects.clear();
	g_bmod_objects.resize(0);

	//clear models
	clearModels();


	//unload the map
	//g_bt_dynamicsWorld->removeRigidBody(g_bmod_mapBody);
	//delete g_bmod_mapBody->getMotionState();
	//delete g_bmod_mapBody;
	//delete g_bmod_mapShape;
	//free(g_bmod_tris->indices);
	//free(g_bmod_tris->vertices);
	//free(g_bmod_tris);
	RETURN_META(MRES_IGNORED);
}

void OnAmxxDetach() {
	MF_Log("detached");

	g_bmod_objects.shrink_to_fit();

	//unload bullet stuff
	delete g_bt_dynamicsWorld;
	delete g_bt_solver;
	delete g_bt_collisionConfiguration;
	delete g_bt_dispatcher;
	delete g_bt_broadphase;

	RETURN_META(MRES_IGNORED);
}