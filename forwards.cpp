#include "sdk/amxxmodule.h"
#include <btBulletDynamicsCommon.h>

#include <vector>
#include "object.h"

extern std::vector<bmodObject*> g_bmod_objects;

int forward_contact = 0;

extern ContactProcessedCallback gContactProcessedCallback;

static bool bmodContactProcessedCallback(btManifoldPoint& point, const btCollisionObject* body1, const btCollisionObject* body2) {
	//TODO BETTER!!!
	cell obj1 = -1;
	cell obj2 = -1;
	for(std::vector<bmodObject*>::iterator it = g_bmod_objects.begin(); it != g_bmod_objects.end(); ++it) {
		if((*it)->getRigidBody() == body1)
			obj1 = it - g_bmod_objects.begin();
		if((*it)->getRigidBody() == body2)
			obj2 = it - g_bmod_objects.begin();
		if(obj1 > -1 && obj2 > -1)
			break;
	}
	MF_ExecuteForward(forward_contact, obj1, obj2, (float)point.getDistance());
	return false;
}

void OnPluginsLoaded() {
	gContactProcessedCallback = (ContactProcessedCallback)bmodContactProcessedCallback;
	forward_contact = MF_RegisterForward("bmod_forward_contact", ET_IGNORE, FP_CELL, FP_CELL, FP_FLOAT, FP_DONE);
}