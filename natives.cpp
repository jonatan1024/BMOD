#include "sdk/amxxmodule.h"
#include "objectlist.h"

extern bmodObjectList * g_bmod_objects;
extern btRigidBody* g_bmod_mapBody;

static cell AMX_NATIVE_CALL bmod_object_add(AMX *amx, cell *params){
	return g_bmod_objects->add(new bmodObject(
		INDEXENT(params[1]),	//e
		params[2],				//type
		amx_ctof(params[3]),	//mass
		amx_ctof(params[4]),	//x
		amx_ctof(params[5]),	//y
		amx_ctof(params[6])		//z
		));
}

static cell AMX_NATIVE_CALL bmod_object_remove(AMX *amx, cell *params){
	return g_bmod_objects->remove(INDEXENT(params[1]));
}

static cell AMX_NATIVE_CALL bmod_object_set_friction(AMX *amx, cell *params){
	if(bmodObject * object = g_bmod_objects->find(INDEXENT(params[1]))){
		object->getRigidBody()->setFriction(amx_ctof(params[2]));
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL bmod_object_set_restitution(AMX *amx, cell *params){
	if(bmodObject * object = g_bmod_objects->find(INDEXENT(params[1]))){
		object->getRigidBody()->setRestitution(amx_ctof(params[2]));
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL bmod_world_set_friction(AMX *amx, cell *params){
	g_bmod_mapBody->setFriction(amx_ctof(params[1]));
	return 1;
}

static cell AMX_NATIVE_CALL bmod_world_set_restitution(AMX *amx, cell *params){
	g_bmod_mapBody->setRestitution(amx_ctof(params[1]));
	return 1;
}

AMX_NATIVE_INFO amxxfunctions[] = {
	{"bmod_object_add",bmod_object_add},
	{"bmod_object_remove",bmod_object_remove},
	{"bmod_object_set_friction",bmod_object_set_friction},
	{"bmod_object_set_restitution",bmod_object_set_restitution},
	{"bmod_world_set_friction",bmod_world_set_friction},
	{"bmod_world_set_restitution",bmod_world_set_restitution},
	{NULL, NULL}
};