#include "call.h"
#include "euler.h"

typedef struct rb_func {
	char name[64];
	void (btRigidBody::*void_const_vector)(const btVector3&);
	void (btRigidBody::*void_scalar)(btScalar);
	void (btRigidBody::*void_const_vector_const_vector)(const btVector3&, const btVector3&);
	void (btRigidBody::*void_)();
	btScalar(btRigidBody::*scalar_const)() const;
	const btVector3& (btRigidBody::*const_vector_ref_const)() const;
	int (btRigidBody::*int_const)() const;
	bool (btRigidBody::*bool_const)() const;
	void (btRigidBody::*void_int)(int);
	void (btRigidBody::*void_scalar_scalar)(btScalar, btScalar);
	void (btRigidBody::*void_scalar_const_vector)(btScalar, const btVector3&);
	void (btRigidBody::*void_bool_const)(bool) const;
	const btTransform& (btRigidBody::*const_transform_ref_const)() const;
	void (btRigidBody::*void_const_transform)(const btTransform&);
	void (btRigidBody::*void_int_const)(int) const;
} rb_func;

rb_func rb_fcs[] = {
	{"activate", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::activate},

	{"applyCentralForce", &btRigidBody::applyCentralForce},
	{"applyCentralImpulse", &btRigidBody::applyCentralImpulse},
	{"applyDamping", 0, &btRigidBody::applyDamping},
	{"applyForce", 0, 0, &btRigidBody::applyForce},
	{"applyGravity", 0, 0, 0, &btRigidBody::applyGravity},
	{"applyImpulse", 0, 0, &btRigidBody::applyImpulse},
	{"applyTorque", &btRigidBody::applyTorque},
	{"applyTorqueImpulse", &btRigidBody::applyTorqueImpulse},

	{"forceActivationState", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::forceActivationState},

	{"getActivationState", 0, 0, 0, 0, 0, 0, &btRigidBody::getActivationState},
	{"getAngularDamping", 0, 0, 0, 0, &btRigidBody::getAngularDamping},
	{"getAngularFactor", 0, 0, 0, 0, 0, &btRigidBody::getAngularFactor},
	{"getAngularSleepingThreshold", 0, 0, 0, 0, &btRigidBody::getAngularSleepingThreshold},
	{"getAngularVelocity", 0, 0, 0, 0, 0, &btRigidBody::getAngularVelocity},
	{"getCollisionFlags", 0, 0, 0, 0, 0, 0, &btRigidBody::getCollisionFlags},
	{"getFlags", 0, 0, 0, 0, 0, 0, &btRigidBody::getFlags},
	{"getFriction", 0, 0, 0, 0, &btRigidBody::getFriction},
	{"getGravity", 0, 0, 0, 0, 0, &btRigidBody::getGravity},
	{"getInvMass", 0, 0, 0, 0, &btRigidBody::getInvMass},
	{"getLinearDamping", 0, 0, 0, 0, &btRigidBody::getLinearDamping},
	{"getLinearFactor", 0, 0, 0, 0, 0, &btRigidBody::getLinearFactor},
	{"getLinearSleepingThreshold", 0, 0, 0, 0, &btRigidBody::getLinearSleepingThreshold},
	{"getLinearVelocity", 0, 0, 0, 0, 0, &btRigidBody::getLinearVelocity},
	{"getRestitution", 0, 0, 0, 0, &btRigidBody::getRestitution},
	{"getRollingFriction", 0, 0, 0, 0, &btRigidBody::getRollingFriction},
	{"getUserIndex", 0, 0, 0, 0, 0, 0, &btRigidBody::getUserIndex},
	{"getWorldTransform", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::getWorldTransform},

	{"isActive", 0, 0, 0, 0, 0, 0, 0, &btRigidBody::isActive},
	{"isKinematicObject", 0, 0, 0, 0, 0, 0, 0, &btRigidBody::isKinematicObject},
	{"isStaticObject", 0, 0, 0, 0, 0, 0, 0, &btRigidBody::isStaticObject},

	{"setActivationState", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::setActivationState},
	{"setAngularFactor", 0, &btRigidBody::setAngularFactor},
	{"setAngularVelocity", &btRigidBody::setAngularVelocity},
	{"setCollisionFlags", 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::setCollisionFlags},
	{"setDamping", 0, 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::setDamping},
	{"setFlags", 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::setFlags},
	{"setFriction", 0, &btRigidBody::setFriction},
	{"setGravity", &btRigidBody::setGravity},
	{"setLinearFactor", &btRigidBody::setLinearFactor},
	{"setLinearVelocity", &btRigidBody::setLinearVelocity},
	{"setRestitution", 0, &btRigidBody::setRestitution},
	{"setRollingFriction", 0, &btRigidBody::setRollingFriction},
	{"setSleepingThresholds", 0, 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::setSleepingThresholds},
	{"setUserIndex", 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::setUserIndex},
	{"setWorldTransform", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::setWorldTransform},
};

rb_func * get_rb_func(char * name) {
	//binsearch
	int left = 0;
	int right = (sizeof(rb_fcs) / sizeof(rb_func)) - 1;
	while(left <= right) {
		int mid = left + (right - left) / 2;
		int sign = strcmp(name, rb_fcs[mid].name);
		if(sign > 0) {
			left = mid + 1;
		}
		else if(sign < 0) {
			right = mid - 1;
		}
		else {
			return &rb_fcs[mid];
		}
	}
	return NULL;
}

bool rbCall(btRigidBody * rb, char * name, AMX * amx, cell * params) {
	rb_func * func = get_rb_func(name);
	if(!func) {
		MF_Log("Function \"%s\" not found!", name);
		return false;
	}
	
	cell * param0 = MF_GetAmxAddr(amx, params[0]);
	cell * param1 = MF_GetAmxAddr(amx, params[1]);

	if(func->void_const_vector) {
		
		btVector3 btvec1;
		btvec1[0] = amx_ctof(param0[0]);
		btvec1[1] = amx_ctof(param0[1]);
		btvec1[2] = amx_ctof(param0[2]);
		(rb->*(func->void_const_vector))(btvec1);
	}
	else if(func->void_scalar) {
		btScalar btscl1;
		btscl1 = amx_ctof(param0[0]);
		(rb->*(func->void_scalar))(btscl1);
	}
	else if(func->void_const_vector_const_vector) {
		btVector3 btvec1;
		btvec1[0] = amx_ctof(param0[0]);
		btvec1[1] = amx_ctof(param0[1]);
		btvec1[2] = amx_ctof(param0[2]);
		btVector3 btvec2;
		btvec2[0] = amx_ctof(param1[0]);
		btvec2[1] = amx_ctof(param1[1]);
		btvec2[2] = amx_ctof(param1[2]);
		(rb->*(func->void_const_vector_const_vector))(btvec1, btvec2);
	}
	else if(func->void_) {
		(rb->*(func->void_))();
	}
	else if(func->scalar_const) {
		btScalar btscl1;
		btscl1 = (rb->*(func->scalar_const))();
		param0[0] = amx_ftoc(btscl1);
	}
	else if(func->const_vector_ref_const) {
		btVector3 btvec1;
		btvec1 = (rb->*(func->const_vector_ref_const))();
		param0[0] = amx_ftoc(btvec1[0]);
		param0[1] = amx_ftoc(btvec1[1]);
		param0[2] = amx_ftoc(btvec1[2]);
	}
	else if(func->int_const) {
		param0[0] = (rb->*(func->int_const))();
	}
	else if(func->bool_const) {
		param0[0] = (rb->*(func->bool_const))();
	}
	else if(func->void_int) {
		(rb->*(func->void_int))(param0[0]);
	}
	else if(func->void_scalar_scalar) {
		btScalar btscl1;
		btScalar btscl2;
		btscl1 = amx_ctof(param0[0]);
		btscl2 = amx_ctof(param1[0]);
		(rb->*(func->void_scalar_scalar))(btscl1, btscl2);
	}
	else if(func->void_scalar_const_vector) {
		btScalar btscl1;
		btscl1 = amx_ctof(param0[0]);
		btVector3 btvec1;
		btvec1[0] = amx_ctof(param1[0]);
		btvec1[1] = amx_ctof(param1[1]);
		btvec1[2] = amx_ctof(param1[2]);
		(rb->*(func->void_scalar_const_vector))(btscl1, btvec1);
	}
	else if(func->void_bool_const) {
		bool b = (bool)(param0[0]);
		(rb->*(func->void_bool_const))(b);
	}
	else if(func->const_transform_ref_const) {
		btTransform tr = (rb->*(func->const_transform_ref_const))();
		btVector3 btvec1 = tr.getOrigin();
		param0[0] = amx_ftoc(btvec1[0]);
		param0[1] = amx_ftoc(btvec1[1]);
		param0[2] = amx_ftoc(btvec1[2]);
		btVector3 btvec2;
		MatrixEuler(tr.getBasis(), btvec2);

		param1[0] = amx_ftoc(btvec2[0]);
		param1[1] = amx_ftoc(btvec2[1]);
		param1[2] = amx_ftoc(btvec2[2]);
	}
	else if(func->void_const_transform) {
		btVector3 btvec1;
		btvec1[0] = amx_ctof(param0[0]);
		btvec1[1] = amx_ctof(param0[1]);
		btvec1[2] = amx_ctof(param0[2]);
		btVector3 btvec2;
		btvec2[0] = amx_ctof(param1[0]);
		btvec2[1] = amx_ctof(param1[1]);
		btvec2[2] = amx_ctof(param1[2]);
		btTransform tr(btQuaternion(0, 0, 0, 1), btvec1);
		EulerMatrix(btvec2, tr.getBasis());
		(rb->*(func->void_const_transform))(tr);
	}
	else if(func->void_int_const) {
		(rb->*(func->void_int_const))(param0[0]);
	}
	else {
		MF_Log("Function (%s) type not found", name);
		return false;
	}
	return true;
}