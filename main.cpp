#include "sdk/amxxmodule.h"

#include <btBulletDynamicsCommon.h>
#include <stdio.h>

#include "object.h"
#include "bsp2tris.h"
#include "objectlist.h"

#define BMOD_GRAVITY -525 //-10m/s^2 -> -1000cm/s^2 -> convert to game units (1/1.905f)
#define BMOD_FPS 60
#define BMOD_MAX_STEPS 10

bmodObjectList * g_bmod_objects;

static cell AMX_NATIVE_CALL bmod_object_add(AMX *amx, cell *params){
	return g_bmod_objects->add(new bmodObject(INDEXENT(params[1])));
}

static cell AMX_NATIVE_CALL bmod_object_remove(AMX *amx, cell *params){
	return g_bmod_objects->remove(INDEXENT(params[1]));
}

AMX_NATIVE_INFO amxxfunctions[] = {
	{"bmod_object_add",bmod_object_add},
	{"bmod_object_remove",bmod_object_remove},
	{NULL, NULL}
};

//shitload of bullet pointers
btBroadphaseInterface* g_bt_broadphase;
btDefaultCollisionConfiguration* g_bt_collisionConfiguration;
btCollisionDispatcher* g_bt_dispatcher;
btSequentialImpulseConstraintSolver* g_bt_solver;
btDiscreteDynamicsWorld* g_bt_dynamicsWorld;

void OnAmxxAttach()
{
	printf("[BMOD] attached\n");
	MF_AddNatives(amxxfunctions);

	//shitload of bullet init stuff
	g_bt_broadphase = new btDbvtBroadphase();
	g_bt_collisionConfiguration = new btDefaultCollisionConfiguration();
    g_bt_dispatcher = new btCollisionDispatcher(g_bt_collisionConfiguration);
	g_bt_solver = new btSequentialImpulseConstraintSolver;
	g_bt_dynamicsWorld = new btDiscreteDynamicsWorld(g_bt_dispatcher,g_bt_broadphase,g_bt_solver,g_bt_collisionConfiguration);
	g_bt_dynamicsWorld->setGravity(btVector3(0,0,BMOD_GRAVITY));
	RETURN_META(MRES_IGNORED);
}

btRigidBody* g_bmod_mapBody;
btCollisionShape* g_bmod_mapShape;
tris_s * g_bmod_tris;

void ServerActivate_Post(edict_t *pEdictList, int edictCount, int clientMax)
{
	printf("[BMOD] activated\n");

	//get bsp path
	char gamedir[16];
	GET_GAME_DIR(gamedir);
	char bspname[64];
	sprintf(bspname,"%s/maps/%s.bsp",gamedir,STRING(gpGlobals->mapname));
	//load the map
	//g_bmod_mapShape = new btStaticPlaneShape(btVector3(0,0,1),0);
	g_bmod_tris = bsp2tris(bspname,0);
	btTriangleIndexVertexArray * map_trimesh = new btTriangleIndexVertexArray(g_bmod_tris->indices_c/3,g_bmod_tris->indices,3*sizeof(int),g_bmod_tris->vertices_c,g_bmod_tris->vertices,3*sizeof(float));
	g_bmod_mapShape = new btBvhTriangleMeshShape(map_trimesh,true);
	btDefaultMotionState* motionState = new btDefaultMotionState();
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0,motionState,g_bmod_mapShape,btVector3(0,0,0));
	g_bmod_mapBody = new btRigidBody(rigidBodyCI);
	g_bt_dynamicsWorld->addRigidBody(g_bmod_mapBody);
	//init object list
	g_bmod_objects = new bmodObjectList();

	RETURN_META(MRES_IGNORED);
}

void StartFrame_Post()
{
	//step it
	g_bt_dynamicsWorld->stepSimulation(1.0/BMOD_FPS,BMOD_MAX_STEPS);
	RETURN_META(MRES_IGNORED);
}

void ServerDeactivate_Post()
{
	printf("[BMOD] deactivated\n");
	
	//unload objects
	delete g_bmod_objects;

	//unload the map
	g_bt_dynamicsWorld->removeRigidBody(g_bmod_mapBody);
	delete g_bmod_mapBody->getMotionState();
	delete g_bmod_mapBody;
	delete g_bmod_mapShape;
	free(g_bmod_tris->indices);
	free(g_bmod_tris->vertices);
	free(g_bmod_tris);
	RETURN_META(MRES_IGNORED);
}

void OnAmxxDetach()
{
	printf("[BMOD] detached\n");

	//unload bullet stuff
	delete g_bt_dynamicsWorld;
	delete g_bt_solver;
	delete g_bt_collisionConfiguration;
	delete g_bt_dispatcher;
	delete g_bt_broadphase;

	RETURN_META(MRES_IGNORED);
}