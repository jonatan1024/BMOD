#include "sdk/amxxmodule.h"
#include <btBulletDynamicsCommon.h>
#include "objectlist.h"

extern bmodObjectList * g_bmod_objects;

int forward_contact = 0;

extern ContactProcessedCallback gContactProcessedCallback;

static bool bmodContactProcessedCallback(btManifoldPoint& point, const btCollisionObject* body1, const btCollisionObject* body2) {
	bmodObject * obj1 = g_bmod_objects->find_b(body1);
	bmodObject * obj2 = g_bmod_objects->find_b(body2);
	cell ent1 = obj1 ? ENTINDEX(obj1->getEntity()) : 0;
	cell ent2 = obj2 ? ENTINDEX(obj2->getEntity()) : 0;
	MF_ExecuteForward(forward_contact, ent1, ent2, (float)point.getDistance());
	return false;
}

void OnPluginsLoaded() {
	gContactProcessedCallback = (ContactProcessedCallback)bmodContactProcessedCallback;
	forward_contact = MF_RegisterForward("bmod_forward_contact", ET_IGNORE, FP_CELL, FP_CELL, FP_FLOAT, FP_DONE);
}