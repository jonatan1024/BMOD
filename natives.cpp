#include "sdk/amxxmodule.h"
#include "objectlist.h"

extern bmodObjectList * g_bmod_objects;
extern btRigidBody* g_bmod_mapBody;
extern btDiscreteDynamicsWorld* g_bt_dynamicsWorld;

extern int g_bt_max_ssteps;
extern float g_bt_ftstep;

enum {
	BMOD_FL_adamping,	//set/getAngularDamping
	BMOD_FL_afactor,	//setAngularFactor only
	BMOD_FL_damping,	//set/getLinearDamping
	BMOD_FL_friction,	//set/getFriction
	BMOD_FL_restitution	//set/getRestitution
};

enum {
	BMOD_VEC_afactor,	//set/getAngularFactor
	BMOD_VEC_avelocity,	//set/getAngularVelocity
	BMOD_VEC_afriction,	//set/getAnisotropicFriction
	BMOD_VEC_gravity,	//set/getGravity
	BMOD_VEC_factor,	//set/getLinearFactor
	BMOD_VEC_velocity,	//set/getLinearVelocity
	BMOD_VEC_force,		//applyCentralForce / getTotalForce
	BMOD_VEC_torque		//applyTorque / getTotalTorque
};

/*
enum{
	BMOD_VEC2_force_at,
};*/

/*bmod_object*/

static cell AMX_NATIVE_CALL bmod_object_add(AMX *amx, cell *params) {
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

static cell AMX_NATIVE_CALL bmod_object_remove(AMX *amx, cell *params) {
	return g_bmod_objects->remove(INDEXENT(params[1]));
}

static cell AMX_NATIVE_CALL bmod_object_check(AMX *amx, cell *params) {
	if(g_bmod_objects->find(INDEXENT(params[1])))
		return 1;
	return 0;
}

/*bmod_object_set*/

static cell AMX_NATIVE_CALL bmod_object_set_float(AMX *amx, cell *params) {
	bmodObject*object = g_bmod_objects->find(INDEXENT(params[1]));
	if(!object)
		return 0;
	switch(params[2]) {
	case BMOD_FL_adamping:
		object->getRigidBody()->setDamping(object->getRigidBody()->getLinearDamping(), amx_ctof(params[3]));
		break;
	case BMOD_FL_afactor:
		object->getRigidBody()->setAngularFactor(amx_ctof(params[3]));
		break;
	case BMOD_FL_damping:
		object->getRigidBody()->setDamping(amx_ctof(params[3]), object->getRigidBody()->getAngularDamping());
		break;
	case BMOD_FL_friction:
		object->getRigidBody()->setFriction(amx_ctof(params[3]));
		break;
	case BMOD_FL_restitution:
		object->getRigidBody()->setRestitution(amx_ctof(params[3]));
		break;
	}
	object->getRigidBody()->activate(true);
	return 1;
}

static cell AMX_NATIVE_CALL bmod_object_set_vector(AMX *amx, cell *params) {
	bmodObject*object = g_bmod_objects->find(INDEXENT(params[1]));
	if(!object)
		return 0;
	cell *c = g_fn_GetAmxAddr(amx, params[3]);
	switch(params[2]) {
	case BMOD_VEC_afactor:
		object->getRigidBody()->setAngularFactor(btVector3(amx_ctof(c[0]), amx_ctof(c[1]), amx_ctof(c[2])));
		break;
	case BMOD_VEC_avelocity:
		object->getRigidBody()->setAngularVelocity(btVector3(amx_ctof(c[0]), amx_ctof(c[1]), amx_ctof(c[2])));
		break;
	case BMOD_VEC_afriction:
		object->getRigidBody()->setAnisotropicFriction(btVector3(amx_ctof(c[0]), amx_ctof(c[1]), amx_ctof(c[2])));
		break;
	case BMOD_VEC_gravity:
		object->getRigidBody()->setGravity(btVector3(amx_ctof(c[0]), amx_ctof(c[1]), amx_ctof(c[2])));
		break;
	case BMOD_VEC_factor:
		object->getRigidBody()->setLinearFactor(btVector3(amx_ctof(c[0]), amx_ctof(c[1]), amx_ctof(c[2])));
		break;
	case BMOD_VEC_velocity:
		object->getRigidBody()->setLinearVelocity(btVector3(amx_ctof(c[0]), amx_ctof(c[1]), amx_ctof(c[2])));
		break;
	case BMOD_VEC_force:
		object->getRigidBody()->applyCentralForce(btVector3(amx_ctof(c[0]), amx_ctof(c[1]), amx_ctof(c[2])));
		break;
	case BMOD_VEC_torque:
		object->getRigidBody()->applyTorque(btVector3(amx_ctof(c[0]), amx_ctof(c[1]), amx_ctof(c[2])));
		break;
	}
	object->getRigidBody()->activate(true);
	return 1;
}

static cell AMX_NATIVE_CALL bmod_object_apply_force_at(AMX *amx, cell *params) {
	bmodObject*object = g_bmod_objects->find(INDEXENT(params[1]));
	if(!object)
		return 0;
	cell *c = g_fn_GetAmxAddr(amx, params[2]);
	cell *c2 = g_fn_GetAmxAddr(amx, params[3]);
	object->getRigidBody()->applyForce(
		btVector3(amx_ctof(c[0]), amx_ctof(c[1]), amx_ctof(c[2])),
		btVector3(amx_ctof(c2[0]), amx_ctof(c2[1]), amx_ctof(c2[2])));
	object->getRigidBody()->activate(true);
	return 1;
}

/*bmod_object_get*/

static cell AMX_NATIVE_CALL bmod_object_get_float(AMX *amx, cell *params) {
	bmodObject*object = g_bmod_objects->find(INDEXENT(params[1]));
	if(!object)
		return 0;
	switch(params[2]) {
	case BMOD_FL_adamping:
		return amx_ftoc(object->getRigidBody()->getAngularDamping());
	case BMOD_FL_damping:
		return amx_ftoc(object->getRigidBody()->getLinearDamping());
	case BMOD_FL_friction:
		return amx_ftoc(object->getRigidBody()->getFriction());
	case BMOD_FL_restitution:
		return amx_ftoc(object->getRigidBody()->getRestitution());
	}
	return 0;
}

static cell AMX_NATIVE_CALL bmod_object_get_vector(AMX *amx, cell *params) {
	bmodObject*object = g_bmod_objects->find(INDEXENT(params[1]));
	if(!object)
		return 0;
	cell *c = g_fn_GetAmxAddr(amx, params[3]);
	btVector3 vec;
	switch(params[2]) {
	case BMOD_VEC_afactor:
		vec = object->getRigidBody()->getAngularFactor();
		break;
	case BMOD_VEC_avelocity:
		vec = object->getRigidBody()->getAngularVelocity();
		break;
	case BMOD_VEC_afriction:
		vec = object->getRigidBody()->getAnisotropicFriction();
		break;
	case BMOD_VEC_gravity:
		vec = object->getRigidBody()->getGravity();
		break;
	case BMOD_VEC_factor:
		vec = object->getRigidBody()->getLinearFactor();
		break;
	case BMOD_VEC_velocity:
		vec = object->getRigidBody()->getLinearVelocity();
		break;
	case BMOD_VEC_force:
		vec = object->getRigidBody()->getTotalForce();
		break;
	case BMOD_VEC_torque:
		vec = object->getRigidBody()->getTotalTorque();
		break;
	}
	c[0] = amx_ftoc(vec.x());
	c[1] = amx_ftoc(vec.y());
	c[2] = amx_ftoc(vec.z());
	return 1;
}

static cell AMX_NATIVE_CALL bmod_object_set_callback(AMX *amx, cell *params) {
	bmodObject*object = g_bmod_objects->find(INDEXENT(params[1]));
	if(!object)
		return 0;
	int flags = object->getRigidBody()->getCollisionFlags();
	if(params[2])
		object->getRigidBody()->setCollisionFlags(flags | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	else
		object->getRigidBody()->setCollisionFlags(flags & ~btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	return 1;
}

/*bmod_world*/

static cell AMX_NATIVE_CALL bmod_world_set_float(AMX *amx, cell *params) {
	switch(params[1]) {
	case BMOD_FL_adamping:
		return 0;
	case BMOD_FL_afactor:
		return 0;
	case BMOD_FL_damping:
		return 0;
	case BMOD_FL_friction:
		g_bmod_mapBody->setFriction(amx_ctof(params[2]));
		break;
	case BMOD_FL_restitution:
		g_bmod_mapBody->setRestitution(amx_ctof(params[2]));
		break;
	}
	return 1;
}

static cell AMX_NATIVE_CALL bmod_world_get_float(AMX *amx, cell *params) {
	switch(params[1]) {
	case BMOD_FL_friction:
		return amx_ftoc(g_bmod_mapBody->getFriction());
	case BMOD_FL_restitution:
		return amx_ftoc(g_bmod_mapBody->getRestitution());
	}
	return 0;
}

static cell AMX_NATIVE_CALL bmod_stepcfg(AMX *amx, cell *params) {
	g_bt_max_ssteps = params[1];
	g_bt_ftstep = amx_ctof(params[2]);
	return 1;
}

static cell AMX_NATIVE_CALL bmod_traceline(AMX *amx, cell *params) {
	cell * c;
	c = g_fn_GetAmxAddr(amx, params[1]);
	btVector3 start(amx_ctof(c[0]), amx_ctof(c[1]), amx_ctof(c[2]));
	c = g_fn_GetAmxAddr(amx, params[2]);
	btVector3 end(amx_ctof(c[0]), amx_ctof(c[1]), amx_ctof(c[2]));
	btCollisionWorld::ClosestRayResultCallback ray(start, end);
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
	{"bmod_object_add", bmod_object_add},
	{"bmod_object_remove", bmod_object_remove},
	{"bmod_object_check", bmod_object_check},

	{"bmod_object_set_float", bmod_object_set_float},
	{"bmod_object_set_vector", bmod_object_set_vector},
	{"bmod_object_apply_force_at", bmod_object_apply_force_at},
	{"bmod_object_get_float", bmod_object_get_float},
	{"bmod_object_get_vector", bmod_object_get_vector},
	{"bmod_object_set_callback", bmod_object_set_callback},

	{"bmod_world_set_float", bmod_world_set_float},
	{"bmod_world_get_float", bmod_world_get_float},

	{"bmod_traceline", bmod_traceline},

	{"bmod_stepcfg", bmod_stepcfg},
	{NULL, NULL}
};