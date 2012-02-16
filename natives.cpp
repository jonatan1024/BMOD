#include "sdk/amxxmodule.h"
#include "objectlist.h"

extern bmodObjectList * g_bmod_objects;
extern btRigidBody* g_bmod_mapBody;

/*bmod_object*/

static cell AMX_NATIVE_CALL bmod_object_add(AMX *amx, cell *params){
	cell *c = g_fn_GetAmxAddr(amx, params[4]);
	return g_bmod_objects->add(new bmodObject(
		INDEXENT(params[1]),	//e
		params[2],	//type
		amx_ctof(params[3]),	//mass
		amx_ctof(c[0]),	//x
		amx_ctof(c[1]),	//y
		amx_ctof(c[2])	//z
		));
}

static cell AMX_NATIVE_CALL bmod_object_remove(AMX *amx, cell *params){
	return g_bmod_objects->remove(INDEXENT(params[1]));
}

static cell AMX_NATIVE_CALL bmod_object_set_friction(AMX *amx, cell *params){
	g_bmod_objects->find(INDEXENT(params[1]))->getRigidBody()->
		setFriction(amx_ctof(params[2]));
	return 1;
}

static cell AMX_NATIVE_CALL bmod_object_set_restitution(AMX *amx, cell *params){
	g_bmod_objects->find(INDEXENT(params[1]))->getRigidBody()->
		setRestitution(amx_ctof(params[2]));
	return 1;
}

static cell AMX_NATIVE_CALL bmod_object_set_gravity(AMX *amx, cell *params){
	cell *c = g_fn_GetAmxAddr(amx, params[2]);
	g_bmod_objects->find(INDEXENT(params[1]))->getRigidBody()->
		setGravity(btVector3(amx_ctof(c[0]),amx_ctof(c[1]),amx_ctof(c[2])));
	return 1;
}

static cell AMX_NATIVE_CALL bmod_object_set_velocity(AMX *amx, cell *params){
	cell *c = g_fn_GetAmxAddr(amx, params[2]);
	g_bmod_objects->find(INDEXENT(params[1]))->getRigidBody()->
		setLinearVelocity(btVector3(amx_ctof(c[0]),amx_ctof(c[1]),amx_ctof(c[2])));
	return 1;
}

static cell AMX_NATIVE_CALL bmod_object_set_avelocity(AMX *amx, cell *params){
	cell *c = g_fn_GetAmxAddr(amx, params[2]);
	g_bmod_objects->find(INDEXENT(params[1]))->getRigidBody()->
		setAngularVelocity(btVector3(amx_ctof(c[0]),amx_ctof(c[1]),amx_ctof(c[2])));
	return 1;
}

static cell AMX_NATIVE_CALL bmod_object_apply_force(AMX *amx, cell *params){
	cell *c = g_fn_GetAmxAddr(amx, params[2]);
	g_bmod_objects->find(INDEXENT(params[1]))->getRigidBody()->
		applyCentralForce(btVector3(amx_ctof(c[0]),amx_ctof(c[1]),amx_ctof(c[2])));
	return 1;
}

static cell AMX_NATIVE_CALL bmod_object_apply_torque(AMX *amx, cell *params){
	cell *c = g_fn_GetAmxAddr(amx, params[2]);
	g_bmod_objects->find(INDEXENT(params[1]))->getRigidBody()->
		applyTorque(btVector3(amx_ctof(c[0]),amx_ctof(c[1]),amx_ctof(c[2])));
	return 1;
}

/*bmod_world*/

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
	{"bmod_object_set_gravity",bmod_object_set_gravity},
	{"bmod_object_set_velocity",bmod_object_set_velocity},
	{"bmod_object_set_avelocity",bmod_object_set_avelocity},
	{"bmod_object_apply_force",bmod_object_apply_force},
	{"bmod_object_apply_torque",bmod_object_apply_torque},
	{"bmod_world_set_friction",bmod_world_set_friction},
	{"bmod_world_set_restitution",bmod_world_set_restitution},
	{NULL, NULL}
};