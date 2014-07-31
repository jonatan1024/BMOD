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
int bmod_obj_get_ents(int obj, entities[], int len);
*/
static cell AMX_NATIVE_CALL bmod_obj_get_ents(AMX *amx, cell *params) {
	objs_it it = g_bmod_objects.find(params[1]);
	if(it == g_bmod_objects.end())
		return -1;
	std::list<int> * entlist = it->second->getEntities();

	cell * entarray = MF_GetAmxAddr(amx, params[2]);

	std::list<int>::iterator lit;
	int i = 0;
	for(lit = entlist->begin(); lit != entlist->end() && i < params[3]; ++lit) {
		entarray[i] = (*lit);
		i++;
	}

	return i;
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

	bool kinematic = (bool)params[2];

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
... bmod_traceline(float start[3], float end[3], float hitpoint[3], float hitnormal[3]);
*/
static cell AMX_NATIVE_CALL bmod_traceline(AMX *amx, cell *params) {
	cell * c;
	c = g_fn_GetAmxAddr(amx, params[1]);
	btVector3 start(amx_ctof(c[0]), amx_ctof(c[1]), amx_ctof(c[2]));
	c = g_fn_GetAmxAddr(amx, params[2]);
	btVector3 end(amx_ctof(c[0]), amx_ctof(c[1]), amx_ctof(c[2]));
	btCollisionWorld::ClosestRayResultCallback ray(start, end);
	g_bt_dynamicsWorld->rayTest(start, end, ray);
	if(!ray.hasHit())
		return -1;
	c = g_fn_GetAmxAddr(amx, params[3]);
	c[0] = amx_ftoc(ray.m_hitPointWorld[0]);
	c[1] = amx_ftoc(ray.m_hitPointWorld[1]);
	c[2] = amx_ftoc(ray.m_hitPointWorld[2]);
	c = g_fn_GetAmxAddr(amx, params[4]);
	c[0] = amx_ftoc(ray.m_hitNormalWorld[0]);
	c[1] = amx_ftoc(ray.m_hitNormalWorld[1]);
	c[2] = amx_ftoc(ray.m_hitNormalWorld[2]);

	//TODO better!
	for(std::map<int, bmodObject*>::iterator it = g_bmod_objects.begin(); it != g_bmod_objects.end(); ++it) {
		if(it->second->getRigidBody() == ray.m_collisionObject)
			return it->first;
	}
	return -1;
}

/*
void bmod_stepcfg(int max_ssteps, float ftstep);
*/
static cell AMX_NATIVE_CALL bmod_stepcfg(AMX *amx, cell *params) {
	g_bt_max_ssteps = params[1];
	g_bt_ftstep = amx_ctof(params[2]);
	return 0;
}

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