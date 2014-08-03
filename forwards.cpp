#include "sdk/amxxmodule.h"
#include <btBulletDynamicsCommon.h>

#include <map>
#include "object.h"

extern std::map<int, bmodObject*> g_bmod_objects;

int forward_contact = 0;

extern ContactProcessedCallback gContactProcessedCallback;

static bool bmodContactProcessedCallback(btManifoldPoint& point, const btCollisionObject* body1, const btCollisionObject* body2) {
	cell obj1 = (int)body1->getUserPointer();
	cell obj2 = (int)body2->getUserPointer();
	/*cell obj1 = -1;
	cell obj2 = -1;
	for(std::map<int, bmodObject*>::iterator it = g_bmod_objects.begin(); it != g_bmod_objects.end(); ++it) {
		if(it->second->getRigidBody() == body1)
			obj1 = it->first;
		if(it->second->getRigidBody() == body2)
			obj2 = it->first;
		if(obj1 > -1 && obj2 > -1)
			break;
	}*/
	MF_ExecuteForward(forward_contact, obj1, obj2, (float)point.getDistance());
	return false;
}

void OnPluginsLoaded() {
	gContactProcessedCallback = (ContactProcessedCallback)bmodContactProcessedCallback;
	forward_contact = MF_RegisterForward("bmod_forward_contact", ET_IGNORE, FP_CELL, FP_CELL, FP_FLOAT, FP_DONE);
}