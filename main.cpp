#include "sdk/amxxmodule.h"

#include <btBulletDynamicsCommon.h>
#include <stdio.h>

#include "object.h"

#define BMOD_GRAVITY -525 //-10m/s^2 -> -1000cm/s^2 -> convert to game units (1/1.905f)
#define BMOD_FPS 60
#define BMOD_MAX_STEPS 10

#define BMOD_ENTVAR iuser3
#define BMOD_ENTVAR_TYPE int

#define INDEXOBJECT(id) ((bmodObject*)INDEXENT(id)->v.BMOD_ENTVAR)
#define INDEXOBJECT_R(id) (INDEXENT(id)->v.BMOD_ENTVAR)
#define ENTOBJECT(id) ((bmodObject*)id->v.BMOD_ENTVAR)
#define ENTOBJECT_R(id) (id->v.BMOD_ENTVAR)

static cell AMX_NATIVE_CALL bmod_object_add(AMX *amx, cell *params){
	INDEXOBJECT_R(params[1]) = (BMOD_ENTVAR_TYPE) new bmodObject(INDEXENT(params[1]));
	//TODO: hook on existing entities using INDEXOBJECT
	/*edict_t* e = CREATE_NAMED_ENTITY(ALLOC_STRING("func_wall"));//TODO: do not allocate string every time
	SET_MODEL(e,"models/fyzbox.mdl");
	e->v.nextthink = 86400.0;
	e->v.movetype = 8;
	e->v.BMOD_ENTVAR = (BMOD_ENTVAR_TYPE) new bmodObject(e);*/
	return 1;
}

AMX_NATIVE_INFO amxxfunctions[] = {
	{"bmod_object_add",bmod_object_add},
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

void ServerActivate_Post(edict_t *pEdictList, int edictCount, int clientMax)
{
	printf("[BMOD] activated\n");

	//load the map
	//TODO: trimesh!
	g_bmod_mapShape = new btStaticPlaneShape(btVector3(0,0,1),0);
	btDefaultMotionState* motionState = new btDefaultMotionState();
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0,motionState,g_bmod_mapShape,btVector3(0,0,0));
	g_bmod_mapBody = new btRigidBody(rigidBodyCI);
	g_bt_dynamicsWorld->addRigidBody(g_bmod_mapBody);

	RETURN_META(MRES_IGNORED);
}

void StartFrame_Post()
{
	//step it
	g_bt_dynamicsWorld->stepSimulation(1.0/BMOD_FPS,BMOD_MAX_STEPS);//lets test variable tick
	RETURN_META(MRES_IGNORED);
}

void ServerDeactivate_Post()
{
	printf("[BMOD] deactivated\n");
	
	//TODO: unload objects

	//unload the map
	g_bt_dynamicsWorld->removeRigidBody(g_bmod_mapBody);
	delete g_bmod_mapBody->getMotionState();
	delete g_bmod_mapBody;
	delete g_bmod_mapShape;

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