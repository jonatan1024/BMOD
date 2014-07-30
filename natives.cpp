#include "sdk/amxxmodule.h"

#include "object.h"
#include <map>
#include "call.h"
#include "model.h"

typedef std::map<int, bmodObject*>::iterator objs_it;
extern std::map<int, bmodObject*> g_bmod_objects;
extern btDiscreteDynamicsWorld* g_bt_dynamicsWorld;

extern int g_bt_max_ssteps;
extern float g_bt_ftstep;

int _insert_object(bmodObject* object) {
	int index = 0;
	if(g_bmod_objects.size()) {
		index = g_bmod_objects.rbegin()->first + 1;
	}
	g_bmod_objects[index] = object;
	return index;
}

/*
int bmod_obj_new(char * model);
*/
static cell AMX_NATIVE_CALL bmod_obj_new(AMX *amx, cell *params) {
	int len;
	char * model = MF_GetAmxString(amx, params[1], 0, &len);

	float mass = amx_ctof(params[2]);

	return _insert_object(new bmodObject(model, mass));
}

/*
bool bmod_obj_delete(int obj);
*/
static cell AMX_NATIVE_CALL bmod_obj_delete(AMX *amx, cell *params) {
	objs_it it = g_bmod_objects.find(params[1]);
	if(it == g_bmod_objects.end())
		return false;
	delete it->second;
	g_bmod_objects.erase(it);
	return true;
}

/*
int bmod_obj_from_ent(int ent);
*/
static cell AMX_NATIVE_CALL bmod_obj_from_ent(AMX *amx, cell *params) {
	edict_t * ent = INDEXENT(params[1]);
	if(!ent)
		return -1;

	const char * model = STRING(ent->v.model);
	if(!model || !model[0])
		return -1;

	bmodObject * obj = new bmodObject(model, 0.0f);
	int index = _insert_object(obj);
	obj->assignEntity(params[1]);
	obj->update();

	return index;
}

/*
int bmod_obj_assign_ent(int obj, int ent);
*/
static cell AMX_NATIVE_CALL bmod_obj_assign_ent(AMX *amx, cell *params) {
	objs_it it = g_bmod_objects.find(params[1]);
	if(it == g_bmod_objects.end())
		return -1;

	int entindex = params[2];
	if(!INDEXENT(entindex))
		return -1;
	return it->second->assignEntity(entindex);
}

/*
int bmod_obj_remove_ent(int obj, int ent);
*/
static cell AMX_NATIVE_CALL bmod_obj_remove_ent(AMX *amx, cell *params) {
	objs_it it = g_bmod_objects.find(params[1]);
	if(it == g_bmod_objects.end())
		return -1;

	int entindex = params[2];
	if(!INDEXENT(entindex))
		return -1;
	return it->second->removeEntity(entindex);
}

/*
int bmod_obj_get_ents(int obj);
*/
static cell AMX_NATIVE_CALL bmod_obj_get_ents(AMX *amx, cell *params) {
	objs_it it = g_bmod_objects.find(params[1]);
	if(it == g_bmod_objects.end())
		return false;

	//TDOODOODODO TODO

	return true;
}

/*
int bmod_obj_by_ent(int ent);
*/
static cell AMX_NATIVE_CALL bmod_obj_by_ent(AMX *amx, cell *params) {
	int entindex = params[1];
	if(!INDEXENT(entindex))
		return -1;

	for(objs_it it = g_bmod_objects.begin(); it != g_bmod_objects.end(); ++it) {
		std::list<int> * ents = it->second->getEntities();
		for(std::list<int>::iterator it2 = ents->begin(); it2 != ents->end(); ++it) {
			if(entindex == *it2) {
				return it->first;
			}
		}
	}

	return -1;
}

/*
bool bmod_obj_call(int obj, char * func, ... );
*/
static cell AMX_NATIVE_CALL bmod_obj_call(AMX *amx, cell *params) {
	objs_it it = g_bmod_objects.find(params[1]);
	if(it == g_bmod_objects.end())
		return false;

	int len;
	char * func = MF_GetAmxString(amx, params[2], 0, &len);
	if(!func || !func[0])
		return false;

	return rbCall(it->second->getRigidBody(), func, amx, params + 3);
}

/*
bool bmod_obj_set_mass(int obj, float mass);
*/
static cell AMX_NATIVE_CALL bmod_obj_set_mass(AMX *amx, cell *params) {
	objs_it it = g_bmod_objects.find(params[1]);
	if(it == g_bmod_objects.end())
		return false;

	it->second->setMass(amx_ctof(params[2]));
	return true;
}

/*
bool bmod_obj_set_kinematic(int obj, bool kinematic);
*/
static cell AMX_NATIVE_CALL bmod_obj_set_kinematic(AMX *amx, cell *params) {
	objs_it it = g_bmod_objects.find(params[1]);
	if(it == g_bmod_objects.end())
		return false;

	bool kinematic = params[2];

	btRigidBody * rigidBody = it->second->getRigidBody();

	if(kinematic) {
		it->second->setMass(0);
		rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		rigidBody->setActivationState(DISABLE_DEACTIVATION);
	}
	else {
		rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
		rigidBody->setActivationState(WANTS_DEACTIVATION);
	}

	return true;
}

/*
bool bmod_obj_update_pos(int obj);
*/
static cell AMX_NATIVE_CALL bmod_obj_update_pos(AMX *amx, cell *params) {
	objs_it it = g_bmod_objects.find(params[1]);
	if(it == g_bmod_objects.end())
		return false;

	it->second->update();
	return true;
}

/*
bool bmod_shape_cfg(trimesh_shape_type tst, float[3] origin, float[3] scale)
*/
static cell AMX_NATIVE_CALL bmod_shape_cfg(AMX *amx, cell *params) {
	if(params[1] >= TST_NUM_TYPES || params[1] < 0)
		params[1] = 0;
	trimesh_shape_type tst = (trimesh_shape_type)params[1];
	cell * c_origin = MF_GetAmxAddr(amx, params[2]);
	float origin[3];
	cell * c_scale = MF_GetAmxAddr(amx, params[3]);
	float scale[3];
	for(int i = 0; i < 3; i++) {
		origin[i] = amx_ctof(c_origin[i]);
		scale[i] = amx_ctof(c_scale[i]);
	}

	return setModelConfig(tst, origin, scale);
}

/*
... bmod_traceline( ... );
*/
static cell AMX_NATIVE_CALL bmod_traceline(AMX *amx, cell *params) {

	//TODO

	return true;
}

/*
void bmod_stepcfg(int max_ssteps, float ftstep);
*/
static cell AMX_NATIVE_CALL bmod_stepcfg(AMX *amx, cell *params) {
	g_bt_max_ssteps = params[1];
	g_bt_ftstep = amx_ctof(params[2]);
	return 0;
}

#if 0
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

#endif

AMX_NATIVE_INFO amxxfunctions[] = {
	{"bmod_obj_new", bmod_obj_new},
	{"bmod_obj_delete", bmod_obj_delete},
	{"bmod_obj_from_ent", bmod_obj_from_ent},

	{"bmod_obj_assign_ent", bmod_obj_assign_ent},
	{"bmod_obj_remove_ent", bmod_obj_remove_ent},
	{"bmod_obj_get_ents", bmod_obj_get_ents},
	{"bmod_obj_by_ent", bmod_obj_by_ent},

	{"bmod_obj_call", bmod_obj_call},
	{"bmod_obj_set_mass", bmod_obj_set_mass},
	{"bmod_obj_set_kinematic", bmod_obj_set_kinematic},
	{"bmod_obj_update_pos", bmod_obj_update_pos},

	{"bmod_shape_cfg", bmod_shape_cfg},

	{"bmod_traceline", bmod_traceline},

	{"bmod_stepcfg", bmod_stepcfg},
	{NULL, NULL}
};