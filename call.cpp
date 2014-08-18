#include "call.h"
#include "euler.h"

//fix bad bullet header file
#define SIMD_2_PI			(btScalar(2.0) * SIMD_PI)

typedef struct rb_func {
	bool angular;
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
	{0, "activate", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::activate},

	{0, "applyCentralForce", &btRigidBody::applyCentralForce},
	{0, "applyCentralImpulse", &btRigidBody::applyCentralImpulse},
	{0, "applyDamping", 0, &btRigidBody::applyDamping},
	{0, "applyForce", 0, 0, &btRigidBody::applyForce},
	{0, "applyGravity", 0, 0, 0, &btRigidBody::applyGravity},
	{0, "applyImpulse", 0, 0, &btRigidBody::applyImpulse},
	{1, "applyTorque", &btRigidBody::applyTorque},
	{1, "applyTorqueImpulse", &btRigidBody::applyTorqueImpulse},

	{0, "forceActivationState", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::forceActivationState},

	{0, "getActivationState", 0, 0, 0, 0, 0, 0, &btRigidBody::getActivationState},
	{0, "getAngularDamping", 0, 0, 0, 0, &btRigidBody::getAngularDamping},
	{1, "getAngularFactor", 0, 0, 0, 0, 0, &btRigidBody::getAngularFactor},
	{0, "getAngularSleepingThreshold", 0, 0, 0, 0, &btRigidBody::getAngularSleepingThreshold},
	{1, "getAngularVelocity", 0, 0, 0, 0, 0, &btRigidBody::getAngularVelocity},
	{0, "getCollisionFlags", 0, 0, 0, 0, 0, 0, &btRigidBody::getCollisionFlags},
	{0, "getFlags", 0, 0, 0, 0, 0, 0, &btRigidBody::getFlags},
	{0, "getFriction", 0, 0, 0, 0, &btRigidBody::getFriction},
	{0, "getGravity", 0, 0, 0, 0, 0, &btRigidBody::getGravity},
	{0, "getInvMass", 0, 0, 0, 0, &btRigidBody::getInvMass},
	{0, "getLinearDamping", 0, 0, 0, 0, &btRigidBody::getLinearDamping},
	{0, "getLinearFactor", 0, 0, 0, 0, 0, &btRigidBody::getLinearFactor},
	{0, "getLinearSleepingThreshold", 0, 0, 0, 0, &btRigidBody::getLinearSleepingThreshold},
	{0, "getLinearVelocity", 0, 0, 0, 0, 0, &btRigidBody::getLinearVelocity},
	{0, "getRestitution", 0, 0, 0, 0, &btRigidBody::getRestitution},
	{0, "getRollingFriction", 0, 0, 0, 0, &btRigidBody::getRollingFriction},
	{0, "getUserIndex", 0, 0, 0, 0, 0, 0, &btRigidBody::getUserIndex},
	{0, "getWorldTransform", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::getWorldTransform},

	{0, "isActive", 0, 0, 0, 0, 0, 0, 0, &btRigidBody::isActive},
	{0, "isKinematicObject", 0, 0, 0, 0, 0, 0, 0, &btRigidBody::isKinematicObject},
	{0, "isStaticObject", 0, 0, 0, 0, 0, 0, 0, &btRigidBody::isStaticObject},

	{0, "setActivationState", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::setActivationState},
	{0, "setAngularFactor", 0, &btRigidBody::setAngularFactor},
	{1, "setAngularVelocity", &btRigidBody::setAngularVelocity},
	{0, "setCollisionFlags", 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::setCollisionFlags},
	{0, "setDamping", 0, 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::setDamping},
	{0, "setFlags", 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::setFlags},
	{0, "setFriction", 0, &btRigidBody::setFriction},
	{0, "setGravity", &btRigidBody::setGravity},
	{0, "setLinearFactor", &btRigidBody::setLinearFactor},
	{0, "setLinearVelocity", &btRigidBody::setLinearVelocity},
	{0, "setRestitution", 0, &btRigidBody::setRestitution},
	{0, "setRollingFriction", 0, &btRigidBody::setRollingFriction},
	{0, "setSleepingThresholds", 0, 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::setSleepingThresholds},
	{0, "setUserIndex", 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::setUserIndex},
	{0, "setWorldTransform", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &btRigidBody::setWorldTransform},
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
		if(func->angular) {
			btvec1 = btVector3(btvec1[2], -btvec1[0], btvec1[1]) * SIMD_RADS_PER_DEG;
		}
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
		if(func->angular) {
			btvec1 = btVector3(-btvec1[1], btvec1[2], btvec1[0]) * SIMD_DEGS_PER_RAD;
		}
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