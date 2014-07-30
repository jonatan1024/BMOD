#include "object.h"
#include "model.h"

#include "sdk\amxxmodule.h"

bmodObject::bmodObject(const char * model, float mass) {
	//MF_Log("full init %s", model);
	if(!getModelShape(model, &shape)) {
		MF_Log("BAD model! (%s)", model);
		//leaking, but fuck it, this shouldn't be happening at the first place
		//shape = new btBoxShape(btVector3(32, 32, 32));
		shape = new btEmptyShape();
	}

	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);
	bmodMotionState* objectMotionState = new bmodMotionState(this);
	btRigidBody::btRigidBodyConstructionInfo objectRigidBodyCI(mass, objectMotionState, shape, inertia);
	rigidBody = new btRigidBody(objectRigidBodyCI);

	//rigidBody->setUserPointer(this);

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

btRigidBody * bmodObject::getRigidBody() {
	return rigidBody;
}

/*
copy entity's position&stuff to rigidbody
*/
void bmodObject::update() {
	if(entities.size() == 0)
		return;

	//TODO: use getWorldTransform instead!
	edict_t * entity = INDEXENT(entities.front());

	btTransform worldTrans(btQuaternion(0, 0, 0, 1), btVector3(entity->v.origin.x, entity->v.origin.y, entity->v.origin.z));
	worldTrans.getBasis().setEulerZYX(entity->v.angles.z / RAD2DEG, -entity->v.angles.x / RAD2DEG, entity->v.angles.y / RAD2DEG);

	rigidBody->setWorldTransform(worldTrans);

	rigidBody->setLinearVelocity(btVector3(entity->v.velocity.x, entity->v.velocity.y, entity->v.velocity.z));
	rigidBody->setAngularVelocity(btVector3(entity->v.avelocity.x / RAD2DEG, entity->v.avelocity.y / RAD2DEG, entity->v.avelocity.z / RAD2DEG));
}

bmodObject::~bmodObject() {
	//MF_Log("dinit");
	entities.clear();
	g_bt_dynamicsWorld->removeRigidBody(rigidBody);
	delete rigidBody->getMotionState();
	delete rigidBody;
}

//bmod motion state (move callback)

bmodMotionState::bmodMotionState(bmodObject * obj) {
	this->obj = obj;
}

bmodMotionState::~bmodMotionState() {

}

void bmodMotionState::getWorldTransform(btTransform &worldTrans) const {
	if(obj->getEntities()->size() == 0) {
		worldTrans = btTransform(btQuaternion(0, 0, 0, 1));
		return;
	}

	edict_t * entity = INDEXENT(obj->getEntities()->front());

	worldTrans = btTransform(btQuaternion(0, 0, 0, 1), btVector3(entity->v.origin.x, entity->v.origin.y, entity->v.origin.z));
	worldTrans.getBasis().setEulerZYX(entity->v.angles.z / RAD2DEG, -entity->v.angles.x / RAD2DEG, entity->v.angles.y / RAD2DEG);
}

void bmodMotionState::setWorldTransform(const btTransform &worldTrans) {
	//don't affect immovable objects
	if(obj->getRigidBody()->isStaticOrKinematicObject())
		return;

	Vector origin = Vector((float*)(worldTrans.getOrigin().m_floats));

	//FIXME: gimbal lock problem, rewrite matrix -> euler from scratch
	Vector angles;
	worldTrans.getBasis().getEulerZYX(angles[1], angles[0], angles[2], 1); // Yaw (Z), -Pitch(Y), Roll (X)
	angles[0] = -angles[0];
	angles = angles * RAD2DEG;
	
	btRigidBody * body = obj->getRigidBody();
	Vector velocity = Vector((float*)body->getLinearVelocity().m_floats);
	Vector avelocity = Vector((float*)body->getAngularVelocity().m_floats) * RAD2DEG;

	std::list<int> * entities = obj->getEntities();
	for(std::list<int>::iterator it = entities->begin(); it != entities->end(); ++it) {
		edict_t * entity = INDEXENT(*it);
		SET_ORIGIN(entity, origin);
		entity->v.angles = angles;
		//TODO: someday fix sending velocities before deactivation
		entity->v.velocity = velocity;
		entity->v.avelocity = avelocity;
	}
}