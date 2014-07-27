#include "object.h"

#include "sdk\amxxmodule.h"

/*bmodObject::bmodObject() : bmodObject("BMOD/sphere/1") {
	MF_Log("empty init");
}*/

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
	shape = new btBoxShape(btVector3(24, 24, 24));

	bmodMotionState* objectMotionState = new bmodMotionState(this);
	btRigidBody::btRigidBodyConstructionInfo objectRigidBodyCI(0, objectMotionState, shape, btVector3(0, 0, 0));
	rigidBody = new btRigidBody(objectRigidBodyCI);
	g_bt_dynamicsWorld->addRigidBody(rigidBody);
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
	delete shape;
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

void bmodMotionState::setWorldTransform(const btTransform &worldTrans) {
	if(obj->isImmovable())
		return;

	Vector origin = Vector((float*)(worldTrans.getOrigin().m_floats));

	Vector angles;
	worldTrans.getBasis().getEulerZYX(angles[1], angles[0], angles[2]); // Yaw (Z), -Pitch(Y), Roll (X)
	angles[0] = -angles[0];
	angles = angles * RAD2DEG;

	btRigidBody * body = obj->getRigidBody();
	Vector velocity = Vector((float*)body->getLinearVelocity().m_floats);
	Vector avelocity = Vector((float*)body->getAngularVelocity().m_floats);

	std::list<int> * entities = obj->getEntities();
	for(std::list<int>::iterator it = entities->begin(); it != entities->end(); ++it) {
		edict_t * entity = INDEXENT(*it);
		SET_ORIGIN(entity, origin);
		entity->v.angles = angles;
		entity->v.velocity = velocity;
		entity->v.avelocity = avelocity;
	}
}