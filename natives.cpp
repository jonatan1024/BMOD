#include "sdk/amxxmodule.h"
#include "objectlist.h"

extern bmodObjectList * g_bmod_objects;
extern btRigidBody* g_bmod_mapBody;
extern btDiscreteDynamicsWorld* g_bt_dynamicsWorld;

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

static cell AMX_NATIVE_CALL bmod_object_check(AMX *amx, cell *params){
	if(g_bmod_objects->find(INDEXENT(params[1])))
		return 1;
	return 0;
}

/*bmod_object_set*/

static cell AMX_NATIVE_CALL bmod_object_set_friction(AMX *amx, cell *params){
	bmodObject*object=g_bmod_objects->find(INDEXENT(params[1]));
	if(!object)
		return 0;
	object->getRigidBody()->setFriction(amx_ctof(params[2]));
	return 1;
}

static cell AMX_NATIVE_CALL bmod_object_set_restitution(AMX *amx, cell *params){
	bmodObject*object=g_bmod_objects->find(INDEXENT(params[1]));
	if(!object)
		return 0;
	object->getRigidBody()->setRestitution(amx_ctof(params[2]));
	return 1;
}

static cell AMX_NATIVE_CALL bmod_object_set_gravity(AMX *amx, cell *params){
	bmodObject*object=g_bmod_objects->find(INDEXENT(params[1]));
	if(!object)
		return 0;
	cell *c = g_fn_GetAmxAddr(amx, params[2]);
	object->getRigidBody()->	setGravity(btVector3(amx_ctof(c[0]),amx_ctof(c[1]),amx_ctof(c[2])));
	object->getRigidBody()->activate(true);
	return 1;
}

static cell AMX_NATIVE_CALL bmod_object_set_velocity(AMX *amx, cell *params){
	bmodObject*object=g_bmod_objects->find(INDEXENT(params[1]));
	if(!object)
		return 0;
	cell *c = g_fn_GetAmxAddr(amx, params[2]);
	object->getRigidBody()->setLinearVelocity(btVector3(amx_ctof(c[0]),amx_ctof(c[1]),amx_ctof(c[2])));
	object->getRigidBody()->activate(true);
	return 1;
}

static cell AMX_NATIVE_CALL bmod_object_set_avelocity(AMX *amx, cell *params){
	bmodObject*object=g_bmod_objects->find(INDEXENT(params[1]));
	if(!object)
		return 0;
	cell *c = g_fn_GetAmxAddr(amx, params[2]);
	object->getRigidBody()->	setAngularVelocity(btVector3(amx_ctof(c[0]),amx_ctof(c[1]),amx_ctof(c[2])));
	object->getRigidBody()->activate(true);
	return 1;
}

static cell AMX_NATIVE_CALL bmod_object_apply_force(AMX *amx, cell *params){
	bmodObject*object=g_bmod_objects->find(INDEXENT(params[1]));
	if(!object)
		return 0;
	cell *c = g_fn_GetAmxAddr(amx, params[2]);
	object->getRigidBody()->applyCentralForce(btVector3(amx_ctof(c[0]),amx_ctof(c[1]),amx_ctof(c[2])));
	object->getRigidBody()->activate(true);
	return 1;
}

static cell AMX_NATIVE_CALL bmod_object_apply_torque(AMX *amx, cell *params){
	bmodObject*object=g_bmod_objects->find(INDEXENT(params[1]));
	if(!object)
		return 0;
	cell *c = g_fn_GetAmxAddr(amx, params[2]);
	object->getRigidBody()->applyTorque(btVector3(amx_ctof(c[0]),amx_ctof(c[1]),amx_ctof(c[2])));
	object->getRigidBody()->activate(true);
	return 1;
}

static cell AMX_NATIVE_CALL bmod_object_apply_force_at(AMX *amx, cell *params){
	bmodObject*object=g_bmod_objects->find(INDEXENT(params[1]));
	if(!object)
		return 0;
	cell *c = g_fn_GetAmxAddr(amx, params[2]);
	cell *c2 = g_fn_GetAmxAddr(amx, params[3]);
	object->getRigidBody()->applyForce(
		btVector3(amx_ctof(c[0]),amx_ctof(c[1]),amx_ctof(c[2])),
		btVector3(amx_ctof(c2[0]),amx_ctof(c2[1]),amx_ctof(c2[2])));
	object->getRigidBody()->activate(true);
	return 1;
}

/*bmod_object_get*/

static cell AMX_NATIVE_CALL bmod_object_get_friction(AMX *amx, cell *params){
	return amx_ftoc(
		g_bmod_objects->find(INDEXENT(params[1]))->getRigidBody()->getFriction()
		);
}

static cell AMX_NATIVE_CALL bmod_object_get_restitution(AMX *amx, cell *params){
	return amx_ftoc(
		g_bmod_objects->find(INDEXENT(params[1]))->getRigidBody()->getRestitution()
		);
}

static cell AMX_NATIVE_CALL bmod_object_get_gravity(AMX *amx, cell *params){
	cell *c = g_fn_GetAmxAddr(amx, params[2]);
	btVector3 vec = g_bmod_objects->find(INDEXENT(params[1]))->getRigidBody()->getGravity();
	c[0] = amx_ftoc(vec.x());
	c[1] = amx_ftoc(vec.y());
	c[2] = amx_ftoc(vec.z());
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

static cell AMX_NATIVE_CALL bmod_world_get_friction(AMX *amx, cell *params){
	return amx_ftoc(g_bmod_mapBody->getFriction());
}

static cell AMX_NATIVE_CALL bmod_world_get_restitution(AMX *amx, cell *params){
	return amx_ftoc(g_bmod_mapBody->getRestitution());
}

static cell AMX_NATIVE_CALL bmod_traceline(AMX *amx, cell *params){
	cell * c;
	c = g_fn_GetAmxAddr(amx, params[1]);
	btVector3 start(amx_ctof(c[0]),amx_ctof(c[1]),amx_ctof(c[2]));
	c = g_fn_GetAmxAddr(amx, params[2]);
	btVector3 end(amx_ctof(c[0]),amx_ctof(c[1]),amx_ctof(c[2]));
	btCollisionWorld::ClosestRayResultCallback ray(start,end);
	g_bt_dynamicsWorld->rayTest(start, end, ray);
	if(!ray.hasHit())
		return 0;
	c = g_fn_GetAmxAddr(amx, params[3]);
	c[0] = amx_ftoc(ray.m_hitPointWorld.x());
	c[1] = amx_ftoc(ray.m_hitPointWorld.y());
	c[2] = amx_ftoc(ray.m_hitPointWorld.z());
	c = g_fn_GetAmxAddr(amx, params[4]);
	c[0] = amx_ftoc(ray.m_hitNormalWorld.x());
	c[1] = amx_ftoc(ray.m_hitNormalWorld.y());
	c[2] = amx_ftoc(ray.m_hitNormalWorld.z());
	bmodObject * object = g_bmod_objects->find_b(ray.m_collisionObject);
	if(!object)
		return 0;
	return ENTINDEX(object->getEntity());
}

AMX_NATIVE_INFO amxxfunctions[] = {
	{"bmod_object_add",bmod_object_add},
	{"bmod_object_remove",bmod_object_remove},
	{"bmod_object_check",bmod_object_check},

	{"bmod_object_set_friction",bmod_object_set_friction},
	{"bmod_object_set_restitution",bmod_object_set_restitution},
	{"bmod_object_set_gravity",bmod_object_set_gravity},
	{"bmod_object_set_velocity",bmod_object_set_velocity},
	{"bmod_object_set_avelocity",bmod_object_set_avelocity},
	{"bmod_object_apply_force",bmod_object_apply_force},
	{"bmod_object_apply_torque",bmod_object_apply_torque},
	{"bmod_object_apply_force_at",bmod_object_apply_force_at},

	{"bmod_object_get_friction",bmod_object_get_friction},
	{"bmod_object_get_restitution",bmod_object_get_restitution},
	{"bmod_object_get_gravity",bmod_object_get_gravity},

	{"bmod_world_set_friction",bmod_world_set_friction},
	{"bmod_world_set_restitution",bmod_world_set_restitution},

	{"bmod_world_get_friction",bmod_world_get_friction},
	{"bmod_world_get_restitution",bmod_world_get_restitution},

	{"bmod_traceline",bmod_traceline},
	{NULL, NULL}
};