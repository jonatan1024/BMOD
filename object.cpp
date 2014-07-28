#include "object.h"
#include "model.h"

#include "sdk\amxxmodule.h"

bmodObject::bmodObject(const char * model) {
	MF_Log("full init %s", model);
	//set object shape
	/*switch(type) {
	case 0:
	shape = new btSphereShape(x);
	break;
	case 1:
	shape = new btBoxShape(btVector3(x, y, z));
	break;
	case 2:
	shape = new btCylinderShapeZ(btVector3(x, y, z));
	break;
	case 3:
	shape = new btCapsuleShapeZ(x, y);
	break;
	case 4:
	shape = new btConeShapeZ(x, y);
	break;
	}*/
	if(!getModelShape(model, &shape)) {
		MF_Log("BAD model! (%s)", model);
		//leaking, but fuck it, this shouldn't be happening at the first place
		shape = new btBoxShape(btVector3(32, 32, 32));
	}

	bmodMotionState* objectMotionState = new bmodMotionState(this);
	btRigidBody::btRigidBodyConstructionInfo objectRigidBodyCI(0, objectMotionState, shape, btVector3(0, 0, 0));
	rigidBody = new btRigidBody(objectRigidBodyCI);
	g_bt_dynamicsWorld->addRigidBody(rigidBody);

	rigidBody->setUserIndex(1337);
	int (btRigidBody::*asdf)() const = &btRigidBody::getUserIndex;
	MF_Log("ui: %d %d", rigidBody->getUserIndex(), (rigidBody->*asdf)());
}



std::list<int> * bmodObject::getEntities() {
	return &entities;
}

int bmodObject::assignEntity(int entity) {
	entities.push_back(entity);
	return entities.size();
}

int bmodObject::removeEntity(int entity) {
	entities.remove(entity);
	return entities.size();
}

void bmodObject::setMass(float mass) {
	g_bt_dynamicsWorld->removeRigidBody(rigidBody);

	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);
	rigidBody->setMassProps(mass, inertia);
	rigidBody->updateInertiaTensor();
	rigidBody->activate();

	g_bt_dynamicsWorld->addRigidBody(rigidBody);
}

bool bmodObject::isImmovable() {
	return rigidBody->getInvMass() == 0;
}

btRigidBody * bmodObject::getRigidBody() {
	return rigidBody;
}

void bmodObject::update() {
	if(entities.size() == 0)
		return;

	edict_t * entity = INDEXENT(entities.front());

	btTransform worldTrans(btQuaternion(0, 0, 0, 1), btVector3(entity->v.origin.x, entity->v.origin.y, entity->v.origin.z));
	worldTrans.getBasis().setEulerZYX(entity->v.angles.z / RAD2DEG, -entity->v.angles.x / RAD2DEG, entity->v.angles.y / RAD2DEG);

	rigidBody->setWorldTransform(worldTrans);

	rigidBody->setLinearVelocity(btVector3(entity->v.velocity.x, entity->v.velocity.y, entity->v.velocity.z));
	rigidBody->setAngularVelocity(btVector3(entity->v.avelocity.x, entity->v.avelocity.y, entity->v.avelocity.z));
}

bmodObject::~bmodObject() {
	MF_Log("dinit");
	entities.clear();
	g_bt_dynamicsWorld->removeRigidBody(rigidBody);
	delete rigidBody->getMotionState();
	delete rigidBody;
	//delete shape;
}

//bmod motion state (move callback)

bmodMotionState::bmodMotionState(bmodObject * obj) {
	this->obj = obj;
}

bmodMotionState::~bmodMotionState() {

}

void bmodMotionState::getWorldTransform(btTransform &worldTrans) const {
	worldTrans = btTransform(btQuaternion(0, 0, 0, 1));
}

void MatrixAngles(const btMatrix3x3& matrix, Vector& angles) {
	float forward[3];
	float left[3];
	float up[3];

	//
	// Extract the basis vectors from the matrix. Since we only need the Z
	// component of the up vector, we don't get X and Y.
	//
	forward[0] = matrix[0][0];
	forward[1] = matrix[1][0];
	forward[2] = matrix[2][0];
	left[0] = matrix[0][1];
	left[1] = matrix[1][1];
	left[2] = matrix[2][1];
	up[2] = matrix[2][2];

	float xyDist = sqrtf(forward[0] * forward[0] + forward[1] * forward[1]);

	// enough here to get angles?
	if(xyDist > 0.001f) {
		// (yaw) y = ATAN( forward.y, forward.x ); -- in our space, forward is the X axis
		angles[1] = RAD2DEG * atan2f(forward[1], forward[0]);

		// (pitch) x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
		angles[0] = RAD2DEG * atan2f(-forward[2], xyDist);

		// (roll) z = ATAN( left.z, up.z );
		angles[2] = RAD2DEG * atan2f(left[2], up[2]);
	}
	else	// forward is mostly Z, gimbal lock-
	{
		// (yaw) y = ATAN( -left.x, left.y ); -- forward is mostly z, so use right for yaw
		angles[1] = RAD2DEG * atan2f(-left[0], left[1]);

		// (pitch) x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
		angles[0] = RAD2DEG * atan2f(-forward[2], xyDist);

		// Assume no roll in this case as one degree of freedom has been lost (i.e. yaw == roll)
		angles[2] = 0;
	}
}

void bmodMotionState::setWorldTransform(const btTransform &worldTrans) {
	if(obj->isImmovable())
		return;

	Vector origin = Vector((float*)(worldTrans.getOrigin().m_floats));

	Vector angles;
	worldTrans.getBasis().getEulerZYX(angles[1], angles[0], angles[2], 1); // Yaw (Z), -Pitch(Y), Roll (X)
	angles[0] = -angles[0];
	angles = angles * RAD2DEG;

	//Vector angles2;
	//MatrixAngles(worldTrans.getBasis(), angles2);

	btRigidBody * body = obj->getRigidBody();
	Vector velocity = Vector((float*)body->getLinearVelocity().m_floats);
	Vector avelocity = Vector((float*)body->getAngularVelocity().m_floats) * RAD2DEG;

	std::list<int> * entities = obj->getEntities();
	for(std::list<int>::iterator it = entities->begin(); it != entities->end(); ++it) {
		edict_t * entity = INDEXENT(*it);
		SET_ORIGIN(entity, origin);
		entity->v.angles = angles;
		entity->v.velocity = velocity;
		entity->v.avelocity = avelocity;
	}
}